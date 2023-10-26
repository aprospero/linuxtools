#ifndef _H_LINUXTOOLS_TOOL_LOGGER
#define _H_LINUXTOOLS_TOOL_LOGGER

#include <stdarg.h>
#include <stddef.h>

#include "../stuff.h"

enum log_level
{
  LL_NONE,       /* always printed */
  LL_CRITICAL,   /* application severed -> exit */
  LL_ERROR,      /* unwanted event but application can handle */
  LL_WARN,       /* unexpected event with potential to lead to errors */
  LL_INFO,       /* remarkable event */
  LL_EVENT,      /* standard event */
  LL_DEBUG,      /* additional output for error tracking */
  LL_DEBUG_MORE, /* even more additional output */
  LL_DEBUG_MAX,  /* all available additional output */

  LL_COUNT
};

enum log_facility
{
  LF_STDOUT = 0x00,
  LF_USER         , /* random user-level messages */
  LF_LOCAL0       , /* reserved for local use */
  LF_LOCAL1       , /* reserved for local use */
  LF_LOCAL2       , /* reserved for local use */
  LF_LOCAL3       , /* reserved for local use */
  LF_LOCAL4       , /* reserved for local use */
  LF_LOCAL5       , /* reserved for local use */
  LF_LOCAL6       , /* reserved for local use */
  LF_LOCAL7       , /* reserved for local use */

  LF_COUNT
};

#ifndef MAX_LOG_LEN
#define MAX_LOG_LEN 256
#endif

#define LG_DBGMX(FORMAT, ...) log_push(LL_DEBUG_MAX, FORMAT, ##__VA_ARGS__)
#define LG_DBGMR(FORMAT, ...) log_push(LL_DEBUG_MORE, FORMAT, ##__VA_ARGS__)
#define LG_DEBUG(FORMAT, ...) log_push(LL_DEBUG, FORMAT, ##__VA_ARGS__)
#define LG_EVENT(FORMAT, ...) log_push(LL_EVENT, FORMAT, ##__VA_ARGS__)
#define LG_INFO(FORMAT, ... ) log_push(LL_INFO, FORMAT, ##__VA_ARGS__)
#define LG_WARN(FORMAT, ...) log_push(LL_WARN, FORMAT, ##__VA_ARGS__)
#define LG_ERROR(FORMAT, ...) log_push(LL_ERROR, FORMAT, ##__VA_ARGS__)
#define LG_CRITICAL(FORMAT, ...) log_push(LL_CRITICAL, FORMAT, ##__VA_ARGS__)

#ifdef __cplusplus
extern "C"
{
#endif

  void log_init(const char * ident, enum log_facility facility, enum log_level ll);
  void log_set_level_state(enum log_level ll, size_t active);
  int  log_get_level_state(enum log_level ll);

  enum log_level log_get_level_no(const char * level);
  enum log_facility log_get_facility(const char * facility);

  const char * log_get_level_name(enum log_level ll, int do_fulltext);
  const char * log_get_facility_name(enum log_facility lf);

  void log_push(const enum log_level ll, const char * format, ...) __attribute__((format(printf, 2, 3)));

#ifdef __cplusplus
}
#endif

#endif  /* _H_LINUXTOOLS_TOOL_LOGGER */
