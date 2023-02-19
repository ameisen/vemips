#ifndef BENCHMARK
#	define BENCHMARK "mandelbrot.inc"
#endif
#ifndef RECODE
#	define RECODE 1
#endif
#ifndef PRINT_RECODE
#	define PRINT_RECODE 0
#endif

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include BENCHMARK

#define _unreachable __builtin_unreachable()
#define restrict __restrict

using uint = unsigned int;

using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;

using byte_vector = std::vector<uint8>;

struct uint16u final
{
	union
	{
		uint16 as_ushort;
		uint8 as_bytes[sizeof(as_ushort)];
	};
	
	uint16u() = default;
	uint16u(uint16 v) : as_ushort(v) {}
	uint16u(uint32 v) : as_ushort(uint16(v)) {}
};

struct uint32u final
{
	union
	{
		uint32 as_uint;
		uint16u as_ushort[sizeof(as_uint) / sizeof(uint16)];
		uint8 as_bytes[sizeof(as_uint)];
	};
	
	uint32u() = default;
	uint32u(uint32 v) : as_uint(v) {}
};

static byte_vector recode(const char * restrict code, const uint code_len)
{
#if !RECODE
	return byte_vector(code, code + code_len);
#else
  byte_vector recoded;
  recoded.reserve(code_len);

  // We recode the Brainfuck so that it's faster to execute.
  // We will handle a few optimizations here. Mainly detecting sequences.

  static constexpr const uint seq_min = 3;

  // b == > seq 4b
  // c == < seq 4b
  // d == + seq 4b
  // e == - seq 4b
  // f == > seq 2b
  // g == < seq 2b
  // h == + seq 2b
  // i == - seq 2b
  // j == > seq 1b
  // k == < seq 1b
  // l == + seq 1b
  // m == - seq 1b
  // n == ] 2b
  // o == ] 1b
  // p == [ 2b
  // q == [ 1b //unimplemented
  // r = special case [->+<] (*(ptr + 1) += *ptr; *ptr = 0
  // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
  // t = special case [-<n+>n] (*(ptr - n) += *ptr; *ptr = 0
  // u = special case [->n-<n] (*(ptr + n) -= *ptr; *ptr = 0
  // v = special case [-<n->n] (*(ptr - n) -= *ptr; *ptr = 0
  // [->>>>>>>+<<+<<<+<<]
  // [->>+>>+<<<<]
  // [-<+>>>>+<<[-]]
  // w = [<n] while (*ptr) ptr -= n
  // x = [>n] while (*ptr) ptr += n

  {
    std::vector<uint32> loops;
    uint32 instruction_pointer = 0;
    printf("Original Program Size: %u\n", code_len);
    while (instruction_pointer < code_len)
    {
      const char i = code[instruction_pointer];
      recoded.push_back(i);
      switch (i)
      {
        case '>':
        {
          // See if there's a sequence of them after it.
          uint32 ii = instruction_pointer + 1;
          uint32 seq_cnt = 1;
          for (; ii < code_len && code[ii] == '>'; ++ii)
          {
            ++seq_cnt;
          }
          if (seq_cnt >= seq_min)
          {
            recoded.pop_back();

            if (seq_cnt <= uint8(-1))
            {
              recoded.push_back('j');

              recoded.push_back(seq_cnt);
            }
            else if (seq_cnt <= uint16(-1))
            {
              recoded.push_back('f');

              uint16u ub = {uint16(seq_cnt)};

              recoded.push_back(ub.as_bytes[0]);
              recoded.push_back(ub.as_bytes[1]);
            }
            else
            {
              recoded.push_back('b');

							uint32u ub = {seq_cnt};

              recoded.push_back(ub.as_bytes[0]);
              recoded.push_back(ub.as_bytes[1]);
              recoded.push_back(ub.as_bytes[2]);
              recoded.push_back(ub.as_bytes[3]);
            }
            instruction_pointer += seq_cnt - 1;
          }
        }  break;
        case '<':
        {
          // See if there's a sequence of them after it.
          uint32 ii = instruction_pointer + 1;
          uint32 seq_cnt = 1;
          for (; ii < code_len && code[ii] == '<'; ++ii)
          {
            ++seq_cnt;
          }
          if (seq_cnt >= seq_min)
          {
            recoded.pop_back();

            if (seq_cnt <= uint8(-1))
            {
              recoded.push_back('k');

              recoded.push_back(seq_cnt);
            }
            else if (seq_cnt <= uint16(-1))
            {
              recoded.push_back('g');

              uint16u ub = {uint16(seq_cnt)};

              recoded.push_back(ub.as_bytes[0]);
              recoded.push_back(ub.as_bytes[1]);
            }
            else
            {
              recoded.push_back('c');

              union
              {
                uint32 as_uint;
                uint8 as_bytes[4];
              } ub;
              ub.as_uint = seq_cnt;

              recoded.push_back(ub.as_bytes[0]);
              recoded.push_back(ub.as_bytes[1]);
              recoded.push_back(ub.as_bytes[2]);
              recoded.push_back(ub.as_bytes[3]);
            }
            instruction_pointer += seq_cnt - 1;
          }
        }  break;
        case '+':
        {
          // See if there's a sequence of them after it.
          uint32 ii = instruction_pointer + 1;
          uint32 seq_cnt = 1;
          for (; ii < code_len && code[ii] == '+'; ++ii)
          {
            ++seq_cnt;
          }
          if (seq_cnt >= seq_min)
          {
            recoded.pop_back();

            if (seq_cnt <= uint8(-1))
            {
              recoded.push_back('l');

              recoded.push_back(seq_cnt);
            }
            else if (seq_cnt <= uint16(-1))
            {
              recoded.push_back('h');

              union
              {
                uint16 as_uint;
                uint8 as_bytes[2];
              } ub;
              ub.as_uint = seq_cnt;

              recoded.push_back(ub.as_bytes[0]);
              recoded.push_back(ub.as_bytes[1]);
            }
            else
            {
              recoded.push_back('d');

              union
              {
                uint32 as_uint;
                uint8 as_bytes[4];
              } ub;
              ub.as_uint = seq_cnt;

              recoded.push_back(ub.as_bytes[0]);
              recoded.push_back(ub.as_bytes[1]);
              recoded.push_back(ub.as_bytes[2]);
              recoded.push_back(ub.as_bytes[3]);
            }
            instruction_pointer += seq_cnt - 1;
          }
        }  break;
        case '-':
        {
          // See if there's a sequence of them after it.
          uint32 ii = instruction_pointer + 1;
          uint32 seq_cnt = 1;
          for (; ii < code_len && code[ii] == '-'; ++ii)
          {
            ++seq_cnt;
          }
          if (seq_cnt >= seq_min)
          {
            recoded.pop_back();

            if (seq_cnt <= uint8(-1))
            {
              recoded.push_back('m');

              recoded.push_back(seq_cnt);
            }
            else if (seq_cnt <= uint16(-1))
            {
              recoded.push_back('i');

              union
              {
                uint16 as_uint;
                uint8 as_bytes[2];
              } ub;
              ub.as_uint = seq_cnt;

              recoded.push_back(ub.as_bytes[0]);
              recoded.push_back(ub.as_bytes[1]);
            }
            else
            {
              recoded.push_back('e');

              union
              {
                uint32 as_uint;
                uint8 as_bytes[4];
              } ub;
              ub.as_uint = seq_cnt;

              recoded.push_back(ub.as_bytes[0]);
              recoded.push_back(ub.as_bytes[1]);
              recoded.push_back(ub.as_bytes[2]);
              recoded.push_back(ub.as_bytes[3]);
            }
            instruction_pointer += seq_cnt - 1;
          }
        }  break;
        case '[':
        {
          // check for special case '[-]'
          if (code[instruction_pointer + 1] == '-' && code[instruction_pointer + 2] == ']')
          {
            recoded.pop_back();
            recoded.push_back('a');
            instruction_pointer += 2;
            break;
          }
          // w = [<n] while (*ptr) ptr -= n
          {
            uint32 ii = std::min(code_len, instruction_pointer + 1);
            uint32 seq_len = 0;
            while (code[ii] == '<')
            {
              ii = std::min(code_len, ii + 1);
              ++seq_len;
            }
            if (seq_len > 0 && seq_len < 0xFF && code[ii] == ']')
            {
              recoded[recoded.size() - 1] = 'w';
              recoded.push_back(seq_len);
              instruction_pointer = ii;
              break;
            }
          }
          // x = [>n] while (*ptr) ptr += n
          {
            uint32 ii = std::min(code_len, instruction_pointer + 1);
            uint32 seq_len = 0;
            while (code[ii] == '>')
            {
              ii = std::min(code_len, ii + 1);
              ++seq_len;
            }
            if (seq_len > 0 && seq_len < 0xFF && code[ii] == ']')
            {
              recoded[recoded.size() - 1] = 'x';
              recoded.push_back(seq_len);
              instruction_pointer = ii;
              break;
            }
          }
          // check for special case [->+<]
          if (
            code[instruction_pointer + 1] == '-' &&
            code[instruction_pointer + 2] == '>' &&
            code[instruction_pointer + 3] == '+' &&
            code[instruction_pointer + 4] == '<' &&
            code[instruction_pointer + 5] == ']')
          {
            recoded.pop_back();
            recoded.push_back('r');
            instruction_pointer += 5;
            break;
          }
          // check for : // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
          {
            uint32 ii = std::min(code_len, instruction_pointer + 1);
            if (code[ii] == '-')
            {
              ii = std::min(code_len, ii + 1);
              uint32 seq_len = 0;
              while (code[ii] == '>')
              {
                ii = std::min(code_len, ii + 1);
                ++seq_len;
              }
              if (seq_len && seq_len <= 0xFF && code[ii] == '+') // TODO if we want 2- and 4-b sequences, change this.
              {
                ii = std::min(code_len, ii + 1);
                uint32 seq_len2 = 0;
                while (code[ii] == '<')
                {
                  ii = std::min(code_len, ii + 1);
                  ++seq_len2;
                }
                if (seq_len2 == seq_len && code[ii] == ']')
                {
                  recoded[recoded.size() - 1] = 's';
                  recoded.push_back(seq_len);
                  instruction_pointer = ii;
                  break;
                }
              }
            }
          }
          // check for : // t = special case [->n+<n] (*(ptr - n) += *ptr; *ptr = 0
          {
            uint32 ii = std::min(code_len, instruction_pointer + 1);
            if (code[ii] == '-')
            {
              ii = std::min(code_len, ii + 1);
              uint32 seq_len = 0;
              while (code[ii] == '<')
              {
                ii = std::min(code_len, ii + 1);
                ++seq_len;
              }
              if (seq_len && seq_len <= 0xFF && code[ii] == '+') // TODO if we want 2- and 4-b sequences, change this.
              {
                ii = std::min(code_len, ii + 1);
                uint32 seq_len2 = 0;
                while (code[ii] == '>')
                {
                  ii = std::min(code_len, ii + 1);
                  ++seq_len2;
                }
                if (seq_len2 == seq_len && code[ii] == ']')
                {
                  recoded[recoded.size() - 1] = 't';
                  recoded.push_back(seq_len);
                  instruction_pointer = ii;
                  break;
                }
              }
            }
          }
          // check for : // s = special case [->n+<n] (*(ptr + n) -= *ptr; *ptr = 0
          {
            uint32 ii = std::min(code_len, instruction_pointer + 1);
            if (code[ii] == '-')
            {
              ii = std::min(code_len, ii + 1);
              uint32 seq_len = 0;
              while (code[ii] == '>')
              {
                ii = std::min(code_len, ii + 1);
                ++seq_len;
              }
              if (seq_len && seq_len <= 0xFF && code[ii] == '-') // TODO if we want 2- and 4-b sequences, change this.
              {
                ii = std::min(code_len, ii + 1);
                uint32 seq_len2 = 0;
                while (code[ii] == '<')
                {
                  ii = std::min(code_len, ii + 1);
                  ++seq_len2;
                }
                if (seq_len2 == seq_len && code[ii] == ']')
                {
                  recoded[recoded.size() - 1] = 'u';
                  recoded.push_back(seq_len);
                  instruction_pointer = ii;
                  break;
                }
              }
            }
          }
          // check for : // t = special case [->n+<n] (*(ptr - n) -= *ptr; *ptr = 0
          {
            uint32 ii = std::min(code_len, instruction_pointer + 1);
            if (code[ii] == '-')
            {
              ii = std::min(code_len, ii + 1);
              uint32 seq_len = 0;
              while (code[ii] == '<')
              {
                ii = std::min(code_len, ii + 1);
                ++seq_len;
              }
              if (seq_len && seq_len <= 0xFF && code[ii] == '-') // TODO if we want 2- and 4-b sequences, change this.
              {
                ii = std::min(code_len, ii + 1);
                uint32 seq_len2 = 0;
                while (code[ii] == '>')
                {
                  ii = std::min(code_len, ii + 1);
                  ++seq_len2;
                }
                if (seq_len2 == seq_len && code[ii] == ']')
                {
                  recoded[recoded.size() - 1] = 'v';
                  recoded.push_back(seq_len);
                  instruction_pointer = ii;
                  break;
                }
              }
            }
          }


          // r = special case [->+<] (*(ptr - 1) += *ptr; *ptr = 0

          loops.push_back(recoded.size() - 1);
          
          if (code_len <= 0xFFFF)
          {
            recoded[recoded.size() - 1] = 'p';
            recoded.push_back(0);
            recoded.push_back(0);
          }
          else
          {
            recoded.push_back(0);
            recoded.push_back(0);
            recoded.push_back(0);
            recoded.push_back(0);
          }
        }  break;
        case ']':
        {
          union
          {
            uint32 as_uint;
            uint8 as_bytes[4];
          } ub;
          ub.as_uint = loops.back();
          loops.pop_back();

          // write this offset back in.
          uint32 * __restrict return_loc = (uint32 * __restrict)(recoded.data() + ub.as_uint + 1);

          if (code_len <= 0xFFFF)
          {
            ub.as_uint = (recoded.size() - 1) - (ub.as_uint + 3);
          }
          else
          {
            ub.as_uint = (recoded.size() - 1) - (ub.as_uint + 5);
          }

          uint32 return_loc_val;

          if (ub.as_uint <= uint8(-1))
          {
            return_loc_val = recoded.size() + 1;
            recoded[recoded.size() - 1] = 'o';
            recoded.push_back(ub.as_bytes[0]);
          }
          else if (ub.as_uint <= uint16(-1))
          {
            return_loc_val = recoded.size() + 2;
            recoded[recoded.size() - 1] = 'n';
            recoded.push_back(ub.as_bytes[0]);
            recoded.push_back(ub.as_bytes[1]);
          }
          else
          {
            return_loc_val = recoded.size() + 4;
            recoded.push_back(ub.as_bytes[0]);
            recoded.push_back(ub.as_bytes[1]);
            recoded.push_back(ub.as_bytes[2]);
            recoded.push_back(ub.as_bytes[3]);
          }

          if (code_len <= 0xFFFF)
          {
            *(uint16 * __restrict)return_loc = return_loc_val;
          }
          else
          {
            *return_loc = return_loc_val;
          }

        }  break;
      }
      ++instruction_pointer;
    }
  }

  return recoded;
#endif
}

