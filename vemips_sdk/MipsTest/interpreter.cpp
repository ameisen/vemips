// ReSharper disable CppClangTidyClangDiagnosticCpp20Designator
// ReSharper disable CppClangTidyClangDiagnosticCpp20AttributeExtensions
#include "interpreter.hpp"

#ifndef BENCHMARK
#	define BENCHMARK "mandelbrot.inc"
#endif
#ifndef RECODE
#	define RECODE 1
#endif
#ifndef PRINT_RECODE
#	define PRINT_RECODE 0
#endif
#ifndef PRE_SCAN
# define PRE_SCAN 0
#endif

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <array>
#if __has_include(<bit>)
# include <bit>
#endif
#if __has_include(<span>)
# include <span>
#endif
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include BENCHMARK

#ifndef _MSC_VER
#	define __assume(...) __builtin_assume(__VA_ARGS__)
#	define __forceinline __attribute__((__always_inline__))
#endif

namespace std {
#if !__has_include(<bit>)
  template <class TTo, class TFrom>
  static constexpr TTo bit_cast(const TFrom &v) noexcept {
    TTo result;
    memcpy(&result, &v, sizeof(TTo));
    return result;
  }
#endif

#if !__has_include(<span>)
  template <typename T>
  class span {
    using element_type = T;
    using value_type = typename std::remove_cv<T>::type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    pointer pointer_ = nullptr;
    size_type size_ = 0;

  public:
    template <typename U, std::size_t N>
    requires(std::is_same<typename std::remove_cv<U>::type, typename std::remove_cv<T>::type>::value)
    constexpr span(std::array<U, N>& arr) noexcept :
      pointer_(arr.data()),
      size_(arr.size())
    {}

    template <typename U, std::size_t N>
    requires(std::is_same<typename std::remove_cv<U>::type, typename std::remove_cv<T>::type>::value)
    constexpr span(const std::array<U, N>& arr) noexcept :
      pointer_(arr.data()),
      size_(arr.size())
    {}

    template <typename U>
    requires(std::is_same<typename std::remove_cv<U>::type, typename std::remove_cv<T>::type>::value)
    constexpr span(std::vector<U>& vec) noexcept :
      pointer_(vec.data()),
      size_(vec.size())
    {}

    template <typename U>
    requires(std::is_same<typename std::remove_cv<U>::type, typename std::remove_cv<T>::type>::value)
    constexpr span(const std::vector<U>& vec) noexcept :
      pointer_(vec.data()),
      size_(vec.size())
    {}

    constexpr size_type size() const {
      return size_;
    }

    constexpr size_type size_bytes() const {
      return size_ * sizeof(T);
    }

    constexpr reference operator[] (size_type idx) const {
      return pointer_[idx];
    }
  };
#endif
}

namespace {
  using uint = unsigned int;

  using uint64 = uint64_t;
  using uint32 = uint32_t;
  using uint16 = uint16_t;
  using uint8 = uint8_t;

  using byte_vector = std::vector<uint8>;

  enum class token : char {
    // Main Tokens
    increment_pointer_one = '>',
    decrement_pointer_one = '<',
    increment_byte_one = '+',
    decrement_byte_one = '-',
    output_byte = '.',
    input_byte = ',',
    jump_forth = '[',
    jump_back = ']',

#if RECODE
    // Extended Tokens
    // [-]
    zero_byte = 'a',
    // > seq 4b
    increment_pointer_seq_4 = 'b',
    // < seq 4b
    decrement_pointer_seq_4 = 'c',
    // + seq 4b
    increment_byte_seq_4 = 'd',
    // - seq 4b
    decrement_byte_seq_4 = 'e',
    // > seq 2b
    increment_pointer_seq_2 = 'f',
    // < seq 2b
    decrement_pointer_seq_2 = 'g',
    // + seq 2b
    increment_byte_seq_2 = 'h',
    // - seq 2b
    decrement_byte_seq_2 = 'i',
    // > seq 1b
    increment_pointer_seq_1 = 'j',
    // < seq 1b
    decrement_pointer_seq_1 = 'k',
    // + seq 1b
    increment_byte_seq_1 = 'l',
    // - seq 1b
    decrement_byte_seq_1 = 'm',
    // ] 2b
    jump_back_2 = '}',
    // ] 1b
    jump_back_1 = '\xF2',
    // [ 2b
    jump_forth_2 = '{',
    // [ 1b //unimplemented
    jump_forth_1 = '\xF3',
    // special case [->+<] (*(ptr + 1) += *ptr; *ptr = 0
    special_case_1 = 'r',
    // special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
    special_case_2 = 's',
    // special case [-<n+>n] (*(ptr - n) += *ptr; *ptr = 0
    special_case_3 = 't',
    // special case [->n-<n] (*(ptr + n) -= *ptr; *ptr = 0
    special_case_4 = 'u',
    // special case [-<n->n] (*(ptr - n) -= *ptr; *ptr = 0
    special_case_5 = 'v',
    // [<n] while (*ptr) ptr -= n
    special_case_6 = 'w',
    // x = [>n] while (*ptr) ptr += n
    special_case_7 = 'x',

    // [->>>>>>>+<<+<<<+<<]
    // [->>+>>+<<<<]
    // [-<+>>>>+<<[-]]
#endif
  };

  static constexpr inline uint32 get_operand_size(const token tok) {
#if RECODE
    switch (tok) {
      case token::increment_pointer_seq_4:
      case token::decrement_pointer_seq_4:
      case token::increment_byte_seq_4:
      case token::decrement_byte_seq_4:
        return 4;
      case token::increment_pointer_seq_2:
      case token::decrement_pointer_seq_2:
      case token::increment_byte_seq_2:
      case token::decrement_byte_seq_2:
      case token::jump_back_2:
      case token::jump_forth_2:
        return 2;
      case token::increment_pointer_seq_1:
      case token::decrement_pointer_seq_1:
      case token::increment_byte_seq_1:
      case token::decrement_byte_seq_1:
      case token::jump_back_1:
      case token::jump_forth_1:
        return 1;
      case token::special_case_1:
      case token::special_case_2:
      case token::special_case_3:
      case token::special_case_4:
      case token::special_case_5:
      case token::special_case_6:
      case token::special_case_7:
        return 1;
      default: [[unlikely]]
        UNREACHABLE();
    }
#else
    return 0;
#endif
  }

