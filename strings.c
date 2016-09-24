#include "system.h"
#include "types.h"
#include "strings.h"
#include "maths.h"


#define is_one_byte_char(c)   (((c) & 0b10000000) == 0b00000000)
#define is_two_byte_char(c)   (((c) & 0b11100000) == 0b11000000)
#define is_three_byte_char(c) (((c) & 0b11110000) == 0b11100000)
#define is_four_byte_char(c)  (((c) & 0b11111000) == 0b11110000)


u32
get_byte_length(char c)
{
  u32 result = 0;

  if (is_one_byte_char(c))
  {
    result = 1;
  }
  else if (is_two_byte_char(c))
  {
    result = 2;
  }
  else if (is_three_byte_char(c))
  {
    result = 3;
  }
  else if (is_four_byte_char(c))
  {
    result = 4;
  }

  return result;
}


String *
alloc_string(u32 length)
{
  String *result = (String *)malloc(sizeof(String) + (length * sizeof(utf8)) - 1);
  result->length = length;
  return result;
}


String *
new_string(const char *c_string)
{
  String *result = 0;

  u32 n = 0;
  u32 string_length = 0;
  while (c_string[n] != '\0')
  {
    if (get_byte_length(c_string[n]) != 0)
    {
      ++string_length;
    }
    ++n;
  }

  result = alloc_string(string_length);

  u32 c_string_pos = 0;
  for (s32 n = 0; n < result->length; ++n)
  {
    const char *c_string_char = c_string + c_string_pos;

    u32 char_bytes = get_byte_length(c_string_char[0]);

    if (char_bytes <= 4)
    {
      for (u32 byte = 0; byte < char_bytes; ++byte)
      {
        result->cs[n].bytes[byte] = c_string_char[byte];
      }
      c_string_pos += char_bytes;
    }
    else
    {
      ++c_string_pos;
      --n;
    }
  }

  return result;
}


char *
to_c_string(String *string)
{
  char *c_string;

  u32 c_string_length = 0;

  for (u32 n = 0; n < string->length; ++n)
  {
    c_string_length += get_byte_length(string->cs[n].bytes[0]);
  }

  c_string = (char *)malloc(c_string_length * sizeof(char));

  u32 c_string_pos = 0;
  for (u32 n = 0; n < string->length; ++n)
  {
    char *c_string_char = c_string + c_string_pos;

    u32 char_bytes = get_byte_length(string->cs[n].bytes[0]);

    if (char_bytes <= 4)
    {
      for (u32 byte = 0; byte < char_bytes; ++byte)
      {
        c_string_char[byte] = string->cs[n].bytes[byte];
      }
      c_string_pos += char_bytes;
    }
  }

  return c_string;
}


void
print(String *string)
{
  File *stdout = get_stdout_fd();
  for (u32 i = 0; i < string->length; ++i)
  {
    utf8 *c = string->cs + i;

    u32 length = get_byte_length(c->bytes[0]);

    fwrite(c->bytes, sizeof(u8), length, stdout);
  }
  fclose(stdout);
}


String *
int_to_string(u32 i)
{
  u32 length = 1;
  u32 i_tmp = i;

  if (i_tmp >= 100000000)
  {
    i_tmp /= 100000000;
    length += 8;
  }
  if (i_tmp >= 10000)
  {
    i_tmp /= 10000;
    length += 4;
  }
  if (i_tmp >= 100)
  {
    i_tmp /= 100;
    length += 2;
  }
  if (i_tmp >= 10)
  {
    i_tmp /= 10;
    length += 1;
  }

  String *result = alloc_string(length);

  for (u32 n = 0; n < result->length; ++n)
  {
    result->cs[n].bytes[0] = '0' + (i / (u32)pow(10, result->length - n - 1)) % 10;
  }

  return result;
}


String *
join(String *a, String *b)
{
  String *result = alloc_string(a->length + b->length);

  for (u32 i = 0; i < a->length; ++i)
  {
    result->cs[i] = a->cs[i];
  }
  for (u32 j = 0; j < b->length; ++j)
  {
    result->cs[a->length + j] = b->cs[j];
  }

  return result;
}


void
append(String **string, String *add)
{
  String *new = join(*string, add);
  free(*string);
  *string = new;
}