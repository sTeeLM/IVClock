#ifndef __IVCLOCK_REMOTE_CONTROL_H__
#define __IVCLOCK_REMOTE_CONTROL_H__

#include <stdint.h>

#include "alarm.h"
#include "clock.h"

typedef struct remote_control_msg_header
{
  uint32_t magic;
  uint16_t length; // body length
}remote_control_msg_header_t;

typedef enum remote_control_cmd_type
{
  REMOTE_CONTROL_CMD_BASE = 0,
  REMOTE_CONTROL_CMD_GET_TIME,
  REMOTE_CONTROL_CMD_SET_TIME,  
  REMOTE_CONTROL_CMD_GET_ALARM,
  REMOTE_CONTROL_CMD_SET_ALARM,
  REMOTE_CONTROL_CMD_GET_PARAM,
  REMOTE_CONTROL_CMD_SET_PARAM,
  REMOTE_CONTROL_CMD_GET_BAT,
  REMOTE_CONTROL_CMD_STOP_ALARM  
}remote_control_cmd_type_t;

typedef enum remote_control_res_type
{
  REMOTE_CONTROL_RES_BASE = 100,
  REMOTE_CONTROL_RES_GET_TIME,
  REMOTE_CONTROL_RES_SET_TIME,  
  REMOTE_CONTROL_RES_GET_ALARM,
  REMOTE_CONTROL_RES_SET_ALARM,
  REMOTE_CONTROL_RES_GET_PARAM,
  REMOTE_CONTROL_RES_SET_PARAM,
  REMOTE_CONTROL_RES_GET_BAT,
  REMOTE_CONTROL_RES_STOP_ALARM 
}remote_control_res_type_t;

typedef enum remote_control_code_type
{
  REMOTE_CONTROL_CODE_OK   = 0,
  REMOTE_CONTROL_CODE_ERROR
}remote_control_code_type_t;

typedef struct remote_control_body_time
{
  struct clock_struct time;
}remote_control_body_time_t;

typedef struct remote_control_body_alarm
{
  struct  alarm0_struct alarm0[ALARM0_CNT];
}remote_control_body_alarm_t;

typedef struct remote_control_body_param
{
  uint8_t time_12;
  uint8_t acc_th;
  uint8_t bp_en;
  uint8_t alm1_en;
  uint8_t mon_lt_en;
  uint8_t temp_cen;
  uint8_t power_timeo; 
}remote_control_body_param_t;

// command
typedef struct remote_control_cmd
{
  remote_control_msg_header_t header;
  remote_control_cmd_type_t cmd;
  remote_control_code_type_t code;
  union {
    remote_control_body_time_t time;
    remote_control_body_alarm_t alarm; 
    remote_control_body_param_t param;    
  }body;
}remote_control_cmd_t;

// respond
typedef struct remote_control_res
{
  remote_control_msg_header_t header;
  remote_control_res_type_t res;
  remote_control_code_type_t code;
  union {
    remote_control_body_time_t time;
    remote_control_body_alarm_t alarm; 
    remote_control_body_param_t param;
    double bat_voltage;
  }body;
}remote_control_res_t;

void remote_control_init(void);
void remote_control_scan(void);
void remote_control_run(void);

#endif
