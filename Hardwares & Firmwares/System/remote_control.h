#ifndef __IVCLOCK_REMOTE_CONTROL_H__
#define __IVCLOCK_REMOTE_CONTROL_H__

#include <stdint.h>

struct remote_control_msg_header
{
  uint32_t magic;
  uint16_t length; // body length
};

enum remote_control_cmd_type
{
  REMOTE_CONTROL_CMD_NONE = 0,
  REMOTE_CONTROL_CMD_SYNC_TIME,
  REMOTE_CONTROL_CMD_READ_ALARM,
  REMOTE_CONTROL_CMD_SET_ALARM,
  REMOTE_CONTROL_CMD_READ_PARAM,
  REMOTE_CONTROL_CMD_SET_PARAM
};


struct remote_control_cmd_sync_time
{
  uint8_t year;   // 0 - 99 (2000 ~ 2099)
  uint8_t mon;    // 0 - 11
  uint8_t date;   // 0 - 30(29/28/27)
  uint8_t day;    // 0 - 6
  uint8_t hour;   // 0 - 23
  uint8_t min;    // 0 - 59
  uint8_t sec;    // 0 - 59 
};

struct remote_control_cmd_set_param
{
  
};

// command
struct remote_control_cmd
{
  struct remote_control_msg_header header;
  uint16_t cmd;
  union {
    struct remote_control_cmd_sync_time sync_time;
  }body;
};

// respond
struct remote_control_res
{
  struct remote_control_msg_header header;
  uint16_t res;
  union {
    struct remote_control_cmd_sync_time sync_time;
  }body;
};


struct remote_control_msg_get_param
{
  uint8_t year;   // 0 - 99 (2000 ~ 2099)
  uint8_t mon;    // 0 - 11
  uint8_t date;   // 0 - 30(29/28/27)
  uint8_t day;    // 0 - 6
  uint8_t hour;   // 0 - 23
  uint8_t min;    // 0 - 59
  uint8_t sec;    // 0 - 59 
};

void remote_control_init(void);
void remote_control_scan(void);
void remote_run(void);

#endif