  struct token_info final {
    token token;
    uint32 operand_size;
  };

  template <typename T> requires (sizeof(T) == sizeof(token) && std::is_trivially_copyable_v<T>)
    static constexpr inline token_info get_token_info(const T byte) {
    return { token(byte), get_operand_size(token(byte)) };
  }

  static constexpr inline token resize_jump_operand(const token tok, const uint32 new_size) {
#if RECODE
    switch (tok) {
      case token::jump_back_2: [[likely]] {
        if (check(new_size == 1)) [[likely]] {
          return token::jump_back_1;
        }
      }
      case token::jump_forth_2: [[likely]] {
        if (check(new_size == 1)) [[likely]] {
          return token::jump_forth_1;
        }
      }
      case token::jump_back_1:
      {
        if (check(new_size == 2)) [[likely]] {
          return token::jump_back_2;
        }
      }
      case token::jump_forth_1:
      {
        if (check(new_size == 2)) [[likely]] {
          return token::jump_forth_2;
        }
      }
      default: [[unlikely]]
        UNREACHABLE();
    }
#else
    UNREACHABLE();
    return tok;
#endif
  }

  static constexpr inline token resize_operand(const token tok, const uint32 new_size) {
#if RECODE
    switch (tok) {
      case token::increment_pointer_seq_4:
        switch (new_size) {
          case 2:
            return token::increment_pointer_seq_2;
          case 1:
            return token::increment_pointer_seq_1;
          default: [[unlikely]]
            check(false);
        }
      case token::decrement_pointer_seq_4:
        switch (new_size) {
          case 2:
            return token::decrement_pointer_seq_2;
          case 1:
            return token::decrement_pointer_seq_1;
          default: [[unlikely]]
            check(false);
        }
      case token::increment_byte_seq_4:
        switch (new_size) {
          case 2:
            return token::increment_byte_seq_2;
          case 1:
            return token::increment_byte_seq_1;
          default: [[unlikely]]
            check(false);
        }
      case token::decrement_byte_seq_4:
        switch (new_size) {
          case 2:
            return token::decrement_byte_seq_2;
          case 1:
            return token::decrement_byte_seq_1;
          default: [[unlikely]]
            check(false);
        }

      case token::increment_pointer_seq_2:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::increment_pointer_seq_4;
          case 1:
            return token::increment_pointer_seq_1;
          default: [[unlikely]]
            check(false);
        }
      case token::decrement_pointer_seq_2:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::decrement_pointer_seq_4;
          case 1:
            return token::decrement_pointer_seq_1;
          default: [[unlikely]]
            check(false);
        }
      case token::increment_byte_seq_2:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::increment_byte_seq_4;
          case 1:
            return token::increment_byte_seq_1;
          default: [[unlikely]]
            check(false);
        }
      case token::decrement_byte_seq_2:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::decrement_byte_seq_4;
          case 1:
            return token::decrement_byte_seq_1;
          default: [[unlikely]]
            check(false);
        }

      case token::increment_pointer_seq_1:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::increment_pointer_seq_4;
          case 2:
            return token::increment_pointer_seq_2;
          default: [[unlikely]]
            check(false);
        }
      case token::decrement_pointer_seq_1:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::decrement_pointer_seq_4;
          case 2:
            return token::decrement_pointer_seq_2;
          default: [[unlikely]]
            check(false);
        }
      case token::increment_byte_seq_1:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::increment_byte_seq_4;
          case 2:
            return token::increment_byte_seq_2;
          default: [[unlikely]]
            check(false);
        }
      case token::decrement_byte_seq_1:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::decrement_byte_seq_4;
          case 2:
            return token::decrement_byte_seq_2;
          default: [[unlikely]]
            check(false);
        }
      case token::jump_back_2:
      case token::jump_back_1:
      case token::jump_forth_2:
      case token::jump_forth_1:
        return resize_jump_operand(tok, new_size);

      case token::special_case_1:
      case token::special_case_2:
      case token::special_case_3:
      case token::special_case_4:
      case token::special_case_5:
      case token::special_case_6:
      case token::special_case_7:
        return tok; // no resizing for these

      default: [[unlikely]]
        UNREACHABLE();
    }
#else
    UNREACHABLE();
