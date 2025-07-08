// Copyright 2016-2017 Digital Carbide / Michael Kuklinski. All Rights Reserved.

#include "pch.hpp"
#include "debugger.hpp"

#include <cassert>

#include <bit>

#include "elf/elf.hpp"
#include "mips/system.hpp"
#include "mips/coprocessor/coprocessor1/coprocessor1.hpp"
#include "platform/platform.hpp"

#include <WS2tcpip.h>
#include <WinSock2.h>

using namespace mips;

namespace {
	static std::array<char, 2> checksum_to_chars(const uint32 sum) {
		xassert(sum <= std::numeric_limits<uint8>::max());

		const auto nibble_to_char = [](const uint8_t nibble) {
			if (nibble < 10) {
				return char('0' + nibble);
			}
			else {
				return char('a' + uint8_t(nibble - 10));
			}
		};

		const uint8_t low_nibble = sum & 0xF;
		const uint8_t hi_nibble = (sum >> 4) & 0xF;

		return { nibble_to_char(hi_nibble), nibble_to_char(low_nibble) };
	}

	static WSADATA g_wsa_data;

	static std::vector<char> assemble_packet(const std::span<const char> payload) {
		std::vector<char> packet;
		packet.resize(payload.size() + 4);
		packet[0] = '$';
		
		//calculate checksum
		uint32 sum = 0;
		for (const char v : payload) {
			sum += v;
		}
		sum %= 256;

		const auto checksum_chars = checksum_to_chars(sum);

		memcpy(&packet[1], payload.data(), payload.size());

		packet[packet.size() - 3] = '#';
		packet[packet.size() - 2] = checksum_chars[0];
		packet[packet.size() - 1] = checksum_chars[1];

		return packet;
	}

	static bool begins(const std::string_view str, const std::string_view ref) {
		if (str.length() < ref.length()) {
			return false;
		}

		for (size_t i = 0; i < ref.length(); ++i) {
			if (str[i] != ref[i]) {
				return false;
			}
		}

		return true;
	}

	struct debugger_init final {
		debugger_init() {
			if (WSAStartup(MAKEWORD(2, 2), &g_wsa_data) != 0) [[unlikely]] {
				throw std::exception("Failed to initialize WinSock");
			}
		}
	};
}

