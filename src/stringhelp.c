#include <ctype.h>
#include <stdint.h>
#include "stringhelp.h"

int stricmp(const char * str1, const char * str2)
{
  return strincmp(str1, str2, SIZE_MAX);
}

int strincmp(const char * str1, const char * str2, size_t len)
{
  int idx;
  for (;*str1 != '\0' && *str2 != '\0' && len > 1; ++str1, ++str2, --len)
  {
    idx = toupper(*str1) - toupper(*str2);
    if (idx)
      return idx;
  }
  return toupper(*str1) - toupper(*str2);
}

