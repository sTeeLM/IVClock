#include <stdio.h>
#include <stdarg.h>
#include "debug.h"
#include "usart.h"

int8_t debug_init(void)
{
  return 0;
}

static char buffer[1024];

void debug_printf(uint8_t raw, ivlog_type_t level, const char * fmt, ...)
{
  va_list arg_ptr;
  uint32_t len = 0;
  uint64_t now = ticks_get_now_ms();
  if(!raw) {
    len = snprintf(buffer, sizeof(buffer) - 1, "[%lld]%s ",
      now,
      level == IVLOG_ERROR ? "[ERR ]" : 
      (level == IVLOG_INFO? "[INFO]" : "[DBG ]"));
  }
  
  va_start (arg_ptr, fmt); /* format string */
  len += vsnprintf (buffer + len, sizeof(buffer) - len - 5, fmt, arg_ptr);
  va_end (arg_ptr);
  
  if(!raw)
    len += snprintf(buffer + len, sizeof(buffer) - len - 1, "%s", "\r\n");
  
  buffer[sizeof(buffer) - 1] = 0;

  BSP_USART1_Transmit((uint8_t *)buffer, len);
}

