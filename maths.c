#include "maths.h"


u32
pow(u32 base, u32 exp)
{
  u32 result = 1;

  for (u32 i = 0; i < exp; ++i)
  {
    result *= base;
  }

  return result;
}