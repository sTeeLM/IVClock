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
void debug_printf(uint8_t newline, ivlog_type_t level, const char * fmt, ...);

#define IV_DEBUG_LEVEL 4

#if (IV_DEBUG_LEVEL >= 1)
#define IVERR(...)     debug_printf(0, IVLOG_ERROR, __VA_ARGS__)
#define IVERR_R(...)   debug_printf(1, IVLOG_ERROR, __VA_ARGS__)
#define IVERR_RH       debug_printf(1, IVLOG_ERROR, "%s","[ERR ]")
#define IVERR_RT       debug_printf(1, IVLOG_ERROR, "%s", "\r\n")
#else
#define IVERR(...)     do{}while(0)
#define IVERR_R(...)   do{}while(0)
#define IVERR_RH
#define IVERR_RT
#endif

#if (IV_DEBUG_LEVEL >= 2)
#define IVINFO(...)    debug_printf(0, IVLOG_INFO, __VA_ARGS__) 
#define IVINFO_R(...)  debug_printf(1, IVLOG_INFO, __VA_ARGS__) 
#define IVINFO_RH      debug_printf(1, IVLOG_INFO, "%s", "[INFO]")
#define IVINFO_RT      debug_printf(1, IVLOG_INFO, "%s", "\r\n")
#else
#define IVINFO(...)  do{}while(0)
#define IVINFO_R(...)  do{}while(0)
#define IVINFO_RH
#endif

#if (IV_DEBUG_LEVEL >= 3)
#define IVDBG(...)     debug_printf(0, IVLOG_DBG, __VA_ARGS__)
#define IVDBG_R(...)   debug_printf(1, IVLOG_DBG, __VA_ARGS__) 
#define IVDBG_RH       debug_printf(1, IVLOG_DBG, "[DBG ]")
#define IVDBG_RT       debug_printf(1, IVLOG_DBG, "\r\n")
#else
#define IVDBG(...)   do{}while(0)
#define IVDBG_R(...) do{}while(0)
#define IVDBG_RH
#define IVDBG_RT
#endif

#endif

