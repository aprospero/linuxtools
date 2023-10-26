#ifndef _H_LINUXTOOLS_TIMEHELP
#define _H_LINUXTOOLS_TIMEHELP

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  const char * getTimeValString(struct timeval tv, const char * format, char * buf, size_t buflen);

#ifdef __cplusplus
}
#endif



#endif  // _H_LINUXTOOLS_TIMEHELP