int main()
{
  const char * restrict code = bench::program;
  const size_t code_len = sizeof(bench::program);
	 printf("Generating %s via Brainfsck\n", bench::name);
  const byte_vector recoded = recode(code, code_len);

  // b == > seq 4b
  // c == < seq 4b
  // d == + seq 4b
  // e == - seq 4b
  // f == > seq 2b
  // g == < seq 2b
  // h == + seq 2b
  // i == - seq 2b
  // j == > seq 1b
  // k == < seq 1b
  // l == + seq 1b
  // m == - seq 1b

  {
    uint32 instruction_pointer = 0;
    uint8 * __restrict ptr;
    uint8 cells[30'000];
    memset(cells, 0, sizeof(cells));
    ptr = &cells[0];
    // Now execute it recoded.

    const uint32 recoded_size = recoded.size();
    const uint8 * __restrict recoded_data = recoded.data();

    printf("Recoded Program Size: %u\n", recoded_size);
    while (instruction_pointer < recoded_size)
    {
      const char i = recoded_data[instruction_pointer];
      switch (i)
      {
        case 'a': // [-] special case
          *ptr = 0; break;
        case 'r': // r = special case [->+<] (*(ptr - 1) += *ptr; *ptr = 0
          *(ptr + 1) += *ptr; *ptr = 0; break;
        case 'w':
        {
          const uint8 count = recoded_data[instruction_pointer + 1];
          ++instruction_pointer;
          while (*ptr)
          {
            ptr -= count;
          }
        } break;
        case 'x':
        {
          const uint8 count = recoded_data[instruction_pointer + 1];
          ++instruction_pointer;
          while (*ptr)
          {
            ptr += count;
          }
        } break;
        case 's': // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
        {
          const uint8 count = recoded_data[instruction_pointer + 1];
          *(ptr + count) += *ptr; *ptr = 0;
          ++instruction_pointer;
        } break;
        case 't': // t = special case [->n+<n] (*(ptr - n) += *ptr; *ptr = 0
        {
          const uint8 count = recoded_data[instruction_pointer + 1];
          *(ptr - count) += *ptr; *ptr = 0;
          ++instruction_pointer;
        } break;
        case 'u': // u = special case [->n+<n] (*(ptr + n) -= *ptr; *ptr = 0
        {
          const uint8 count = recoded_data[instruction_pointer + 1];
          *(ptr + count) -= *ptr; *ptr = 0;
          ++instruction_pointer;
        } break;
        case 'v': // v = special case [->n+<n] (*(ptr - n) -= *ptr; *ptr = 0
        {
          const uint8 count = recoded_data[instruction_pointer + 1];
          *(ptr - count) -= *ptr; *ptr = 0;
          ++instruction_pointer;
        } break;
        case 'b': // > sequence special case
        {
          const uint32 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
          instruction_pointer += 4;
          ptr += value;
        } break;
        case 'c': // < sequence special case
        {
          const uint32 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
          instruction_pointer += 4;
          ptr -= value;
        } break;
        case 'd': // + sequence special case
        {
          const uint32 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
          instruction_pointer += 4;
          *ptr += value;
        } break;
        case 'e': // - sequence special case
        {
          const uint32 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
          instruction_pointer += 4;
          *ptr -= value;
        } break;
        case 'f': // > sequence special case
        {
          const uint16 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
          instruction_pointer += 2;
          ptr += value;
        } break;
        case 'g': // < sequence special case
        {
          const uint16 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
          instruction_pointer += 2;
          ptr -= value;
        } break;
        case 'h': // + sequence special case
        {
          const uint16 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
          instruction_pointer += 2;
          *ptr += value;
        } break;
        case 'i': // - sequence special case
        {
          const uint16 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
          instruction_pointer += 2;
          *ptr -= value;
        } break;
        case 'j': // > sequence special case
        {
          ptr += recoded_data[instruction_pointer + 1];
          ++instruction_pointer;
        } break;
        case 'k': // < sequence special case
        {
          ptr -= recoded_data[instruction_pointer + 1];
          ++instruction_pointer;
        } break;
        case 'l': // + sequence special case
        {
          *ptr += recoded_data[instruction_pointer + 1];
          ++instruction_pointer;
        } break;
        case 'm': // - sequence special case
        {
          *ptr -= recoded_data[instruction_pointer + 1];
          ++instruction_pointer;
        } break;
        case '>':
          ++ptr; break;
        case '<':
          --ptr; break;
        case '+':
          ++*ptr; break;
        case '-':
          --*ptr; break;
        case '.':
          putchar(*ptr); fflush(stdout); break;
        case ',':
          // don't handle yet.
          break;
        case '[':
          if (*ptr == 0)
          {
            // Jump.
            const uint32 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
            instruction_pointer = value;
            continue;
          }
          else
          {
            instruction_pointer += 4;
          } break;
        case ']':
          if (*ptr != 0)
          {
            // Jump.
            const uint32 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
            instruction_pointer = instruction_pointer - value;
            continue;
          }
          else
          {
            instruction_pointer += 4;
          } break;
        case 'n':
          if (*ptr != 0)
          {
            // Jump.
            const uint16 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
            instruction_pointer = instruction_pointer - value;
            continue;
          }
          else
          {
            instruction_pointer += 2;
          } break;
        case 'o':
          if (*ptr != 0)
          {
            // Jump.
            instruction_pointer = instruction_pointer - recoded_data[instruction_pointer + 1];
            continue;
          }
          else
          {
            ++instruction_pointer;
          } break;
        case 'p':
          if (*ptr == 0)
          {
            // Jump.
            const uint16 value = (uint32 & __restrict)recoded_data[instruction_pointer + 1];
            instruction_pointer = value;
            continue;
          }
          else
          {
            instruction_pointer += 2;
          } break;
        case 0:
          break;
        default:
          _unreachable;
      }
      ++instruction_pointer;
    }


#if PRINT_RECODE

    printf("Recoded Bytecode:\n\n");

    instruction_pointer = 0;
    while (instruction_pointer < recoded_size)
    {
      const char i = recoded_data[instruction_pointer];
      putchar(i);

      switch (i)
      {
        case 'a': // [-] special case
          break;
        case 'r': // r = special case [->+<] (*(ptr - 1) += *ptr; *ptr = 0
          break;
        case 'w':
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 'x':
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 's': // s = special case [->n+<n] (*(ptr + n) += *ptr; *ptr = 0
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 't': // t = special case [->n+<n] (*(ptr - n) += *ptr; *ptr = 0
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 'u': // u = special case [->n+<n] (*(ptr + n) -= *ptr; *ptr = 0
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 'v': // v = special case [->n+<n] (*(ptr - n) -= *ptr; *ptr = 0
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 'b': // > sequence special case
        {
          putchar('?');putchar('?');putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 4;
        } break;
        case 'c': // < sequence special case
        {
          putchar('?');putchar('?');putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 4;
        } break;
        case 'd': // + sequence special case
        {
          putchar('?');putchar('?');putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 4;
        } break;
        case 'e': // - sequence special case
        {
          putchar('?');putchar('?');putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 4;
        } break;
        case 'f': // > sequence special case
        {
          putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 2;
        } break;
        case 'g': // < sequence special case
        {
          putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 2;
        } break;
        case 'h': // + sequence special case
        {
          putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 2;
        } break;
        case 'i': // - sequence special case
        {
          putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 2;
        } break;
        case 'j': // > sequence special case
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 'k': // < sequence special case
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 'l': // + sequence special case
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case 'm': // - sequence special case
        {
          putchar('?');fflush(stdout);
          ++instruction_pointer;
        } break;
        case '>':
          break;
        case '<':
          break;
        case '+':
          break;
        case '-':
          break;
        case '.':
          break;
        case ',':
          // don't handle yet.
          break;
        case '[':
          putchar('?');putchar('?');putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 4;
          break;
        case ']':
          putchar('?');putchar('?');putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 4;
          break;
        case 'n':
          putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 2;
          break;
        case 'o':
          putchar('?');fflush(stdout);
          ++instruction_pointer;
          break;
        case 'p':
          putchar('?');putchar('?');fflush(stdout);
          instruction_pointer += 2;
          break;
        case 0:
          break;
        default:
          _unreachable;
      }
      ++instruction_pointer;
    }
#endif
  }
  fflush(stdout);
}
