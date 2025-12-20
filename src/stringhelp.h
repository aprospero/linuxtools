#ifndef _H_LINUXTOOLS_STRINGHELP
#define _H_LINUXTOOLS_STRINGHELP

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  int stricmp(const char * str1, const char * str2);
  int strincmp(const char * str1, const char * str2, size_t len);

#ifdef __cplusplus
}
#endif

#endif  // _H_LINUXTOOLS_STRINGHELP
