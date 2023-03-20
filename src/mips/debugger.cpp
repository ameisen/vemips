// Copyright 2016-2017 Digital Carbide / Michael Kuklinski. All Rights Reserved.

#include "pch.hpp"

#include "debugger.hpp"
#include "elf/elf.hpp"
#include "mips/system.hpp"
#include "mips/coprocessor/coprocessor1/coprocessor1.hpp"

#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace mips;

namespace {
	static std::array<char, 2> checksum_to_chars(uint32 sum) {
		char chars[3];
		sprintf(chars, "%02x", sum);
		return { chars[0], chars[1] };
	}

	static WSADATA g_wsa_data;

	static std::vector<char> assemble_packet(const std::vector<char> & __restrict payload) {
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

		memcpy(packet.data() + 1, payload.data(), payload.size());

		packet[packet.size() - 3] = '#';
		packet[packet.size() - 2] = checksum_chars[0];
		packet[packet.size() - 1] = checksum_chars[1];

		return packet;
	}
}

static bool begins(const std::string & __restrict str, const std::string & __restrict ref) {
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

debugger::debugger(const uint16 port, mips::system &sys) : m_system(sys) {
	static bool once = true;
	if (once) {
		if (WSAStartup(MAKEWORD(2, 2), &g_wsa_data) != 0) {
			throw std::string("Failed to initialize WinSock");
		}
		once = false;
	}

	constexpr addrinfo hints = {
		.ai_flags = AI_PASSIVE,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = IPPROTO_TCP
	};

	struct addrinfo *result = nullptr;
	char port_str[std::numeric_limits<uint16>::digits10 + 2];
	sprintf(port_str, "%u", port);
	int res = getaddrinfo(nullptr, port_str, &hints, &result);
	if (res != 0 || result == nullptr) {
		throw std::string("failed to configure socket");
	}

	m_ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		throw std::string("Could not create listener socket");
	}

	res = bind(m_ListenSocket, result->ai_addr, int(result->ai_addrlen));
	if (res == SOCKET_ERROR) {
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		throw std::string("socket bind failed");
	}

	freeaddrinfo(result);

	DWORD value = 1;
	setsockopt(m_ListenSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&value, sizeof(value));
	value = 0;
	setsockopt(m_ListenSocket, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof(value));

	res = listen(m_ListenSocket, SOMAXCONN);
	if (res == SOCKET_ERROR) {
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		throw std::string("socket listen failed");
	}

	m_ServerThread = std::thread([this]() {
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

		for (;;) {
			m_ClientSocket = accept(m_ListenSocket, nullptr, nullptr);

			m_ack_mode = true;
			m_non_stop = false;

			DWORD value = 1;
			setsockopt(m_ClientSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&value, sizeof(value));
			value = 0;
			setsockopt(m_ClientSocket, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof(value));

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
					printf("Packet: Ctrl-C\n");
					handle_stop();
					static const std::vector<char> OK = { 'S', ' ', '1', '1' };
					auto out_packet = assemble_packet(OK);
					xassert(out_packet.size() <= std::numeric_limits<int>::max());
					send(m_ClientSocket, out_packet.data(), int(out_packet.size()), 0);
					std::string out;
					out.resize(out_packet.size());
					memcpy(out.data(), out_packet.data(), out_packet.size());
					printf("Sent: %s\n", out.data());
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
									printf("Sent: %s\n", out.data());
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
								printf("Sent: %s\n", out.data());
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
	m_ServerThread.join();
}

static bool test_checksum(const std::vector<char> & __restrict packet) {
	const char checksum[2] = { packet[packet.size() - 2], packet[packet.size() - 1] };

	const size_t packet_len = packet.size() - 3;

	uint32 sum = 0;
	for (size_t i = 1; i < packet_len; ++i) {
		sum += packet[i];
	}
	sum %= 256;

	const auto checksum_chars = checksum_to_chars(sum);

	return checksum[0] == checksum_chars[0] && checksum[1] == checksum_chars[1];
}

namespace {
	template <typename T>
	T endian_swap(const T & __restrict val) {
		T out = val;

		uint8 *ptr = (uint8 *)&out;
		for (size_t i = 0; i < sizeof(T) / 2; ++i) {
			std::swap(ptr[i], ptr[(sizeof(T) - 1) - i]);
		}

		return out;
	}
}

bool debugger::handle_packet(const std::vector<char> & __restrict packet, std::vector<char> & __restrict response) {
	std::vector<char> buffer = packet;
	buffer.push_back(0);
	printf("Packet: %s\n", buffer.data());

	if (!test_checksum(packet)) {
		return false;
	}

	const auto send_response = [&](std::string_view str) {
		response.insert(response.end(), str.begin(), str.end());
	};

	std::string in;
	for (int i = 1; i < packet.size(); ++i) {
		if (packet[i] == '#') {
			break;
		}
		in.push_back(packet[i]);
	}

	std::vector<std::string> tokens;

	const auto get_token = [&](size_t i) -> std::string {
		if (i >= tokens.size()) {
			return "";
		}
		return tokens[i];
	};

	{
		const auto is_delim = [](char c) -> bool {
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
							case '\0': c = '\0'; break;
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
		sprintf(buffer, "%x", m_active_thread);
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
		sprintf(buffer, "%x", m_threads[0]);
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
		sscanf(get_token(0).c_str(), "qRegisterInfo%x", &register_number);
		char buffer[1024];
		switch (register_number) {
			case 0:
				sprintf(
					buffer,
					"name:r%u;alt-name:zero;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:zero;",
					register_number, register_number * 4, register_number, register_number
				);
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
				sprintf(
					buffer,
					"name:r%u;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;",
					register_number, register_number * 4, register_number, register_number
				);
				send_response(buffer); break;
			case 4: // a0
				sprintf(
					buffer,
					"name:r%u;alt-name:arg1;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:arg1;",
					register_number, register_number * 4, register_number, register_number
				);
				send_response(buffer); break;
			case 5: // a1
				sprintf(
					buffer,
					"name:r%u;alt-name:arg2;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:arg2;",
					register_number, register_number * 4, register_number, register_number
				);
				send_response(buffer); break;
			case 6: // a2
				sprintf(
					buffer,
					"name:r%u;alt-name:arg3;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:arg3;",
					register_number, register_number * 4, register_number, register_number
				);
				send_response(buffer); break;
			case 7: // a3
				sprintf(
					buffer,
					"name:r%u;alt-name:arg4;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:arg4;",
					register_number, register_number * 4, register_number, register_number
				);
				send_response(buffer); break;
			case 28: // gp
				sprintf(
					buffer,
					"name:r%u;alt-name:gp;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:gp;",
					register_number, register_number * 4, register_number, register_number
				);
				send_response(buffer); break;
			case 29:
				sprintf(
					buffer,
					"name:r%u;alt-name:sp;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:sp;",
					register_number, register_number * 4, register_number, register_number
				);
				send_response(buffer); break;
			case 30:
				sprintf(
					buffer,
					"name:r%u;alt-name:fp;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:fp;",
					register_number, register_number * 4, register_number, register_number
				);
				send_response(buffer); break;
			case 31: // ra
				sprintf(
					buffer,
					"name:r%u;alt-name:ra;bitsize:32;offset:%u;encoding:uint;format:hex;set:General Purpose Registers;gcc:%u;dwarf:%u;generic:ra;",
					register_number, register_number * 4, register_number, register_number
				);
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
				sprintf(
					buffer,
					"name:f%u;bitsize:64;offset:%u;encoding:ieee754;format:float;set:Floating Point Registers;gcc:%u;dwarf:%u;",
					register_number - 32, (32 * 4) + (register_number * 8), register_number, register_number
				);
				send_response(buffer); break;
			case 64: // IP
				sprintf(
					buffer,
					"name:pc;alt-name:pc;bitsize:32;encoding:uint;format:hex;set:General Purpose Registers;generic:pc;"
				);
				send_response(buffer); break;
			default:
				send_response("E45");
		}
	}
	else if (begins(get_token(0), "Hg")) {
		// register info query.
		uint32 thread_id;
		sscanf(get_token(0).c_str(), "Hg%x", &thread_id);
		m_active_thread = thread_id;
		send_response("OK");
	}
	else if (begins(get_token(0), "p")) {
		// register info query.
		uint32 register_id;
		sscanf(get_token(0).c_str(), "p%x", &register_id);

		char buffer[64];
		if (register_id < 32) {
			// gpr
			const uint32 value = m_system.get_processor()->get_register<uint32>(register_id);
			sprintf(buffer, "%08x", endian_swap(value));
		}
		else if (register_id < 64) {
			// fpu
			const uint64 value = ((coprocessor1 * __restrict)m_system.get_processor()->get_coprocessor(1))->get_register<uint64>(register_id - 32);
			sprintf(buffer, "%016llx", endian_swap(value));
		}
		else if (register_id == 64) {
			const uint32 value = m_system.get_processor()->get_program_counter();
			sprintf(buffer, "%08x", endian_swap(value));
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
			sscanf(get_token(1).c_str(), "%x", &address);
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
		sscanf(&get_token(0)[1], "%x,%x", &addr, &length);
		
		bool has_response = false;

		for (uint32 i = 0; i < length; ++i) {
			if (auto *ptr = m_system.get_processor()->mem_fetch_debugger<uint8>(addr)) {
				char buffer[3];
				sprintf(buffer, "%02x", *ptr);
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
		sscanf(get_token(0).c_str(), "Z%x,%x,%x", &type, &address, &kind);

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
		sscanf(get_token(0).c_str(), "z%x,%x,%x", &type, &address, &kind);

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
			sscanf(get_token(2).c_str(), "%x", &thread);
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
		sscanf(get_token(0).c_str(), "H%c%d", &type, &thread);
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
	printf("Thread Interrupted (remaining threads %u\n", m_threads_to_pause.load());
	
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
			sprintf(buffer, "thread:%x;", thread_id);
			send_response(buffer);
			sprintf(buffer, "%02x:%08x;", 64, endian_swap(m_system.get_processor()->get_program_counter()));
			send_response(buffer);
			sprintf(buffer, "%02x:%08x;", 0x1d, endian_swap(m_system.get_processor()->get_register<uint32>(0x1d)));
			send_response(buffer);
			sprintf(buffer, "%02x:%08x;", 0x1f, endian_swap(m_system.get_processor()->get_register<uint32>(0x1f)));
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
		printf("Sent: %s\n", out_packet.data());
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
			const breakpoint_t steppoint{ uint32(-1), pc, 0 };
			m_pending_breakpoints.emplace_back(thread, steppoint); // TODO thread
			m_paused = true;
			return true;
		}
	}

	return false;
}
