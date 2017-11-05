#include "Header.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>

std::vector<uint8_t> recode(const char * __restrict code, const uint32_t code_len)
{
   std::vector<uint8_t> recoded;
   recoded.reserve(code_len);

   // We recode the Brainfuck so that it's faster to execute.
   // We will handle a few optimizations here. Mainly detecting sequences.

   static constexpr uint32_t seq_min = 3;

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
      std::vector<uint32_t> loops;
      uint32_t instruction_pointer = 0;
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
               uint32_t ii = instruction_pointer + 1;
               uint32_t seq_cnt = 1;
               for (; ii < code_len && code[ii] == '>'; ++ii)
               {
                  ++seq_cnt;
               }
               if (seq_cnt >= seq_min)
               {
                  recoded.pop_back();

                  if (seq_cnt <= uint8_t(-1))
                  {
                     recoded.push_back('j');

                     recoded.push_back(seq_cnt);
                  }
                  else if (seq_cnt <= uint16_t(-1))
                  {
                     recoded.push_back('f');

                     union
                     {
                        uint16_t as_uint;
                        uint8_t as_bytes[2];
                     } ub;
                     ub.as_uint = seq_cnt;

                     recoded.push_back(ub.as_bytes[0]);
                     recoded.push_back(ub.as_bytes[1]);
                  }
                  else
                  {
                     recoded.push_back('b');

                     union
                     {
                        uint32_t as_uint;
                        uint8_t as_bytes[4];
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
            case '<':
            {
               // See if there's a sequence of them after it.
               uint32_t ii = instruction_pointer + 1;
               uint32_t seq_cnt = 1;
               for (; ii < code_len && code[ii] == '<'; ++ii)
               {
                  ++seq_cnt;
               }
               if (seq_cnt >= seq_min)
               {
                  recoded.pop_back();

                  if (seq_cnt <= uint8_t(-1))
                  {
                     recoded.push_back('k');

                     recoded.push_back(seq_cnt);
                  }
                  else if (seq_cnt <= uint16_t(-1))
                  {
                     recoded.push_back('g');

                     union
                     {
                        uint16_t as_uint;
                        uint8_t as_bytes[2];
                     } ub;
                     ub.as_uint = seq_cnt;

                     recoded.push_back(ub.as_bytes[0]);
                     recoded.push_back(ub.as_bytes[1]);
                  }
                  else
                  {
                     recoded.push_back('c');

                     union
                     {
                        uint32_t as_uint;
                        uint8_t as_bytes[4];
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
               uint32_t ii = instruction_pointer + 1;
               uint32_t seq_cnt = 1;
               for (; ii < code_len && code[ii] == '+'; ++ii)
               {
                  ++seq_cnt;
               }
               if (seq_cnt >= seq_min)
               {
                  recoded.pop_back();

                  if (seq_cnt <= uint8_t(-1))
                  {
                     recoded.push_back('l');

                     recoded.push_back(seq_cnt);
                  }
                  else if (seq_cnt <= uint16_t(-1))
                  {
                     recoded.push_back('h');

                     union
                     {
                        uint16_t as_uint;
                        uint8_t as_bytes[2];
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
                        uint32_t as_uint;
                        uint8_t as_bytes[4];
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
               uint32_t ii = instruction_pointer + 1;
               uint32_t seq_cnt = 1;
               for (; ii < code_len && code[ii] == '-'; ++ii)
               {
                  ++seq_cnt;
               }
               if (seq_cnt >= seq_min)
               {
                  recoded.pop_back();

                  if (seq_cnt <= uint8_t(-1))
                  {
                     recoded.push_back('m');

                     recoded.push_back(seq_cnt);
                  }
                  else if (seq_cnt <= uint16_t(-1))
                  {
                     recoded.push_back('i');

                     union
                     {
                        uint16_t as_uint;
                        uint8_t as_bytes[2];
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
                        uint32_t as_uint;
                        uint8_t as_bytes[4];
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
                  uint32_t ii = std::min(code_len, instruction_pointer + 1);
                  uint32_t seq_len = 0;
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
                  uint32_t ii = std::min(code_len, instruction_pointer + 1);
                  uint32_t seq_len = 0;
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
                  uint32_t ii = std::min(code_len, instruction_pointer + 1);
                  if (code[ii] == '-')
                  {
                     ii = std::min(code_len, ii + 1);
                     uint32_t seq_len = 0;
                     while (code[ii] == '>')
                     {
                        ii = std::min(code_len, ii + 1);
                        ++seq_len;
                     }
                     if (seq_len && seq_len <= 0xFF && code[ii] == '+') // TODO if we want 2- and 4-b sequences, change this.
                     {
                        ii = std::min(code_len, ii + 1);
                        uint32_t seq_len2 = 0;
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
                  uint32_t ii = std::min(code_len, instruction_pointer + 1);
                  if (code[ii] == '-')
                  {
                     ii = std::min(code_len, ii + 1);
                     uint32_t seq_len = 0;
                     while (code[ii] == '<')
                     {
                        ii = std::min(code_len, ii + 1);
                        ++seq_len;
                     }
                     if (seq_len && seq_len <= 0xFF && code[ii] == '+') // TODO if we want 2- and 4-b sequences, change this.
                     {
                        ii = std::min(code_len, ii + 1);
                        uint32_t seq_len2 = 0;
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
                  uint32_t ii = std::min(code_len, instruction_pointer + 1);
                  if (code[ii] == '-')
                  {
                     ii = std::min(code_len, ii + 1);
                     uint32_t seq_len = 0;
                     while (code[ii] == '>')
                     {
                        ii = std::min(code_len, ii + 1);
                        ++seq_len;
                     }
                     if (seq_len && seq_len <= 0xFF && code[ii] == '-') // TODO if we want 2- and 4-b sequences, change this.
                     {
                        ii = std::min(code_len, ii + 1);
                        uint32_t seq_len2 = 0;
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
                  uint32_t ii = std::min(code_len, instruction_pointer + 1);
                  if (code[ii] == '-')
                  {
                     ii = std::min(code_len, ii + 1);
                     uint32_t seq_len = 0;
                     while (code[ii] == '<')
                     {
                        ii = std::min(code_len, ii + 1);
                        ++seq_len;
                     }
                     if (seq_len && seq_len <= 0xFF && code[ii] == '-') // TODO if we want 2- and 4-b sequences, change this.
                     {
                        ii = std::min(code_len, ii + 1);
                        uint32_t seq_len2 = 0;
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
                  uint32_t as_uint;
                  uint8_t as_bytes[4];
               } ub;
               ub.as_uint = loops.back();
               loops.pop_back();

               // write this offset back in.
               uint32_t * __restrict return_loc = (uint32_t * __restrict)(recoded.data() + ub.as_uint + 1);

               if (code_len <= 0xFFFF)
               {
                  ub.as_uint = (recoded.size() - 1) - (ub.as_uint + 3);
               }
               else
               {
                  ub.as_uint = (recoded.size() - 1) - (ub.as_uint + 5);
               }

               uint32_t return_loc_val;

               if (ub.as_uint <= uint8_t(-1))
               {
                  return_loc_val = recoded.size() + 1;
                  recoded[recoded.size() - 1] = 'o';
                  recoded.push_back(ub.as_bytes[0]);
               }
               else if (ub.as_uint <= uint16_t(-1))
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
                  *(uint16_t * __restrict)return_loc = return_loc_val;
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
}
