#ifndef _H_LINUXTOOLS_STUFF
#define _H_LINUXTOOLS_STUFF

// standard stuff that would be missed if not there

#ifndef NULL
#  define NULL ((void*) 0)
#endif

#ifndef FALSE
#  define FALSE  0
#endif

#ifndef TRUE
#  define TRUE   (!0)
#endif

#ifndef ARRLEN
#define ARRLEN(ARR) (sizeof(ARR) / sizeof((ARR)[0]))
#endif

#define ssizeof(TYPE) ((int) sizeof(TYPE))


#endif  // _H_LINUXTOOLS_MISSING
