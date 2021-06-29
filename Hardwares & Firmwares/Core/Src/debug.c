#include <stdio.h>
#include <stdarg.h>
#include "debug.h"
#include "usart.h"

int8_t debug_init(void)
{
  return 0;
}

static char buffer[1024];

void debug_printf(ivlog_type_t level, const char * fmt, ...)
{
  va_list arg_ptr;
  int len;
  
  len = snprintf(buffer, sizeof(buffer) - 1, "%s ",
  level == IVLOG_ERROR ? "[ERR ]" : 
  (level == IVLOG_INFO? "[INFO]" : "[DBG ]"));
  
  va_start (arg_ptr, fmt); /* format string */
  len += vsnprintf (buffer + len, sizeof(buffer) - len - 5, fmt, arg_ptr);
  va_end (arg_ptr);
  
  len += snprintf(buffer + len, sizeof(buffer) - len - 1, "%s", "\r\n");
  
  buffer[sizeof(buffer) - 1] = 0;

  BSP_USART1_Transmit((uint8_t *)buffer, len);
}

int fputc(int ch, FILE* file)
{
	BSP_USART1_Transmit((uint8_t *)&ch, 1);
	return ch;
}