debugger::debugger(const uint16 port, mips::system &sys) : m_system(sys) {
	// ReSharper disable once CppDeclaratorNeverUsed
	static debugger_init init_singleton = {};

	constexpr addrinfo hints = {
		.ai_flags = AI_PASSIVE,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = IPPROTO_TCP
	};

	addrinfo *result = nullptr;
	char port_str[std::numeric_limits<uint16>::digits10 + 2];
	*fmt::format_to(port_str, "{}", port) = '\0';
	int res = getaddrinfo(nullptr, port_str, &hints, &result);
	if (res != 0 || result == nullptr) {
		throw std::exception("failed to configure socket");
	}

	m_ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	try {
		if (m_ListenSocket == INVALID_SOCKET) {
			freeaddrinfo(result);
			throw std::exception("Could not create listener socket");
		}

		res = bind(m_ListenSocket, result->ai_addr, int(result->ai_addrlen));
		if (res == SOCKET_ERROR) {
			closesocket(m_ListenSocket);
			m_ListenSocket = INVALID_SOCKET;
			throw std::exception("socket bind failed");
		}

		freeaddrinfo(result);

		DWORD value = 1;
		setsockopt(m_ListenSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&value), sizeof(value));
		value = 0;
		setsockopt(m_ListenSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&value), sizeof(value));
	}
	catch (...) {
		closesocket(m_ListenSocket);
		throw;
	}

	res = listen(m_ListenSocket, SOMAXCONN);
	if (res == SOCKET_ERROR) {
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		throw std::exception("socket listen failed");
	}

	m_ServerThread = std::thread([this]() {
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

		for (;;) {
			m_ClientSocket = accept(m_ListenSocket, nullptr, nullptr);

			if (m_ClientSocket == INVALID_SOCKET) {
				continue;
			}

			m_ack_mode = true;
			m_non_stop = false;

			DWORD value = 1;
			setsockopt(m_ClientSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&value), sizeof(value));
			value = 0;
			setsockopt(m_ClientSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&value), sizeof(value));

			bool in_packet = false;
			unsigned remaining_checksum = 0;

			static constexpr size_t max_buffer_size = 8192;
			std::vector<char> buffer;
			while (m_ClientSocket != INVALID_SOCKET) {
				char temp_buffer[512];
				int res = recv(m_ClientSocket, temp_buffer, sizeof(temp_buffer), 0);
				if (res <= 0) {
					closesocket(m_ClientSocket);
					m_ClientSocket = INVALID_SOCKET;
					break;
				}

				char *tbuffer = temp_buffer;

				if (tbuffer[0] == '+' || tbuffer[0] == '-') { // on - we should resent the last packet.
					// received an ack.
					--res;
					++tbuffer;
				}
				if (res == 0) {
					continue;
				}

				if (!in_packet && tbuffer[0] == '\x3') {
					// stop execution of all threads
					fmt::println("Packet: Ctrl-C");
					handle_stop();
					static const constexpr std::array ok = { 'S', ' ', '1', '1' };
					auto out_packet = assemble_packet(ok);
					xassert(out_packet.size() <= std::numeric_limits<int>::max());
					send(m_ClientSocket, out_packet.data(), int(out_packet.size()), 0);
					std::string out;
					out.resize(out_packet.size());
					memcpy(out.data(), out_packet.data(), out_packet.size());
					fmt::println("Sent: {}", out);
					++tbuffer;
					--res;
				}
				else if (!in_packet && tbuffer[0] != '$') {
					// this socket is sending us junk.
					closesocket(m_ClientSocket);
					m_ClientSocket = INVALID_SOCKET;
					break;
				}

				if (buffer.size() >= max_buffer_size) {
					// We won't handle inputs larger than 'max_buffer_size'
					closesocket(m_ClientSocket);
					m_ClientSocket = INVALID_SOCKET;
					break;
				}

				for (int i = 0; i < res; ++i) {
					buffer.push_back(tbuffer[i]);

					if (tbuffer[i] == '$' && !in_packet) {
						in_packet = true;
					}
					else if (tbuffer[i] == '#' && in_packet) {
						in_packet = false;
						remaining_checksum = 2;
					}
					else if (!in_packet && remaining_checksum) {
						--remaining_checksum;

						if (!remaining_checksum) {
							// Otherwise, we need to handle the packet.
							std::vector<char> response;

							if (m_ack_mode) {
								if (handle_packet(buffer, response)) {
									auto out_packet = assemble_packet(response);

									constexpr char plus = '+';
									send(m_ClientSocket, &plus, 1, 0);
									xassert(out_packet.size() <= std::numeric_limits<int>::max());
									send(m_ClientSocket, out_packet.data(), int(out_packet.size()), 0);
									
									std::string out;
									out.resize(out_packet.size());
									memcpy(out.data(), out_packet.data(), out_packet.size());
									fmt::println("Sent: {}", out);
								}
								else {
									constexpr char minus = '-';
									send(m_ClientSocket, &minus, 1, 0);
								}
							}
							else {
								handle_packet(buffer, response);

								auto out_packet = assemble_packet(response);

								xassert(out_packet.size() <= std::numeric_limits<int>::max());
								send(m_ClientSocket, out_packet.data(), int(out_packet.size()), 0);

								std::string out;
								out.resize(out_packet.size());
								memcpy(out.data(), out_packet.data(), out_packet.size());
								fmt::println("Sent: {}", out);
							}
							buffer.clear();
						}
					}
				}
			}
		}
	});
}

debugger::~debugger() {
	if (m_ServerThread.joinable()) {
		m_ServerThread.join();
	}

	if (m_ListenSocket != INVALID_SOCKET) {
		closesocket(m_ListenSocket);
	}
}