#endif
  }

  template <typename T> requires (std::is_same_v<T, char> || std::is_same_v<T, uint8>)
    static constexpr inline bool operator==(const token left, const T right) {
    return left == std::bit_cast<token>(right);
  }

  template <typename T> requires (std::is_same_v<T, char> || std::is_same_v<T, uint8>)
    static constexpr inline bool operator==(const T left, const token right) {
    return std::bit_cast<token>(left) == right;
  }

  template <typename T> requires (std::is_trivially_copyable_v<T> && sizeof(T) == 1)
    static constexpr uint8 byte_cast(T value) {
    return std::bit_cast<uint8>(value);
  }

  template <typename T> requires (std::is_enum_v<T> &&std::is_trivially_copyable_v<T>)
    static constexpr std::underlying_type_t<T> enum_cast(T value) {
    return std::bit_cast<std::underlying_type_t<T>>(value);
  }

  template <typename T> requires (std::is_trivially_copyable_v<T>)
    static constexpr std::array<uint8, sizeof(T)> array_cast(const T value) {
    return std::bit_cast<std::array<uint8, sizeof(T)>>(value);
  }

  template <typename T> requires(std::is_integral_v<T> &&std::is_trivially_copyable_v<T>)
    static void push_back(byte_vector &__restrict vec, const T value) {
    if constexpr (sizeof(T) == 1) {
      vec.push_back(value);
    }
    else {
      auto bytes = array_cast<T>(value);

      const std::size_t start = vec.size();
      vec.resize(start + sizeof(T));

      std::copy(bytes.cbegin(), bytes.cend(), &vec[start]);
    }
  }

  template <typename T> requires(std::is_integral_v<T> &&std::is_trivially_copyable_v<T>)
    static void insert_back(byte_vector &__restrict vec, const T value) {
    if constexpr (sizeof(T) == 1) {
      vec.back() = value;
    }
    else {
      auto bytes = array_cast<T>(value);

      const std::size_t start = vec.size() - 1;
      vec.resize(start + sizeof(T) - 1);

      std::copy(bytes.cbegin(), bytes.cend(), &vec[start]);
    }
  }

  template <typename TTo, typename TFrom> requires (std::is_integral_v<TTo> && std::is_integral_v<TFrom>)
  static constexpr TTo check_cast(TFrom value) {
    check(value >= std::numeric_limits<TTo>::min() && value <= std::numeric_limits<TTo>::max());
    return TTo(value);
  }

  static byte_vector recode(const std::span<const char> code, const std::unordered_map<uint32, uint32> &matching_brackets) {
    __assume(code.size_bytes() <= std::numeric_limits<uint32>::max());

#if !RECODE
    return byte_vector(code, code + code_len);
#else
    byte_vector recoded;
    recoded.reserve(code.size_bytes());

    // We recode the Brainfuck so that it's faster to execute.
    // We will handle a few optimizations here. Mainly detecting sequences.

    static constexpr const uint seq_min = 3;

    {
      std::vector<uint32> loops;
      uint32 instruction_pointer = 0;
      fmt::println("Original Program Size: {}", check_cast<uint32>(code.size_bytes()));
      while (instruction_pointer < code.size_bytes()) {
        const auto i = token(code[instruction_pointer]);
        recoded.push_back(byte_cast(i));
        switch (i) {
          default: [[unlikely]]
            // Ignore all other symbols
            recoded.pop_back();
            break;
          case token::output_byte:
            break;
          case token::input_byte: [[unlikely]]
            break;
          case token::increment_pointer_one:
          {
            // See if there's a sequence of them after it.
            uint32 seq_cnt = 1;
            for (uint32 ii = instruction_pointer + 1; ii < code.size_bytes() && code[ii] == token::increment_pointer_one; ++ii) {
              ++seq_cnt;
            }
            if (seq_cnt >= seq_min) {
              recoded.pop_back();

              if (seq_cnt <= std::numeric_limits<uint8>::max()) {
                recoded.push_back(byte_cast(token::increment_pointer_seq_1));

                recoded.push_back(uint8(seq_cnt));
              }
              else if (seq_cnt <= std::numeric_limits<uint16>::max()) {
                recoded.push_back(byte_cast(token::increment_pointer_seq_2));

                push_back(recoded, uint16(seq_cnt));
              }
              else {
                recoded.push_back(byte_cast(token::increment_pointer_seq_4));

                push_back(recoded, uint32(seq_cnt));
              }
              instruction_pointer += seq_cnt - 1;
            }
          }  break;
          case token::decrement_pointer_one:
          {
            // See if there's a sequence of them after it.
            uint32 seq_cnt = 1;
            for (uint32 ii = instruction_pointer + 1; ii < code.size_bytes() && code[ii] == token::decrement_pointer_one; ++ii) {
              ++seq_cnt;
            }
            if (seq_cnt >= seq_min) {
              recoded.pop_back();

              if (seq_cnt <= std::numeric_limits<uint8>::max()) {
                recoded.push_back(byte_cast(token::decrement_pointer_seq_1));

                recoded.push_back(uint8(seq_cnt));
              }
              else if (seq_cnt <= std::numeric_limits<uint16>::max()) {
                recoded.push_back(byte_cast(token::decrement_pointer_seq_2));

                push_back(recoded, uint16(seq_cnt));
              }
              else {
                recoded.push_back(byte_cast(token::decrement_pointer_seq_4));

                push_back(recoded, uint32(seq_cnt));
              }
              instruction_pointer += seq_cnt - 1;
            }
          }  break;
          case token::increment_byte_one:
          {
            // See if there's a sequence of them after it.
            uint32 seq_cnt = 1;
            for (uint32 ii = instruction_pointer + 1; ii < code.size_bytes() && code[ii] == token::increment_byte_one; ++ii) {
              ++seq_cnt;
            }
            if (seq_cnt >= seq_min) {
              recoded.pop_back();

              if (seq_cnt <= std::numeric_limits<uint8>::max()) {
                recoded.push_back(byte_cast(token::increment_byte_seq_1));

                recoded.push_back(uint8(seq_cnt));
              }
              else if (seq_cnt <= std::numeric_limits<uint16>::max()) {
                recoded.push_back(byte_cast(token::increment_byte_seq_2));

                push_back(recoded, uint16(seq_cnt));
              }
              else {
                recoded.push_back(byte_cast(token::increment_byte_seq_4));

                push_back(recoded, uint32(seq_cnt));
              }
              instruction_pointer += seq_cnt - 1;
            }
          }  break;
          case token::decrement_byte_one:
          {
            // See if there's a sequence of them after it.
            uint32 seq_cnt = 1;
            for (uint32 ii = instruction_pointer + 1; ii < code.size_bytes() && code[ii] == token::decrement_byte_one; ++ii) {
              ++seq_cnt;
            }
            if (seq_cnt >= seq_min) {
              recoded.pop_back();

              if (seq_cnt <= std::numeric_limits<uint8>::max()) {
                recoded.push_back(byte_cast(token::decrement_byte_seq_1));

                recoded.push_back(uint8(seq_cnt));
              }
              else if (seq_cnt <= std::numeric_limits<uint16>::max()) {
                recoded.push_back(byte_cast(token::decrement_byte_seq_2));

                push_back(recoded, uint16(seq_cnt));
              }
              else {
                recoded.push_back(byte_cast(token::decrement_byte_seq_4));

                push_back(recoded, uint32(seq_cnt));
              }
              instruction_pointer += seq_cnt - 1;
            }
          }  break;
          case token::jump_forth:
          {
            // check for special case '[-]'
            if (code[instruction_pointer + 1] == token::decrement_byte_one && code[instruction_pointer + 2] == token::jump_back) {
              recoded[recoded.size() - 1] = byte_cast(token::zero_byte);
              instruction_pointer += 2;
              break;
            }
            // w = [<n] while (*ptr) ptr -= n
            {
              uint32 ii = std::min(uint32(code.size_bytes()), instruction_pointer + 1);
              uint32 seq_len = 0;
              while (code[ii] == token::decrement_pointer_one) {
                ii = std::min(uint32(code.size_bytes()), ii + 1);
                ++seq_len;
              }
              if (seq_len > 0 && seq_len < std::numeric_limits<uint8>::max() && code[ii] == token::jump_back) {
                recoded[recoded.size() - 1] = byte_cast(token::special_case_6);
                recoded.push_back(uint8(seq_len));
                instruction_pointer = ii;
                break;
              }
            }
            // x = [>n] while (*ptr) ptr += n
            {
              uint32 ii = std::min(uint32(code.size_bytes()), instruction_pointer + 1);
              uint32 seq_len = 0;
              while (code[ii] == token::increment_pointer_one) {
                ii = std::min(uint32(code.size_bytes()), ii + 1);
                ++seq_len;
              }
              if (seq_len > 0 && seq_len < std::numeric_limits<uint8>::max() && code[ii] == token::jump_back) {
                recoded[recoded.size() - 1] = byte_cast(token::special_case_7);
                recoded.push_back(uint8(seq_len));
                instruction_pointer = ii;
                break;
              }
            }
            // check for special case [->+<]
            if (
              code[instruction_pointer + 1] == token::decrement_byte_one &&
              code[instruction_pointer + 2] == token::increment_pointer_one &&
              code[instruction_pointer + 3] == token::increment_byte_one &&
              code[instruction_pointer + 4] == token::decrement_pointer_one &&
              code[instruction_pointer + 5] == token::jump_back) {
              recoded[recoded.size() - 1] = byte_cast(token::special_case_1);
              instruction_pointer += 5;
              break;
            }
            // check for : // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
            {
              if (uint32 ii = std::min(uint32(code.size_bytes()), instruction_pointer + 1); code[ii] == token::decrement_byte_one) {
                ii = std::min(uint32(code.size_bytes()), ii + 1);
                uint32 seq_len = 0;
                while (code[ii] == token::increment_pointer_one) {
                  ii = std::min(uint32(code.size_bytes()), ii + 1);
                  ++seq_len;
                }
                if (seq_len && seq_len <= std::numeric_limits<uint8>::max() && code[ii] == token::increment_byte_one) // TODO if we want 2- and 4-b sequences, change this.
                {
                  ii = std::min(uint32(code.size_bytes()), ii + 1);
                  uint32 seq_len2 = 0;
                  while (code[ii] == token::decrement_pointer_one) {
                    ii = std::min(uint32(code.size_bytes()), ii + 1);
                    ++seq_len2;
                  }
                  if (seq_len2 == seq_len && code[ii] == token::jump_back) {
                    recoded[recoded.size() - 1] = byte_cast(token::special_case_2);
                    recoded.push_back(uint8(seq_len));
                    instruction_pointer = ii;
                    break;
                  }
                }
              }
            }
            // check for : // t = special case [->n+<n] (*(ptr - n) += *ptr; *ptr = 0
            {
              if (uint32 ii = std::min(uint32(code.size_bytes()), instruction_pointer + 1); code[ii] == token::decrement_byte_one) {
                ii = std::min(uint32(code.size_bytes()), ii + 1);
                uint32 seq_len = 0;
                while (code[ii] == token::decrement_pointer_one) {
                  ii = std::min(uint32(code.size_bytes()), ii + 1);
                  ++seq_len;
                }
                if (seq_len && seq_len <= std::numeric_limits<uint8>::max() && code[ii] == token::increment_byte_one) // TODO if we want 2- and 4-b sequences, change this.
                {
                  ii = std::min(uint32(code.size_bytes()), ii + 1);
                  uint32 seq_len2 = 0;
                  while (code[ii] == token::increment_pointer_one) {
                    ii = std::min(uint32(code.size_bytes()), ii + 1);
                    ++seq_len2;
                  }
                  if (seq_len2 == seq_len && code[ii] == token::jump_back) {
                    recoded[recoded.size() - 1] = byte_cast(token::special_case_3);
                    recoded.push_back(uint8(seq_len));
                    instruction_pointer = ii;
                    break;
                  }
                }
              }
            }
            // check for : // s = special case [->n+<n] (*(ptr + n) -= *ptr; *ptr = 0
            {
              if (uint32 ii = std::min(uint32(code.size_bytes()), instruction_pointer + 1); code[ii] == token::decrement_byte_one) {
                ii = std::min(uint32(code.size_bytes()), ii + 1);
                uint32 seq_len = 0;
                while (code[ii] == token::increment_pointer_one) {
                  ii = std::min(uint32(code.size_bytes()), ii + 1);
                  ++seq_len;
                }
                if (seq_len && seq_len <= std::numeric_limits<uint8>::max() && code[ii] == token::decrement_byte_one) // TODO if we want 2- and 4-b sequences, change this.
                {
                  ii = std::min(uint32(code.size_bytes()), ii + 1);
                  uint32 seq_len2 = 0;
                  while (code[ii] == token::decrement_pointer_one) {
                    ii = std::min(uint32(code.size_bytes()), ii + 1);
                    ++seq_len2;
                  }
                  if (seq_len2 == seq_len && code[ii] == token::jump_back) {
                    recoded[recoded.size() - 1] = byte_cast(token::special_case_4);
                    recoded.push_back(uint8(seq_len));
                    instruction_pointer = ii;
                    break;
                  }
                }
              }
            }
            // check for : // t = special case [->n+<n] (*(ptr - n) -= *ptr; *ptr = 0
            {
              if (uint32 ii = std::min(uint32(code.size_bytes()), instruction_pointer + 1); code[ii] == token::decrement_byte_one) {
                ii = std::min(uint32(code.size_bytes()), ii + 1);
                uint32 seq_len = 0;
                while (code[ii] == token::decrement_pointer_one) {
                  ii = std::min(uint32(code.size_bytes()), ii + 1);
                  ++seq_len;
                }
                if (seq_len && seq_len <= std::numeric_limits<uint8>::max() && code[ii] == token::decrement_byte_one) // TODO if we want 2- and 4-b sequences, change this.
                {
                  ii = std::min(uint32(code.size_bytes()), ii + 1);
                  uint32 seq_len2 = 0;
                  while (code[ii] == token::increment_pointer_one) {
                    ii = std::min(uint32(code.size_bytes()), ii + 1);
                    ++seq_len2;
                  }
                  if (seq_len2 == seq_len && code[ii] == token::jump_back) {
                    recoded[recoded.size() - 1] = byte_cast(token::special_case_5);
                    recoded.push_back(uint8(seq_len));
                    instruction_pointer = ii;
                    break;
                  }
                }
              }
            }


            // r = special case [->+<] (*(ptr - 1) += *ptr; *ptr = 0

            loops.push_back(uint32(recoded.size() - 1));
						
            const uint32 target = code.size_bytes();
            const uint32 match = 0;

            if (target <= std::numeric_limits<uint8>::max()) {
              recoded[recoded.size() - 1] = byte_cast(token::jump_forth_1);
              push_back(recoded, uint8(match));
            }
            else if (target <= std::numeric_limits<uint16>::max()) {
              recoded[recoded.size() - 1] = byte_cast(token::jump_forth_2);
              push_back(recoded, uint16(match));
            }
            else {
              push_back(recoded, uint32(match));
            }
          }  break;
          case token::jump_back:
          {
            uint32 last_loop_offset = loops.back();
            loops.pop_back();

            const uint32 last_loop_end_offset = [&] () -> uint32 {
              uint32 operand_size;

              const uint32 lambda_offset = uint32(code.size_bytes());

              if (lambda_offset <= std::numeric_limits<uint8>::max()) [[likely]] {
                operand_size = sizeof(uint8);
              }
              else if (lambda_offset <= std::numeric_limits<uint16>::max()) {
                operand_size = sizeof(uint16);
              }
              else if (lambda_offset <= std::numeric_limits<uint32>::max()) [[unlikely]] {
                operand_size = sizeof(uint32);
              }
              else {
                UNREACHABLE();
                operand_size = sizeof(uint64);
              }

              return (recoded.size() - 1) - (last_loop_offset + 1 + operand_size);
            }();

            uint32 return_loc_val;

            if (last_loop_end_offset <= std::numeric_limits<uint8>::max()) {
              return_loc_val = check_cast<uint32>(recoded.size()) + 1;
              recoded[recoded.size() - 1] = byte_cast(token::jump_back_1);
              push_back(recoded, uint8(last_loop_end_offset));
            }
            else if (last_loop_end_offset <= std::numeric_limits<uint16>::max()) {
              return_loc_val = check_cast<uint32>(recoded.size()) + 2;
              recoded[recoded.size() - 1] = byte_cast(token::jump_back_2);
              push_back(recoded, uint16(last_loop_end_offset));
            }
            else if (last_loop_end_offset <= std::numeric_limits<uint32>::max()) {
              return_loc_val = check_cast<uint32>(recoded.size()) + 4;
              push_back(recoded, uint32(last_loop_end_offset));
            }
            else {
              UNREACHABLE();
              return_loc_val = check_cast<uint32>(recoded.size()) + 8;
              push_back(recoded, uint64(last_loop_end_offset));
            }

#if !PRE_SCAN
            const auto get_target_size = [] (const uint32 value) -> uint32 {
              if (value <= std::numeric_limits<uint8>::max()) {
                return sizeof(uint8);
              }
              else if (value <= std::numeric_limits<uint16>::max()) [[likely]] {
                return sizeof(uint16);
              }
              else if (value <= std::numeric_limits<uint32>::max()) [[unlikely]] {
                return sizeof(uint32);
              }
              else {
                UNREACHABLE();
                return sizeof(uint64);
              }
            };

            static constexpr const bool recode_instruction = false;

            if constexpr (recode_instruction) {
              const auto reencode_start = [&] (const uint32 target) {
                // const uint32 current_operand_size = get_target_size(code_len);
                const uint32 operand_size = get_target_size(return_loc_val);

                // Re-encode the return location.
                if (const auto encoded_token = get_token_info(recoded[target]); encoded_token.operand_size != operand_size) {
                  if (const token new_token = resize_jump_operand(encoded_token.token, operand_size); new_token != encoded_token.token) {
                    const uint32 to_remove = encoded_token.operand_size - operand_size;

                    const auto erase_iterator = recoded.begin() + target + 1 + (encoded_token.operand_size - to_remove);
                    recoded[target] = byte_cast(new_token);
                    recoded.erase(erase_iterator, erase_iterator + to_remove);
                  }
                }
              };

              reencode_start(last_loop_offset);
            }

            // write this offset back in.
            const auto return_loc = &recoded[last_loop_offset + 1];

            const uint32 compare_value = recode_instruction ? return_loc_val : uint32(code.size_bytes());

            if (compare_value <= std::numeric_limits<uint8>::max()) {
              *reinterpret_cast<uint8 *__restrict>(return_loc) = uint8(return_loc_val);
            }
            else if (compare_value <= std::numeric_limits<uint16>::max()) [[likely]] {
              *reinterpret_cast<uint16 *__restrict>(return_loc) = uint16(return_loc_val);
            }
            else if (compare_value <= std::numeric_limits<uint32>::max()) [[unlikely]] {
              *reinterpret_cast<uint32 *__restrict>(return_loc) = return_loc_val;
            }
            else {
              UNREACHABLE();
              *reinterpret_cast<uint64 *__restrict>(return_loc) = return_loc_val;
            }
#endif
          }  break;
        }
        ++instruction_pointer;
      }
    }

    return recoded;
#endif
  }

  template <uint32 Count = 32'768>
  struct cell_t final : std::array<uint8, Count> {
  private:
    static constexpr const bool count_pow2 = (Count & (Count - 1u)) == 0u;

  public:
    uint32 index = 0;

    __forceinline uint8 &operator++() {
      if constexpr (count_pow2) {
        ++index;
        index &= Count - 1;
      }
      else {
        if (index >= Count - 1) [[unlikely]] {
          index = 0;
        }
      }

      return (*this)[index];
    }

    __forceinline uint8 &operator++(int) {
      const uint32 original_index = index;
      if constexpr (count_pow2) {
        ++index;
        index &= Count - 1;
      }
      else {
        if (index >= Count - 1) [[unlikely]] {
          index = 0;
        }
      }

      return (*this)[original_index];
    }

    __forceinline uint8 &operator--() {
      if constexpr (count_pow2) {
        --index;
        index &= Count - 1;
      }
      else {
        if (1 > index) [[unlikely]] {
          index = Count - 1;
        }
      }

      return (*this)[index];
    }

    __forceinline uint8 &operator--(int) {
      const uint32 original_index = index;
      if constexpr (count_pow2) {
        --index;
        index &= Count - 1;
      }
      else {
        if (1 > index) [[unlikely]] {
          index = Count - 1;
        }
      }

      return (*this)[original_index];
    }

    __forceinline uint8 &operator+(const uint32 value) {
      uint32 current_index = index;
      current_index += value;
      if constexpr (count_pow2) {
        current_index &= Count - 1;
      }
      else {
        if (current_index >= Count) [[unlikely]] {
          current_index %= Count;
        }
      }

      return (*this)[current_index];
    }

    __forceinline uint8 &operator-(const uint32 value) {
      uint32 current_index = index;
      if constexpr (count_pow2) {
        current_index -= value;
        current_index &= Count - 1;
      }
      else {
        if (value > current_index) [[unlikely]] {
          current_index = Count - (value - current_index);
        }
      }

      return (*this)[current_index];
    }

    __forceinline uint8 &operator+=(const uint32 value) {
      uint32 current_index = index;
      current_index += value;
      if constexpr (count_pow2) {
        current_index &= Count - 1;
      }
      else {
        if (current_index >= Count) [[unlikely]] {
          current_index %= Count;
        }
      }

      index = current_index;
      return (*this)[current_index];
    }

    __forceinline uint8 &operator-=(const uint32 value) {
      uint32 current_index = index;
      if constexpr (count_pow2) {
        current_index -= value;
        current_index &= Count - 1;
      }
      else {
        if (value > current_index) [[unlikely]] {
          current_index = Count - (value - current_index);
        }
      }

      index = current_index;
      return (*this)[current_index];
    }

    __forceinline const uint8 operator*() const {
      return (*this)[index];
    }

    __forceinline uint8 &operator*() {
      return (*this)[index];
    }
  };

  template <size_t N>
  static void dump_count(const char(&__restrict value)[N]) {
#if 0
    if constexpr (N == 1) {
      std::putchar(value[0]);
    }
    else {
      std::fputs(value, stdout);
    }
#endif
  }

  static void dump_count(const char value) {
#if 0
    std::putchar(value);
#endif
  }

  static void dump_instructions(std::span<const uint8_t> data) {
#if PRINT_RECODE
    std::puts("Recoded Bytecode:\n");

    for (uint32 instruction_pointer = 0; instruction_pointer < data.size_bytes(); ++instruction_pointer) {
      const auto i = token(data[instruction_pointer]);
      std::putchar(enum_cast(i));

      switch (i) {
#if RECODE
        case token::zero_byte: // [-] special case
        case token::special_case_1: // r = special case [->+<] (*(ptr - 1) += *ptr; *ptr = 0
          break;
        case token::special_case_6:
        case token::special_case_7:
        case token::special_case_2: // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
        case token::special_case_3: // t = special case [->n+<n] (*(ptr - n) += *ptr; *ptr = 0
        case token::special_case_4: // u = special case [->n+<n] (*(ptr + n) -= *ptr; *ptr = 0
        case token::special_case_5: // v = special case [->n+<n] (*(ptr - n) -= *ptr; *ptr = 0
        {
          dump_count('#');
          ++instruction_pointer;
        } break;
        case token::increment_pointer_seq_4: // > sequence special case
        case token::decrement_pointer_seq_4: // < sequence special case
        case token::increment_byte_seq_4: // + sequence special case
        case token::decrement_byte_seq_4: // - sequence special case
        {
          dump_count("####");
          instruction_pointer += 4;
        } break;
        case token::increment_pointer_seq_2: // > sequence special case
        case token::decrement_pointer_seq_2: // < sequence special case
        case token::increment_byte_seq_2: // + sequence special case
        case token::decrement_byte_seq_2: // - sequence special case
        {
          dump_count("##");
          instruction_pointer += 2;
        } break;
        case token::increment_pointer_seq_1: // > sequence special case
        case token::decrement_pointer_seq_1: // < sequence special case
        case token::increment_byte_seq_1: // + sequence special case
        case token::decrement_byte_seq_1: // - sequence special case
        {
          dump_count('#');
          ++instruction_pointer;
        } break;
        case token::increment_pointer_one:
        case token::decrement_pointer_one:
        case token::increment_byte_one:
        case token::decrement_byte_one:
        case token::output_byte:
        case token::input_byte:
          // don't handle yet.
          break;
        case token::jump_forth:
        case token::jump_back:
          dump_count("####");
          instruction_pointer += 4;
          break;
        case token::jump_back_2:
        case token::jump_forth_2:
          dump_count("##");
          instruction_pointer += 2;
          break;
        case token::jump_back_1:
        case token::jump_forth_1:
          dump_count("#");
          ++instruction_pointer;
          break;
#endif
#if !RECODE
        case token('\0'):  // NOLINT(clang-diagnostic-switch)
          break;
#endif
        default:  // NOLINT(clang-diagnostic-covered-switch-default)
#if RECODE
          UNREACHABLE();
#else
          break;
#endif
      }
    }
#endif
  }

  static std::unordered_map<uint32, uint32> pre_scan(const std::span<const char> code) {
#if PRE_SCAN
    std::unordered_map<uint32, uint32> matching_brackets;

    std::vector<uint32> bracket_stack;

    for (uint32 i = 0; i < code.size(); ++i) {
      switch (code[i]) {
        case uint8(uint8(token::jump_forth)):
          bracket_stack.push_back(i);
          break;
        case uint8(uint8(token::jump_back)):
        {
          check(!bracket_stack.empty());
          const uint32 matching_offset = bracket_stack.back();
          bracket_stack.pop_back();
          matching_brackets.insert({ i, matching_offset });
          matching_brackets.insert({ matching_offset, i });
        } break;
        default:
          break;
      }
    }

    check(bracket_stack.empty());

    return matching_brackets;
#else
    return {};
#endif
  }

  struct stdout_buffer_toggle final {
  private:
    static inline size_t toggle_count_ = 0;
    bool success_;

    static NO_THROW bool toggle() noexcept {
      if (toggle_count_++) [[unlikely]] {
        return false;
      }
      return 0 == std::setvbuf(stdout, nullptr, _IONBF, BUFSIZ);
    }

  public:
    NO_THROW stdout_buffer_toggle() noexcept : success_(toggle()) {}

    stdout_buffer_toggle(const stdout_buffer_toggle&) = delete;
    stdout_buffer_toggle(stdout_buffer_toggle&& value) = delete;
    void operator=(const stdout_buffer_toggle&) = delete;
    void operator=(stdout_buffer_toggle&&) = delete;

    NO_THROW ~stdout_buffer_toggle() noexcept {
      if (--toggle_count_ == 0 && success_) [[likely]] {
        check(0 == std::setvbuf(stdout, nullptr, _IOFBF, BUFSIZ));
      }
    }
  };

  static cell_t cells = {};

  static void execute(const std::span<const uint8_t> code) {
    stdout_buffer_toggle disable_buffering_scope;

    uint32 instruction_pointer = 0;

    // Now execute it recoded.

    check(code.size() <= std::numeric_limits<uint32>::max());

    fmt::println("Recoded Program Size: {}", uint32(code.size()));
    while (instruction_pointer < code.size()) {
      switch (static_cast<const token>(code[instruction_pointer])) {
#if RECODE
        case token::zero_byte: // [-] special case
          *cells = 0;
          break;
        case token::special_case_1: // r = special case [->+<] (*(ptr - 1) += *ptr; *ptr = 0
          (cells + 1) += *cells;
          *cells = 0;
          break;
        case token::special_case_6:
        {
          const uint8 count = code[++instruction_pointer];
          while (*cells) {
            cells -= count;
          }
        } break;
        case token::special_case_7:
        {
          const uint8 count = code[++instruction_pointer];
          while (*cells) {
            cells += count;
          }
        } break;
        case token::special_case_2: // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
        {
          const uint8 count = code[++instruction_pointer];
          (cells + count) += *cells;
          *cells = 0;
        } break;
        case token::special_case_3: // t = special case [->n+<n] (*(ptr - n) += *ptr; *ptr = 0
        {
          const uint8 count = code[++instruction_pointer];
          (cells - count) += *cells;
          *cells = 0;
        } break;
        case token::special_case_4: // u = special case [->n+<n] (*(ptr + n) -= *ptr; *ptr = 0
        {
          const uint8 count = code[++instruction_pointer];
          (cells + count) -= *cells;
          *cells = 0;
        } break;
        case token::special_case_5: // v = special case [->n+<n] (*(ptr - n) -= *ptr; *ptr = 0
        {
          const uint8 count = code[++instruction_pointer];
          (cells - count) -= *cells;
          *cells = 0;
        } break;
        case token::increment_pointer_seq_4: // > sequence special case
        {
          const uint32 value = reinterpret_cast<const uint32 & __restrict>(code[instruction_pointer + 1]);
          instruction_pointer += 4;
          cells += value;
        } break;
        case token::decrement_pointer_seq_4: // < sequence special case
        {
          const uint32 value = reinterpret_cast<const uint32 & __restrict>(code[instruction_pointer + 1]);
          instruction_pointer += 4;
          cells -= value;
        } break;
        case token::increment_byte_seq_4: // + sequence special case
        {
          const uint32 value = reinterpret_cast<const uint32 & __restrict>(code[instruction_pointer + 1]);
          instruction_pointer += 4;
          *cells += value;
        } break;
        case token::decrement_byte_seq_4: // - sequence special case
        {
          const uint32 value = reinterpret_cast<const uint32 & __restrict>(code[instruction_pointer + 1]);
          instruction_pointer += 4;
          *cells -= value;
        } break;
        case token::increment_pointer_seq_2: // > sequence special case
        {
          const uint16 value = reinterpret_cast<const uint16 & __restrict>(code[instruction_pointer + 1]);
          instruction_pointer += 2;
          cells += value;
        } break;
        case token::decrement_pointer_seq_2: // < sequence special case
        {
          const uint16 value = reinterpret_cast<const uint16 & __restrict>(code[instruction_pointer + 1]);
          instruction_pointer += 2;
          cells -= value;
        } break;
        case token::increment_byte_seq_2: // + sequence special case
        {
          const uint16 value = reinterpret_cast<const uint16 & __restrict>(code[instruction_pointer + 1]);
          instruction_pointer += 2;
          *cells += value;
        } break;
        case token::decrement_byte_seq_2: // - sequence special case
        {
          const uint16 value = reinterpret_cast<const uint16 & __restrict>(code[instruction_pointer + 1]);
          instruction_pointer += 2;
          *cells -= value;
        } break;
        case token::increment_pointer_seq_1: // > sequence special case
        {
          cells += code[++instruction_pointer];
        } break;
        case token::decrement_pointer_seq_1: // < sequence special case
        {
          cells -= code[++instruction_pointer];
        } break;
        case token::increment_byte_seq_1: // + sequence special case
        {
          *cells += code[++instruction_pointer];
        } break;
        case token::decrement_byte_seq_1: // - sequence special case
        {
          *cells -= code[++instruction_pointer];
        } break;
#endif
        case token::increment_pointer_one:
          ++cells; break;
        case token::decrement_pointer_one:
          --cells; break;
        case token::increment_byte_one:
          ++*cells; break;
        case token::decrement_byte_one:
          --*cells; break;
        case token::output_byte:
          std::putchar(*cells);
					std::fflush(stdout);
          break;
        case token::input_byte: [[unlikely]]
          // don't handle yet.
          break;
        case token::jump_forth:
          if (*cells == 0) {
#if RECODE
            // Jump.
            instruction_pointer = reinterpret_cast<const uint32 & __restrict>(code[instruction_pointer + 1]);
            continue;
#else
# if PRE_SCAN
            instruction_pointer = matching_brackets[instruction_pointer];
# else
            uint32 extra_count = 0;
            uint8 t;
            while (instruction_pointer < code.size() && (((t = code[++instruction_pointer]) != uint8(token::jump_back)) | (extra_count != 0))) {
              if (t == uint8(token::jump_forth)) {
                ++extra_count;
              }
              else if (t == uint8(token::jump_back)) {
                --extra_count;
              }
            }

            if (extra_count != 0) [[unlikely]] {
              std::fputs("Unmatched '[' token", stderr);  // NOLINT(cert-err33-c)
              std::exit(2);  // NOLINT(concurrency-mt-unsafe)
            }
# endif
#endif
          }
#if RECODE
          else {
            instruction_pointer += 4;
          }
#endif
          break;
        case token::jump_back:
          if (*cells != 0) {
#if RECODE
            // Jump.
            instruction_pointer -= reinterpret_cast<const uint32 & __restrict>(code[instruction_pointer + 1]);
            continue;
#else
# if PRE_SCAN
            instruction_pointer = matching_brackets[instruction_pointer];
# else
            uint32 extra_count = 0;
            uint8 t;
            while (instruction_pointer > 0 && (((t = code[--instruction_pointer]) != uint8(token::jump_forth)) | (extra_count != 0))) {
              if (t == uint8(token::jump_back)) {
                ++extra_count;
              }
              else if (t == uint8(token::jump_forth)) {
                --extra_count;
              }
            }

            if (extra_count != 0) [[unlikely]] {
              std::fputs("Unmatched ']' token", stderr);  // NOLINT(cert-err33-c)
              std::exit(2);  // NOLINT(concurrency-mt-unsafe)
            }
# endif
#endif
          }
#if RECODE
          else {
            instruction_pointer += 4;
          }
#endif
          break;
#if RECODE
        case token::jump_back_2:
          if (*cells != 0) {
            // Jump.
            instruction_pointer -= reinterpret_cast<const uint16 & __restrict>(code[instruction_pointer + 1]);
            continue;
          }
          else {
            instruction_pointer += 2;
          } break;
        case token::jump_back_1:
          if (*cells != 0) {
            // Jump.
            instruction_pointer -= code[instruction_pointer + 1];
            continue;
          }
          else {
            ++instruction_pointer;
          } break;
        case token::jump_forth_2:
          if (*cells == 0) {
            // Jump.
            instruction_pointer = reinterpret_cast<const uint16 & __restrict>(code[instruction_pointer + 1]);
            continue;
          }
          else {
            instruction_pointer += 2;
          } break;
        case token::jump_forth_1: [[unlikely]]
          if (*cells == 0) {
            // Jump.
            instruction_pointer = code[instruction_pointer + 1];
            continue;
          }
          else {
            ++instruction_pointer;
          } break;
#endif
#if !RECODE
        case token('\0'): [[unlikely]]  // NOLINT(clang-diagnostic-switch)
          break;
#endif
        default: [[unlikely]]  // NOLINT(clang-diagnostic-covered-switch-default)
          // recoding strips unexpected tokens
#if RECODE
          UNREACHABLE();
#else
          break;
#endif
      }
      ++instruction_pointer;
    }
  }
}

