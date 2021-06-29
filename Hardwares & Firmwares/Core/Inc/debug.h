#ifndef __IVCLOCK_DEBUG_H__
#define __IVCLOCK_DEBUG_H__

#include <stdio.h>
#include <stdint.h>

//#include <EventRecorder.h>

typedef enum {
  IVLOG_ERROR,
  IVLOG_INFO,
  IVLOG_DBG
} ivlog_type_t;

int8_t debug_init(void);
void debug_printf(ivlog_type_t level, const char * fmt, ...);

#define IV_DEBUG_LEVEL 4

#if (IV_DEBUG_LEVEL >= 1)
#define IVERR(...)   debug_printf(IVLOG_ERROR, __VA_ARGS__);
#else
#define IVERR(...)   do{}while(0)
#endif

#if (IV_DEBUG_LEVEL >= 2)
#define IVINFO(...)  debug_printf(IVLOG_INFO, __VA_ARGS__); 
#else
#define IVINFO(...)  do{}while(0)
#endif

#if (IV_DEBUG_LEVEL >= 3)
#define IVDBG(...)   debug_printf(IVLOG_DBG, __VA_ARGS__); 
#else
#define IVDBG(...)   do{}while(0)
#endif

#endif