namespace {
	static bool test_checksum(const std::vector<char> & __restrict packet) {
		const std::array checksum = { packet[packet.size() - 2], packet[packet.size() - 1] };

		const size_t packet_len = packet.size() - 3;

		uint32 sum = 0;
		for (size_t i = 1; i < packet_len; ++i) {
			sum += packet[i];
		}
		sum %= 256;

		const auto checksum_chars = checksum_to_chars(sum);

		return checksum == checksum_chars;
	}
}

bool debugger::handle_packet(const std::vector<char> & __restrict packet, std::vector<char> & __restrict response) {
	fmt::println("Packet: {}", std::string_view{packet.data(), packet.size()});

	if (!test_checksum(packet)) {
		return false;
	}

	const auto send_response = [&](const std::string_view str) {
		response.insert(response.end(), str.begin(), str.end());
	};

	const auto hash_iterator = std::find(packet.begin() + 1, packet.end(), '#');
	auto in = std::string{ std::string_view{&packet[1], size_t((hash_iterator - packet.begin()) - 1)} };

	std::vector<std::string> tokens;

	const auto get_token = [&](const size_t i) -> std::string_view {
		if (i >= tokens.size()) {
			return "";
		}
		return tokens[i];
	};

	{
		const auto is_delim = [](const char c) -> bool {
			return isspace(c) || c == ';' || c == ':';
		};

		std::string token;
		bool in_string = false;
		bool in_escape = false;
		while (!in.empty()) {
			while (!in.empty() && (in_string || !is_delim(in.front()))) {
				if (!in_escape && in.front() == '\\') {
					in_escape = true;
				}
				else {
					char c = in.front();
					if (in_escape) {
						in_escape = false;
						switch (c) {
							case 'n': c = '\n'; break;
							case 't': c = '\t'; break;
							case 'v': c = '\v'; break;
							case 'b': c = '\b'; break;
							case 'r': c = '\r'; break;
							case 'f': c = '\f'; break;
							case 'a': c = '\a'; break;
							case '\\': c = '\\'; break;
							case '?': c = '?'; break;
							case '\'': c = '\''; break;
							case '\"': c = '\"'; break;
							case '0': c = '\0'; break;
							default: {
								in.erase(0, 1);
								continue;
							}
						}
					}
					else {
						if (c == '\"') {
							in_string = !in_string;
							in.erase(0, 1);
							continue;
						}
					}

					token.push_back(c);
					in.erase(0, 1);
				}
			}
			while (!in.empty() && is_delim(in.front())) {
				in.erase(0, 1);
			}
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
		}
	}

	if (get_token(0) == "QStartNoAckMode") {
		m_ack_mode = false;
		send_response("OK");
	}
	else if (get_token(0) == "qC") {
		if (m_active_thread == uint32(-1)) {
			m_active_thread = 1;
		}

		send_response("QC ");
		char buffer[std::numeric_limits<decltype(m_active_thread)>::digits10 + 2];
		*fmt::format_to(buffer, "{:x}", m_active_thread) = '\0';
		send_response(buffer);
	}
	else if (get_token(0) == "qHostInfo") {
		send_response("triple:mipsel;");
		send_response("ptrsize:4;");
		send_response("watchpoint_exceptions_received:before;");
		send_response("endian:little;");
		send_response("hostname:76656d69707300;");
	}
	else if (get_token(0) == "qGetWorkingDir") {
		// send empty response
	}
	else if (get_token(0) == "qQueryGDBServer") {	 
		// send empty response
	}
	else if (get_token(0) == "qLaunchGDBServer") {
		//SendResponse("port:21212;");
	}
	else if (get_token(0) == "qSupported") {
		// multiprocess
		// xmlRegisters
		send_response("qRelocInsn+;QAllow+;QPassSignals+;QThreadEvents+;QCatchSyscalls+;QStartNoAckMode+;ConditionalBreakpoints+;swbreak-;hwbreak+;QThreadEvents+;vContSupported+;");
	}
	else if (get_token(0) == "QThreadSuffixSupported") {
		// I don't know what this is.
	}
	else if (get_token(0) == "QListThreadsInStopReply") {
		// I don't know what this is.
	}
	else if (get_token(0) == "VCont?") {
		send_response("vCont;c;t;s;");
	}
	else if (get_token(0) == "qVAttachOrWaitSupported") {
		// I don't know what this is.
	}
	else if (get_token(0) == "qProcessInfo") {
		send_response("pid:1;");
		send_response("parent-pid:1;");
		send_response("real-uid:1;");
		send_response("real-gid:1;");
		send_response("effective-uid:1;");
		send_response("effective-gid:1;");
		send_response("triple:mipsel;");
		send_response("ptrsize:4;");
		send_response("endian:little;");
	}
	else if (get_token(0) == "qfThreadInfo") {
		send_response("m ");
		// TODO all threads should be reported here. We only have one thread presently.

		char buffer[std::numeric_limits<decltype(m_threads[0])>::digits10 + 2];
		*fmt::format_to(buffer, "{:x}", m_threads[0]) = '\0';
		send_response(buffer);
	}
	else if (get_token(0) == "qsThreadInfo") {
		send_response("l");
	}
	else if (get_token(0) == "?") {
		// Basically, the program must start halted. Thus, we need a debugger flag.
		send_response("S 36");
	}
	else if (get_token(0) == "k") {
		send_response("OK");
		handle_kill();
	}
	else if (get_token(0) == "vCont?") {
		send_response("vCont;c;t;s;");
	}
	else if (begins(get_token(0), "qRegisterInfo")) {
		//  sr; lo; hi; bad; cause; pc; 32 floating-point registers; fsr; fir; fp

		// register info query.
		uint32 register_number;
		std::sscanf(get_token(0).data(), "qRegisterInfo%x", &register_number);
		char buffer[1024];
		switch (register_number) {
			case 0:
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:zero;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:zero;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 1:
			case 2:
			case 3:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				*fmt::format_to(
					buffer,
					"name:r{};bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 4: // a0
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:arg1;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:arg1;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 5: // a1
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:arg2;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:arg2;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 6: // a2
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:arg3;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:arg3;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 7: // a3
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:arg4;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:arg4;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 28: // gp
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:gp;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:gp;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 29:
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:sp;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:sp;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 30:
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:fp;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:fp;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 31: // ra
				*fmt::format_to(
					buffer,
					"name:r{};alt-name:ra;bitsize:32;offset:{};encoding:uint;format:hex;set:General Purpose Registers;gcc:{};dwarf:{};generic:ra;",
					register_number, register_number * 4, register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
				*fmt::format_to(
					buffer,
					"name:f{};bitsize:64;offset:{};encoding:ieee754;format:float;set:Floating Point Registers;gcc:{};dwarf:{};",
					register_number - 32, (32 * 4) + (register_number * 8), register_number, register_number
				) = '\0';
				send_response(buffer); break;
			case 64: // IP
				*fmt::format_to(
					buffer,
					"name:pc;alt-name:pc;bitsize:32;encoding:uint;format:hex;set:General Purpose Registers;generic:pc;"
				) = '\0';
				send_response(buffer); break;
			default:
				send_response("E45");
		}
	}
	else if (begins(get_token(0), "Hg")) {
		// register info query.
		uint32 thread_id;
		std::sscanf(get_token(0).data(), "Hg%x", &thread_id);
		m_active_thread = thread_id;
		send_response("OK");
	}
	else if (begins(get_token(0), "p")) {
		// register info query.
		uint32 register_id;
		std::sscanf(get_token(0).data(), "p%x", &register_id);

		char buffer[64];
		if (register_id < 32) {
			// gpr
			const uint32 value = m_system.get_processor()->get_register<uint32>(register_id);
			*fmt::format_to(buffer, "{:08x}", std::byteswap(value)) = '\0';
		}
		else if (register_id < 64) {
			// fpu
			const uint64 value = static_cast<coprocessor1* __restrict>(m_system.get_processor()->get_coprocessor(1))->get_register<uint64>(register_id - 32);
			*fmt::format_to(buffer, "{:016x}", std::byteswap(value)) = '\0';
		}
		else if (register_id == 64) {
			const uint32 value = m_system.get_processor()->get_program_counter();
			*fmt::format_to(buffer, "{:08x}", std::byteswap(value)) = '\0';
		}
		else {
			// error, send nothing.
		}

		send_response(buffer);
	}
	else if (get_token(0) == "qStructuredDataPlugins") {
		// no idea
	}
	else if (begins(get_token(0), "c")) {
		uint32 address = m_system.get_processor()->get_program_counter();
		if (!get_token(1).empty())
		{
			std::sscanf(get_token(1).data(), "%x", &address);
			m_system.get_processor()->set_program_counter(address);
		}
		this->m_paused = false;
		send_response("OK");
	}
	else if (get_token(0) == "jThreadExtendedInfo") {
		// no idea
	}
	else if (get_token(0) == "jThreadsInfo") {
		// no idea
	}
	else if (get_token(0) == "qSymbol") {
		if (tokens.size() == 1) {
			send_response("OK");
		}
		else {
			// otherwise they're setting symbols
			__debugbreak();
		}
	}
	else if (get_token(0) == "qMemoryRegionInfo") {
		// no idea
	}
	else if (get_token(0)[0] == 'x') {
		// The command we get is malformed?
	}
	else if (get_token(0)[0] == 'm') {
		// we're reading memory.
		uint32 addr = 0;
		uint32 length = 0;
		std::sscanf(&get_token(0)[1], "%x,%x", &addr, &length);
		
		bool has_response = false;

		for (uint32 i = 0; i < length; ++i) {
			if (auto *ptr = m_system.get_processor()->mem_fetch_debugger<uint8>(addr)) {
				char buffer[3];
				*fmt::format_to(buffer, "{:02x}", *ptr) = '\0';
				send_response(buffer);
				has_response = true;
			}
			else {
				break;
			}

			++addr;
		}

		if (!has_response) {
			send_response("E 00");
		}
	}
	else if (get_token(0)[0] == 'Z') {
		// The command we get is malformed?
		uint32 type;
		uint32 address;
		uint32 kind;
		std::sscanf(get_token(0).data(), "Z%x,%x,%x", &type, &address, &kind);

		{
			std::unique_lock _bplock(m_breakpoint_lock);
			m_breakpoints.emplace_back(type, address, kind);
		}
		send_response("OK");
	}
	else if (get_token(0)[0] == 'z') {
		// The command we get is malformed?
		uint32 type;
		uint32 address;
		uint32 kind;
		std::sscanf(get_token(0).data(), "z%x,%x,%x", &type, &address, &kind);

		{
			std::unique_lock _bplock(m_breakpoint_lock);
			breakpoint_t bpt{ type, address, kind };
			if (const auto fiter = std::find(m_breakpoints.begin(), m_breakpoints.end(), bpt); fiter != m_breakpoints.end()) {
				m_breakpoints.erase(fiter);
			}
		}

		send_response("OK");
	}
	else if (begins(get_token(0), "vCont")) {
		// register info query.
		if (get_token(1) == "c") {
			// just continue
			m_paused = false;
		}
		else if (get_token(1) == "t") {
			// we aren't handling this yet TODO
		}
		else if (get_token(1) == "s") {
			uint32 thread;
			std::sscanf(get_token(2).data(), "%x", &thread);
			std::unique_lock _bplock(m_breakpoint_lock);
			m_step[thread] = 2;
			m_paused = false;
		}

		send_response("OK");
	}
	else if (get_token(0)[0] == 'H') {
		// register info query.
		char type = 'g';
		uint32 thread = -1;
		std::sscanf(get_token(0).data(), "H%c%d", &type, &thread);
		if (type == 'c') {
			m_thread_next_sc = thread;
		}
		else {
			m_thread_next_g = thread;
		}
		send_response("OK");
	}
	else if (begins(get_token(0), "s")) {
		if (get_token(1).empty()) {
			// continue
		}
		else {
			// step TODO
		}
		m_paused = false;
		send_response("OK");
	}
	else {
		// no response, unknown packet.
	}

	return true;
}

void debugger::wait() {
	std::vector<char> response;
	const auto send_response = [&](std::string_view str) {
		response.insert(response.end(), str.begin(), str.end());
	};

	--m_threads_to_pause;
	fmt::println("Thread Interrupted (remaining threads {})", m_threads_to_pause.load());
	
	std::unique_lock _lock(m_wait_lock);
	
	if (!m_pending_breakpoints.empty()) {
		const auto [thread_id, breakpoint] = m_pending_breakpoints.back();
		m_pending_breakpoints.pop_back();

		// report back the breakpoint being hit.
		if (std::get<0>(breakpoint) == uint32(-1)) { // step
			send_response("S 05 ");
		}
		else {
			send_response("T 05 "); // TARGET_SIGNAL_TRAP
			char buffer[32];
			*fmt::format_to(buffer, "thread:{:x}", thread_id) = '\0';
			send_response(buffer);
			*fmt::format_to(buffer, "{:02x}:{:08x};", 64, std::byteswap(m_system.get_processor()->get_program_counter())) = '\0';
			send_response(buffer);
			*fmt::format_to(buffer, "{:02x}:{:08x};", 0x1d, std::byteswap(m_system.get_processor()->get_register<uint32>(0x1d))) = '\0';
			send_response(buffer);
			*fmt::format_to(buffer, "{:02x}:{:08x};", 0x1f, std::byteswap(m_system.get_processor()->get_register<uint32>(0x1f))) = '\0';
			send_response(buffer);
			if (std::get<0>(breakpoint) == 0) {
				send_response("swbreak:;");
			}
			else if (std::get<0>(breakpoint) == 1) {
				send_response("hwbreak:;");
			}
		}

		auto out_packet = assemble_packet(response);

		xassert(out_packet.size() <= std::numeric_limits<int>::max());
		send(m_ClientSocket, out_packet.data(), int(out_packet.size()), 0);

		if (out_packet.back() != '\0') {
			out_packet.push_back('\0');
		}
		fmt::println("Sent: {}", std::string_view{out_packet.data(), out_packet.size()});
	}
	
	while (m_paused) {
		Sleep(16);
	}
}

void debugger::handle_kill() {
	m_should_kill = true;
	if (!m_paused) {
		if (m_system.is_debugger_owned()) {
			handle_stop();
			m_paused = false;
		}
	}
	else {
		m_paused = false;
	}
}

void debugger::handle_stop() {
	m_threads_to_pause = 1; // todo when threads.
	m_paused = true;

	while (m_threads_to_pause) {
		Sleep(16);
	}
}

bool debugger::should_interrupt_execution() {
	if (m_paused || m_should_kill) {
		return true;
	}

	const uint32 thread = 1;
	const uint32 pc = m_system.get_processor()->get_program_counter();
	std::unique_lock _bplock(m_breakpoint_lock);

	// check for breakpoints
	for (auto &breakpoint : m_breakpoints) {
		if (pc == std::get<1>(breakpoint)) {
			m_pending_breakpoints.emplace_back(thread, breakpoint); // TODO thread
			m_paused = true;
			return true;
		}
	}

	// check for step
	if (const uint32 step_point = m_step[thread]; step_point > 0) {
		--m_step[thread];
		if (step_point == 1) {
			const breakpoint_t break_point{ uint32(-1), pc, 0 };
			m_pending_breakpoints.emplace_back(thread, break_point); // TODO thread
			m_paused = true;
			return true;
		}
	}

	return false;
}