static constexpr int pow10i(int exponent)
{
	if (exponent < 0)
	{
		return 0;
	}
	
	int result = 1;
	for(; exponent > 0; --exponent)
	{
		result *= 10;
	}
	
	return result;
}

static constexpr std::array<int, 3> split_version3(const int version, const unsigned major_shift, const unsigned minor_shift)
{
	const unsigned major_version_div = pow10i(major_shift);
	const unsigned minor_version_div = pow10i(minor_shift);
	const int major_version = (version / major_version_div);
	const int minor_version = (version / minor_version_div) - (major_version * pow10i(major_shift - minor_shift));
	const int build_version = (version) - (major_version * major_version_div) - (minor_version * minor_version_div);
	
	return { major_version, minor_version, build_version };
}

static void print_toolchain()
{	
	#if defined(_MSC_VER) && !defined(__clang__)
	{
		const auto msvc_version = split_version3(_MSC_FULL_VER, 7, 5);
		std::puts("Toolchain: MSVC");
		fmt::println(
			"   Compiler: CL {:02}.{:02}.{:05}.{}",
			msvc_version[0],
			msvc_version[1],
			msvc_version[2],
			(_MSC_BUILD)
		);
	}
	#elif defined(__clang__)
	std::puts("Toolchain: LLVM");
	fmt::println("   Compiler: Clang {}", __clang_version__);
	#elif defined(__GNUC__)
	std::puts("Toolchain: GNU");
	fmt::println("   Compiler: GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
	#else
	std::puts("Toolchain: unknown");
	#endif
	
	#if defined(__MUSL__)
		#if defined(__MUSL_VER_MAJOR__)
	fmt::println("       libc: musl {}.{}.{}", __MUSL_VER_MAJOR__, __MUSL_VER_MINOR__, __MUSL_VER_PATCH__);
		#else
	fmt::println("       libc: musl");
		#endif
	#elif defined(__GLIBC__)
	fmt::println("       libc: glibc {}.{}", __GLIBC__, __GLIBC_MINOR__);
	#elif defined(_MSC_VER)
	// nothing
	#else
	std::puts("       libc: unknown");
	#endif
	
	#if defined(_LIBCPP_VERSION)
	{
		const auto libcpp_version = split_version3(_LIBCPP_VERSION, 4, 2);
		fmt::println(
			"     libcxx: libcxx {}.{}.{}",
			libcpp_version[0],
			libcpp_version[1],
			libcpp_version[2]
		);
	}
	#elif defined(__GLIBCXX__)
	{
		const auto libstdcpp_version = split_version3(__GLIBCXX__, 4, 2);
		fmt::println(
			"     libc++: libstdc++ {} {}.{}.{}",
			_GLIBCXX_RELEASE,
			libstdcpp_version[0],
			libstdcpp_version[1],
			libstdcpp_version[2]
		);
	}
	#elif defined(_MSC_VER)
	// nothing
	#else
	std::puts("     libc++: unknown");
	#endif
}

int main() {
  static constexpr const auto code = std::to_array(bench::program);
  check(code.size() <= std::numeric_limits<uint32>::max());

  // Pre-scan the code for matching brackets to get offsets.
  // Also will allow the recoder to use smaller offsets.
  const auto matching_brackets = pre_scan(code);

	std::puts("Brainfuck Test Interpreter");
	
	print_toolchain();

  fmt::println("Generating {} via Brainfuck", bench::name);

  {
    const byte_vector recoded = recode(code, matching_brackets);

    execute(recoded);

    dump_instructions(recoded);
  }
  std::fflush(stdout);  // NOLINT(cert-err33-c)
}
