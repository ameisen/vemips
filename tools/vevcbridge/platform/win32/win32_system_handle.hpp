#pragma once

namespace buildcarbide {
	namespace detail {
		using handle_t = void*;

		extern no_throw void close_handle(handle_t& handle, uintptr_t invalid_value) noexcept;
	}

	// TODO : implement reference counting with std::shared_ptr and allow copy-constructors
	template <uintptr_t InvalidValue>
	class system_handle final {
		using handle_t = detail::handle_t;

	public:
		static constexpr uintptr_t invalid = uintptr_t(InvalidValue);

	private:
		handle_t handle_ = handle_t(invalid);

		no_throw void close_handle() noexcept {
			detail::close_handle(handle_, invalid);
		}

	public:
		no_throw system_handle() noexcept = default;

		// ReSharper disable once CppNonExplicitConvertingConstructor
		no_throw system_handle(const handle_t handle) noexcept : handle_(handle) {}

		system_handle(const system_handle &) = delete;

		no_throw system_handle(system_handle &&handle) noexcept : handle_(handle.handle_) {
			handle.handle_ = handle_t(invalid);
		}

		no_throw ~system_handle() noexcept {
			close_handle();
		}

		no_throw system_handle &operator = (const handle_t handle) noexcept {
			close_handle();
			handle_ = handle;
			return *this;
		}

		system_handle &operator = (const system_handle &) = delete;

		no_throw system_handle &operator = (system_handle &&handle) noexcept {
			auto sub_handle = handle.handle_;
			handle.handle_ = handle.invalid;
			return this = sub_handle;
		}

		no_throw handle_t* initialize() noexcept {
			check(handle_ == handle_t(invalid));
			return &handle_;
		}

		explicit no_throw operator handle_t() const noexcept {
			return handle_;
		}

		explicit no_throw operator bool() const noexcept {
			return handle_ != handle_t(invalid); // NOLINT(performance-no-int-to-ptr)
		}
	};
}
