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
#include <bit>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include BENCHMARK

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
  };

  static constexpr inline uint32 get_operand_size(const token tok) {
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
        return 0;
    }
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
    switch (tok) {
      case token::jump_back_2: [[likely]] {
        if (check(new_size == 1)) [[likely]] {
          return token::jump_back_1;
        }
        return tok;
      }
      case token::jump_forth_2: [[likely]] {
        if (check(new_size == 1)) [[likely]] {
          return token::jump_forth_1;
        }
        return tok;
      }
      case token::jump_back_1: {
        
        if (check(new_size == 2)) [[likely]] {
          return token::jump_back_2;
        }
        return tok;
      }
      case token::jump_forth_1: {
        if (check(new_size == 2)) [[likely]] {
          return token::jump_forth_2;
        }
        return tok;
      }
      default: [[unlikely]]
        assert(false);
        return tok;
    }
  }

  static constexpr inline token resize_operand(const token tok, const uint32 new_size) {
    switch (tok) {
      case token::increment_pointer_seq_4:
        switch (new_size) {
          case 2:
            return token::increment_pointer_seq_2;
          case 1:
            return token::increment_pointer_seq_1;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::decrement_pointer_seq_4:
        switch (new_size) {
          case 2:
            return token::decrement_pointer_seq_2;
          case 1:
            return token::decrement_pointer_seq_1;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::increment_byte_seq_4:
        switch (new_size) {
          case 2:
            return token::increment_byte_seq_2;
          case 1:
            return token::increment_byte_seq_1;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::decrement_byte_seq_4:
        switch (new_size) {
          case 2:
            return token::decrement_byte_seq_2;
          case 1:
            return token::decrement_byte_seq_1;
          default: [[unlikely]]
            assert(false);
            return tok;
        }

      case token::increment_pointer_seq_2:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::increment_pointer_seq_4;
          case 1:
            return token::increment_pointer_seq_1;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::decrement_pointer_seq_2:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::decrement_pointer_seq_4;
          case 1:
            return token::decrement_pointer_seq_1;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::increment_byte_seq_2:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::increment_byte_seq_4;
          case 1:
            return token::increment_byte_seq_1;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::decrement_byte_seq_2:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::decrement_byte_seq_4;
          case 1:
            return token::decrement_byte_seq_1;
          default: [[unlikely]]
            assert(false);
            return tok;
        }

      case token::increment_pointer_seq_1:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::increment_pointer_seq_4;
          case 2:
            return token::increment_pointer_seq_2;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::decrement_pointer_seq_1:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::decrement_pointer_seq_4;
          case 2:
            return token::decrement_pointer_seq_2;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::increment_byte_seq_1:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::increment_byte_seq_4;
          case 2:
            return token::increment_byte_seq_2;
          default: [[unlikely]]
            assert(false);
            return tok;
        }
      case token::decrement_byte_seq_1:
        switch (new_size) {
          case 4: [[unlikely]]
            return token::decrement_byte_seq_4;
          case 2:
            return token::decrement_byte_seq_2;
          default: [[unlikely]]
            assert(false);
            return tok;
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
        return tok;
    }
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

  template <typename T> requires (std::is_enum_v<T> && std::is_trivially_copyable_v<T>)
  static constexpr std::underlying_type_t<T> enum_cast(T value) {
    return std::bit_cast<std::underlying_type_t<T>>(value);
  }

  template <typename T> requires (std::is_trivially_copyable_v<T>)
  static constexpr std::array<uint8, sizeof(T)> array_cast(const T value) {
    return std::bit_cast<std::array<uint8, sizeof(T)>>(value);
  }

  template <typename T> requires(std::is_integral_v<T> && std::is_trivially_copyable_v<T>)
  static void push_back(byte_vector & __restrict vec, const T value) {
    if constexpr (sizeof(T) == 1) {
      vec.push_back(value);
    }
    else {
      auto bytes = array_cast<T>(value);

      const std::size_t start = vec.size();
      vec.resize(start + sizeof(T));

      std::copy(bytes.data(), bytes.data() + sizeof(T), vec.data() + start);
    }
  }

  template <typename T> requires(std::is_integral_v<T> && std::is_trivially_copyable_v<T>)
  static void insert_back(byte_vector & __restrict vec, const T value) {
    if constexpr (sizeof(T) == 1) {
      vec.back() = value;
    }
    else {
      auto bytes = array_cast<T>(value);

      const std::size_t start = vec.size() - 1;
      vec.resize(start + sizeof(T) - 1);

      std::copy(bytes.data(), bytes.data() + sizeof(T), vec.data() + start);
    }
  }

  static byte_vector recode(const char * __restrict code, const uint code_len, const std::unordered_map<uint32, uint32>& matching_brackets)
  {
  #if !RECODE
	  return byte_vector(code, code + code_len);
  #else
    byte_vector recoded;
    recoded.reserve(code_len);

    // We recode the Brainfuck so that it's faster to execute.
    // We will handle a few optimizations here. Mainly detecting sequences.

    static constexpr const uint seq_min = 3;

    {
      std::vector<uint32> loops;
      uint32 instruction_pointer = 0;
      std::printf("Original Program Size: %u\n", code_len);
      while (instruction_pointer < code_len)
      {
        const token i = token(code[instruction_pointer]);
        recoded.push_back(byte_cast(i));
        switch (i)
        {
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
            for (uint32 ii = instruction_pointer + 1; ii < code_len && code[ii] == token::increment_pointer_one; ++ii)
            {
              ++seq_cnt;
            }
            if (seq_cnt >= seq_min)
            {
              recoded.pop_back();

              if (seq_cnt <= std::numeric_limits<uint8>::max())
              {
                recoded.push_back(byte_cast(token::increment_pointer_seq_1));

                recoded.push_back(uint8(seq_cnt));
              }
              else if (seq_cnt <= std::numeric_limits<uint16>::max())
              {
                recoded.push_back(byte_cast(token::increment_pointer_seq_2));

                push_back(recoded, uint16(seq_cnt));
              }
              else
              {
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
            for (uint32 ii = instruction_pointer + 1; ii < code_len && code[ii] == token::decrement_pointer_one; ++ii)
            {
              ++seq_cnt;
            }
            if (seq_cnt >= seq_min)
            {
              recoded.pop_back();

              if (seq_cnt <= std::numeric_limits<uint8>::max())
              {
                recoded.push_back(byte_cast(token::decrement_pointer_seq_1));

                recoded.push_back(uint8(seq_cnt));
              }
              else if (seq_cnt <= std::numeric_limits<uint16>::max())
              {
                recoded.push_back(byte_cast(token::decrement_pointer_seq_2));

                push_back(recoded, uint16(seq_cnt));
              }
              else
              {
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
            for (uint32 ii = instruction_pointer + 1; ii < code_len && code[ii] == token::increment_byte_one; ++ii)
            {
              ++seq_cnt;
            }
            if (seq_cnt >= seq_min)
            {
              recoded.pop_back();

              if (seq_cnt <= std::numeric_limits<uint8>::max())
              {
                recoded.push_back(byte_cast(token::increment_byte_seq_1));

                recoded.push_back(uint8(seq_cnt));
              }
              else if (seq_cnt <= std::numeric_limits<uint16>::max())
              {
                recoded.push_back(byte_cast(token::increment_byte_seq_2));

                push_back(recoded, uint16(seq_cnt));
              }
              else
              {
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
            for (uint32 ii = instruction_pointer + 1; ii < code_len && code[ii] == token::decrement_byte_one; ++ii)
            {
              ++seq_cnt;
            }
            if (seq_cnt >= seq_min)
            {
              recoded.pop_back();

              if (seq_cnt <= std::numeric_limits<uint8>::max())
              {
                recoded.push_back(byte_cast(token::decrement_byte_seq_1));

                recoded.push_back(uint8(seq_cnt));
              }
              else if (seq_cnt <= std::numeric_limits<uint16>::max())
              {
                recoded.push_back(byte_cast(token::decrement_byte_seq_2));

                push_back(recoded, uint16(seq_cnt));
              }
              else
              {
                recoded.push_back(byte_cast(token::decrement_byte_seq_4));

                push_back(recoded, uint32(seq_cnt));
              }
              instruction_pointer += seq_cnt - 1;
            }
          }  break;
          case token::jump_forth:
          {
            // check for special case '[-]'
            if (code[instruction_pointer + 1] == token::decrement_byte_one && code[instruction_pointer + 2] == token::jump_back)
            {
              recoded[recoded.size() - 1] = byte_cast(token::zero_byte);
              instruction_pointer += 2;
              break;
            }
            // w = [<n] while (*ptr) ptr -= n
            {
              uint32 ii = std::min(code_len, instruction_pointer + 1);
              uint32 seq_len = 0;
              while (code[ii] == token::decrement_pointer_one)
              {
                ii = std::min(code_len, ii + 1);
                ++seq_len;
              }
              if (seq_len > 0 && seq_len < std::numeric_limits<uint8>::max() && code[ii] == token::jump_back)
              {
                recoded[recoded.size() - 1] = byte_cast(token::special_case_6);
                recoded.push_back(uint8(seq_len));
                instruction_pointer = ii;
                break;
              }
            }
            // x = [>n] while (*ptr) ptr += n
            {
              uint32 ii = std::min(code_len, instruction_pointer + 1);
              uint32 seq_len = 0;
              while (code[ii] == token::increment_pointer_one)
              {
                ii = std::min(code_len, ii + 1);
                ++seq_len;
              }
              if (seq_len > 0 && seq_len < std::numeric_limits<uint8>::max() && code[ii] == token::jump_back)
              {
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
              code[instruction_pointer + 5] == token::jump_back)
            {
              recoded[recoded.size() - 1] = byte_cast(token::special_case_1);
              instruction_pointer += 5;
              break;
            }
            // check for : // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
            {
              if (uint32 ii = std::min(code_len, instruction_pointer + 1); code[ii] == token::decrement_byte_one)
              {
                ii = std::min(code_len, ii + 1);
                uint32 seq_len = 0;
                while (code[ii] == token::increment_pointer_one)
                {
                  ii = std::min(code_len, ii + 1);
                  ++seq_len;
                }
                if (seq_len && seq_len <= std::numeric_limits<uint8>::max() && code[ii] == token::increment_byte_one) // TODO if we want 2- and 4-b sequences, change this.
                {
                  ii = std::min(code_len, ii + 1);
                  uint32 seq_len2 = 0;
                  while (code[ii] == token::decrement_pointer_one)
                  {
                    ii = std::min(code_len, ii + 1);
                    ++seq_len2;
                  }
                  if (seq_len2 == seq_len && code[ii] == token::jump_back)
                  {
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
              if (uint32 ii = std::min(code_len, instruction_pointer + 1); code[ii] == token::decrement_byte_one)
              {
                ii = std::min(code_len, ii + 1);
                uint32 seq_len = 0;
                while (code[ii] == token::decrement_pointer_one)
                {
                  ii = std::min(code_len, ii + 1);
                  ++seq_len;
                }
                if (seq_len && seq_len <= std::numeric_limits<uint8>::max() && code[ii] == token::increment_byte_one) // TODO if we want 2- and 4-b sequences, change this.
                {
                  ii = std::min(code_len, ii + 1);
                  uint32 seq_len2 = 0;
                  while (code[ii] == token::increment_pointer_one)
                  {
                    ii = std::min(code_len, ii + 1);
                    ++seq_len2;
                  }
                  if (seq_len2 == seq_len && code[ii] == token::jump_back)
                  {
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
              if (uint32 ii = std::min(code_len, instruction_pointer + 1); code[ii] == token::decrement_byte_one)
              {
                ii = std::min(code_len, ii + 1);
                uint32 seq_len = 0;
                while (code[ii] == token::increment_pointer_one)
                {
                  ii = std::min(code_len, ii + 1);
                  ++seq_len;
                }
                if (seq_len && seq_len <= std::numeric_limits<uint8>::max() && code[ii] == token::decrement_byte_one) // TODO if we want 2- and 4-b sequences, change this.
                {
                  ii = std::min(code_len, ii + 1);
                  uint32 seq_len2 = 0;
                  while (code[ii] == token::decrement_pointer_one)
                  {
                    ii = std::min(code_len, ii + 1);
                    ++seq_len2;
                  }
                  if (seq_len2 == seq_len && code[ii] == token::jump_back)
                  {
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
              if (uint32 ii = std::min(code_len, instruction_pointer + 1); code[ii] == token::decrement_byte_one)
              {
                ii = std::min(code_len, ii + 1);
                uint32 seq_len = 0;
                while (code[ii] == token::decrement_pointer_one)
                {
                  ii = std::min(code_len, ii + 1);
                  ++seq_len;
                }
                if (seq_len && seq_len <= std::numeric_limits<uint8>::max() && code[ii] == token::decrement_byte_one) // TODO if we want 2- and 4-b sequences, change this.
                {
                  ii = std::min(code_len, ii + 1);
                  uint32 seq_len2 = 0;
                  while (code[ii] == token::increment_pointer_one)
                  {
                    ii = std::min(code_len, ii + 1);
                    ++seq_len2;
                  }
                  if (seq_len2 == seq_len && code[ii] == token::jump_back)
                  {
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

            const uint32 target = code_len;
            const uint32 match = 0;

            if (target <= std::numeric_limits<uint8>::max())
            {
              recoded[recoded.size() - 1] = byte_cast(token::jump_forth_1);
              push_back(recoded, uint8(match));
            }
            else if (target <= std::numeric_limits<uint16>::max())
            {
              recoded[recoded.size() - 1] = byte_cast(token::jump_forth_2);
              push_back(recoded, uint16(match));
            }
            else
            {
              push_back(recoded, uint32(match));
            }
          }  break;
          case token::jump_back:
          {
            uint32 last_loop_offset = loops.back();
            loops.pop_back();

            const uint32 last_loop_end_offset = [&] 
            {
              uint32 operand_size;

              const uint32 lambda_offset = code_len;

              if (lambda_offset <= std::numeric_limits<uint8>::max()) [[likely]]
              {
                operand_size = sizeof(uint8);
                static_assert(sizeof(uint8) == 1);
              }
              else if (lambda_offset <= std::numeric_limits<uint16>::max())
              {
                operand_size = sizeof(uint16);
                static_assert(sizeof(uint16) == 2);
              }
              else [[unlikely]]
              {
                operand_size = sizeof(uint32);
                static_assert(sizeof(uint32) == 4);
              }

              return(recoded.size() - 1) - (last_loop_offset + 1 + operand_size);
            }();

            uint32 return_loc_val;

            if (last_loop_end_offset <= std::numeric_limits<uint8>::max())
            {
              return_loc_val = recoded.size() + 1;
              recoded[recoded.size() - 1] = byte_cast(token::jump_back_1);
              push_back(recoded, uint8(last_loop_end_offset));
            }
            else if (last_loop_end_offset <= std::numeric_limits<uint16>::max())
            {
              return_loc_val = recoded.size() + 2;
              recoded[recoded.size() - 1] = byte_cast(token::jump_back_2);
              push_back(recoded, uint16(last_loop_end_offset));
            }
            else
            {
              return_loc_val = recoded.size() + 4;
              push_back(recoded, uint32(last_loop_end_offset));
            }

#if !PRE_SCAN
            const auto get_target_size = [] (const uint32 value) {
              if (value <= std::numeric_limits<uint8>::max()) {
                return 1;
              }
              else if (value <= std::numeric_limits<uint16>::max()) [[likely]] {
                return 2;
              }
              else if (value <= std::numeric_limits<uint32>::max()) [[unlikely]] {
                return 4;
              }
              else {
                UNREACHABLE();
                return 0;
              }
            };

            static constexpr const bool recode_instruction = false;

            if constexpr (recode_instruction) {
              const auto reencode_start = [&] (uint32 target) {
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

            const uint32 compare_value = recode_instruction ? return_loc_val : code_len;

            if (compare_value <= std::numeric_limits<uint8>::max())
            {
              *reinterpret_cast<uint8* __restrict>(return_loc) = uint8(return_loc_val);
            }
            else if (compare_value <= std::numeric_limits<uint16>::max()) [[likely]]
            {
              *reinterpret_cast<uint16* __restrict>(return_loc) = uint16(return_loc_val);
            }
            else if (compare_value <= std::numeric_limits<uint32>::max()) [[unlikely]]
            {
              *reinterpret_cast<uint32* __restrict>(return_loc) = return_loc_val;
            }
            else {
              UNREACHABLE();
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

  template <uint32 count = 32'768>
  struct cell_t final : std::array<uint8, count> {
  private:
    static constexpr const bool count_pow2 = (count & (count - 1u)) == 0u;

  public:
    uint32 index = 0;

    uint8 & operator++() __restrict {
      ++index;
      if constexpr (count_pow2) {
        index &= count - 1;
      }
      else {
        if (index >= count) [[unlikely]] {
          index %= count;
        }
      }

      return (*this)[index];
    }

    uint8 & operator++(int) __restrict {
      const uint32 original_index = index;
      ++index;
      if constexpr (count_pow2) {
        index &= count - 1;
      }
      else {
        if (index >= count) [[unlikely]] {
          index %= count;
        }
      }

      return (*this)[original_index];
    }

    uint8 & operator--() __restrict {
      if constexpr (count_pow2) {
        --index;
        index &= count - 1;
      }
      else {
        if (1 > index) [[unlikely]] {
          index = count - 1;
        }
      }

      return (*this)[index];
    }

    uint8 & operator--(int) __restrict {
      const uint32 original_index = index;
      if constexpr (count_pow2) {
        --index;
        index &= count - 1;
      }
      else {
        if (1 > index) [[unlikely]] {
          index = count - 1;
        }
      }

      return (*this)[original_index];
    }

    uint8 & operator+(uint32 value) __restrict {
      uint32 current_index = index;
      current_index += value;
      if constexpr (count_pow2) {
        current_index &= count - 1;
      }
      else {
        if (current_index >= count) [[unlikely]] {
          current_index %= count;
        }
      }

      return (*this)[current_index];
    }

    uint8 & operator-(uint32 value) __restrict {
      uint32 current_index = index;
      if constexpr (count_pow2) {
        current_index -= value;
        current_index &= count - 1;
      }
      else {
        if (value > current_index) [[unlikely]] {
          current_index = count - (value - current_index);
        }
      }

      return (*this)[current_index];
    }

    uint8 & operator+=(uint32 value) __restrict {
      uint32 current_index = index;
      current_index += value;
      if constexpr (count_pow2) {
        current_index &= count - 1;
      }
      else {
        if (current_index >= count) [[unlikely]] {
          current_index %= count;
        }
      }

      index = current_index;
      return (*this)[current_index];
    }

    uint8 & operator-=(uint32 value) __restrict {
      uint32 current_index = index;
      if constexpr (count_pow2) {
        current_index -= value;
        current_index &= count - 1;
      }
      else {
        if (value > current_index) [[unlikely]] {
          current_index = count - (value - current_index);
        }
      }

      index = current_index;
      return (*this)[current_index];
    }

    uint8 & operator*() __restrict {
      return (*this)[index];
    }
  };

  static cell_t cells = {};
}

int main()
{
  static constexpr const char * const __restrict code = bench::program;
  static constexpr size_t code_len = sizeof(bench::program);
  check(code_len <= std::numeric_limits<uint32>::max());

  // Pre-scan the code for matching brackets to get offsets.
  // Also will allow the recoder to use smaller offsets.
  std::unordered_map<uint32, uint32> matching_brackets;
#if PRE_SCAN
  {
    std::vector<uint32> bracket_stack;

    for (uint32 i = 0; i < code_len; ++i) {
      switch (code[i]) {
        case '[':
          bracket_stack.push_back(i);
          break;
        case ']': {
          check(!bracket_stack.empty());
          const uint32 matching_offset = bracket_stack.back();
          bracket_stack.pop_back();
          matching_brackets.insert({i, matching_offset});
          matching_brackets.insert({matching_offset, i});
        } break;
        default:
          break;
      }
    }

    check(bracket_stack.empty());
  }
#endif

  std::printf("Generating %s via Brainfuck\n", bench::name);
  const byte_vector recoded = recode(code, code_len, matching_brackets);

  {
    uint32 instruction_pointer = 0;

    // Now execute it recoded.

    check(recoded.size() <= std::numeric_limits<uint32>::max());
    const uint32 recoded_size = uint32(recoded.size());
    const uint8 * __restrict recoded_data = recoded.data();

    std::printf("Recoded Program Size: %u\n", recoded_size);
    while (instruction_pointer < recoded_size)
    {
      const token i = static_cast<const token>(recoded_data[instruction_pointer]);
      switch (i)
      {
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
          ++instruction_pointer;
          const uint8 count = recoded_data[instruction_pointer];
          while (*cells)
          {
            cells -= count;
          }
        } break;
        case token::special_case_7:
        {
          ++instruction_pointer;
          const uint8 count = recoded_data[instruction_pointer];
          while (*cells)
          {
            cells += count;
          }
        } break;
        case token::special_case_2: // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
        {
          ++instruction_pointer;
          const uint8 count = recoded_data[instruction_pointer];
          (cells + count) += *cells;
          *cells = 0;
        } break;
        case token::special_case_3: // t = special case [->n+<n] (*(ptr - n) += *ptr; *ptr = 0
        {
          ++instruction_pointer;
          const uint8 count = recoded_data[instruction_pointer];
          (cells - count) += *cells;
          *cells = 0;
        } break;
        case token::special_case_4: // u = special case [->n+<n] (*(ptr + n) -= *ptr; *ptr = 0
        {
          ++instruction_pointer;
          const uint8 count = recoded_data[instruction_pointer];
          (cells + count) -= *cells;
          *cells = 0;
        } break;
        case token::special_case_5: // v = special case [->n+<n] (*(ptr - n) -= *ptr; *ptr = 0
        {
          ++instruction_pointer;
          const uint8 count = recoded_data[instruction_pointer];
          (cells - count) -= *cells;
          *cells = 0;
        } break;
        case token::increment_pointer_seq_4: // > sequence special case
        {
          ++instruction_pointer;
          const uint32 value = reinterpret_cast<const uint32 & __restrict>(recoded_data[instruction_pointer]);
          instruction_pointer += 3;
          cells += value;
        } break;
        case token::decrement_pointer_seq_4: // < sequence special case
        {
          ++instruction_pointer;
          const uint32 value = reinterpret_cast<const uint32 & __restrict>(recoded_data[instruction_pointer]);
          instruction_pointer += 3;
          cells -= value;
        } break;
        case token::increment_byte_seq_4: // + sequence special case
        {
          ++instruction_pointer;
          const uint32 value = reinterpret_cast<const uint32 & __restrict>(recoded_data[instruction_pointer]);
          instruction_pointer += 3;
          *cells += value;
        } break;
        case token::decrement_byte_seq_4: // - sequence special case
        {
          ++instruction_pointer;
          const uint32 value = reinterpret_cast<const uint32 & __restrict>(recoded_data[instruction_pointer]);
          instruction_pointer += 3;
          *cells -= value;
        } break;
        case token::increment_pointer_seq_2: // > sequence special case
        {
          ++instruction_pointer;
          const uint16 value = reinterpret_cast<const uint16 & __restrict>(recoded_data[instruction_pointer]);
          ++instruction_pointer;
          cells += value;
        } break;
        case token::decrement_pointer_seq_2: // < sequence special case
        {
          ++instruction_pointer;
          const uint16 value = reinterpret_cast<const uint16 & __restrict>(recoded_data[instruction_pointer]);
          ++instruction_pointer;
          cells -= value;
        } break;
        case token::increment_byte_seq_2: // + sequence special case
        {
          ++instruction_pointer;
          const uint16 value = reinterpret_cast<const uint16 & __restrict>(recoded_data[instruction_pointer]);
          ++instruction_pointer;
          *cells += value;
        } break;
        case token::decrement_byte_seq_2: // - sequence special case
        {
          ++instruction_pointer;
          const uint16 value = reinterpret_cast<const uint16 & __restrict>(recoded_data[instruction_pointer]);
          ++instruction_pointer;
          *cells -= value;
        } break;
        case token::increment_pointer_seq_1: // > sequence special case
        {
          ++instruction_pointer;
          cells += recoded_data[instruction_pointer];
        } break;
        case token::decrement_pointer_seq_1: // < sequence special case
        {
          ++instruction_pointer;
          cells -= recoded_data[instruction_pointer];
        } break;
        case token::increment_byte_seq_1: // + sequence special case
        {
          ++instruction_pointer;
          *cells += recoded_data[instruction_pointer];
        } break;
        case token::decrement_byte_seq_1: // - sequence special case
        {
          ++instruction_pointer;
          *cells -= recoded_data[instruction_pointer];
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
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          break;
        case token::input_byte: [[unlikely]]
          // don't handle yet.
          break;
        case token::jump_forth:
          if (*cells == 0)
          {
#if RECODE
            // Jump.
            const uint32 value = reinterpret_cast<const uint32 & __restrict>(recoded_data[instruction_pointer + 1]);
            instruction_pointer = value;
            continue;
#else
# if PRE_SCAN
            instruction_pointer = matching_brackets[instruction_pointer];
# else
            uint32 extra_count = 0;
            uint8 t;
            while (((t = recoded_data[++instruction_pointer]) != ']') | (extra_count != 0)) {
              if (t == '[') {
                ++extra_count;
              }
              else if (t == ']') {
                --extra_count;
              }
            }
# endif
#endif
          }
#if RECODE
          else
          {
            instruction_pointer += 4;
          }
#endif
          break;
        case token::jump_back:
          if (*cells != 0)
          {
#if RECODE
            // Jump.
            const uint32 value = reinterpret_cast<const uint32 & __restrict>(recoded_data[instruction_pointer + 1]);
            instruction_pointer -= value;
            continue;
#else
# if PRE_SCAN
            instruction_pointer = matching_brackets[instruction_pointer];
# else
            uint32 extra_count = 0;
            uint8 t;
            while (((t = recoded_data[--instruction_pointer]) != '[') | (extra_count != 0)) {
              if (t == ']') {
                ++extra_count;
              }
              else if (t == '[') {
                --extra_count;
              }
            }
# endif
#endif
          }
#if RECODE
          else
          {
            instruction_pointer += 4;
          }
#endif
          break;
#if RECODE
        case token::jump_back_2:
          if (*cells != 0)
          {
            // Jump.
            const uint16 value = reinterpret_cast<const uint16 & __restrict>(recoded_data[instruction_pointer + 1]);
            instruction_pointer -= value;
            continue;
          }
          else
          {
            instruction_pointer += 2;
          } break;
        case token::jump_back_1:
          if (*cells != 0)
          {
            // Jump.
            instruction_pointer -= recoded_data[instruction_pointer + 1];
            continue;
          }
          else
          {
            ++instruction_pointer;
          } break;
        case token::jump_forth_2:
          if (*cells == 0)
          {
            // Jump.
            const uint16 value = reinterpret_cast<const uint16 & __restrict>(recoded_data[instruction_pointer + 1]);
            instruction_pointer = value;
            continue;
          }
          else
          {
            instruction_pointer += 2;
          } break;
        case token::jump_forth_1: [[unlikely]]
          if (*cells == 0)
          {
            // Jump.
            instruction_pointer = recoded_data[instruction_pointer + 1];
            continue;
          }
          else
          {
            ++instruction_pointer;
          } break;
#endif
        case token('\0'): [[unlikely]]
          break;
        default: [[unlikely]]
            // recoding strips unexpected tokens
#if RECODE
          UNREACHABLE();
#endif
          break;
      }
      ++instruction_pointer;
    }


#if PRINT_RECODE

    std::printf("Recoded Bytecode:\n\n");

    instruction_pointer = 0;
    while (instruction_pointer < recoded_size)
    {
      const token i = token(recoded_data[instruction_pointer]);
      std::putchar(enum_cast(i));

      switch (i)
      {
#if RECODE
        case token::zero_byte: // [-] special case
          break;
        case token::special_case_1: // r = special case [->+<] (*(ptr - 1) += *ptr; *ptr = 0
          break;
        case token::special_case_6:
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::special_case_7:
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::special_case_2: // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::special_case_3: // t = special case [->n+<n] (*(ptr - n) += *ptr; *ptr = 0
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::special_case_4: // u = special case [->n+<n] (*(ptr + n) -= *ptr; *ptr = 0
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::special_case_5: // v = special case [->n+<n] (*(ptr - n) -= *ptr; *ptr = 0
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::increment_pointer_seq_4: // > sequence special case
        {
          //std::fputs("####", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 4;
        } break;
        case token::decrement_pointer_seq_4: // < sequence special case
        {
          //std::fputs("####", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 4;
        } break;
        case token::increment_byte_seq_4: // + sequence special case
        {
          //std::fputs("####", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 4;
        } break;
        case token::decrement_byte_seq_4: // - sequence special case
        {
          //std::fputs("####", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 4;
        } break;
        case token::increment_pointer_seq_2: // > sequence special case
        {
          //std::fputs("##", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 2;
        } break;
        case token::decrement_pointer_seq_2: // < sequence special case
        {
          //std::fputs("##", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 2;
        } break;
        case token::increment_byte_seq_2: // + sequence special case
        {
          //std::fputs("##", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 2;
        } break;
        case token::decrement_byte_seq_2: // - sequence special case
        {
          //std::fputs("##", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 2;
        } break;
        case token::increment_pointer_seq_1: // > sequence special case
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::decrement_pointer_seq_1: // < sequence special case
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::increment_byte_seq_1: // + sequence special case
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::decrement_byte_seq_1: // - sequence special case
        {
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
        } break;
        case token::increment_pointer_one:
          break;
        case token::decrement_pointer_one:
          break;
        case token::increment_byte_one:
          break;
        case token::decrement_byte_one:
          break;
        case token::output_byte:
          break;
        case token::input_byte:
          // don't handle yet.
          break;
        case token::jump_forth:
          //std::fputs("####", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 4;
          break;
        case token::jump_back:
          //std::fputs("####", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 4;
          break;
        case token::jump_back_2:
          //std::fputs("##", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 2;
          break;
        case token::jump_back_1:
          //std::putchar('#');
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          ++instruction_pointer;
          break;
        case token::jump_forth_2:
          //std::fputs("##", stdout);
          std::fflush(stdout);  // NOLINT(cert-err33-c)
          instruction_pointer += 2;
          break;
#endif
        case token('\0'):
          break;
        default:
          UNREACHABLE();
      }
      ++instruction_pointer;
    }
#endif
  }
  std::fflush(stdout);  // NOLINT(cert-err33-c)
}
