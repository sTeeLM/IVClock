#ifndef __IVCLOCK_REMOTE_PROTOCOL_H__
#define __IVCLOCK_REMOTE_PROTOCOL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REMOTE_CONTROL_MSG_HEADER_MAGIC 0x12345678

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

#pragma pack(push,1)
typedef struct remote_control_msg_header
{
  uint32_t magic;
  uint16_t length; // body length
  union{
    remote_control_cmd_type_t cmd; // command
    remote_control_res_type_t res; // response  
  };
  remote_control_code_type_t code;  // ret code
}remote_control_msg_header_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct remote_control_body_time
{
  uint8_t year;   // 0 - 99 (2000 ~ 2099)
  uint8_t mon;    // 1 - 12
  uint8_t date;   // 1 - 31(30/29/28)
  uint8_t hour;   // 0 - 23
  uint8_t min;    // 0 - 59
  uint8_t sec;    // 0 - 59
}remote_control_body_time_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct remote_control_body_alarm
{
  uint8_t alarm_index; // 0 - 9
  uint8_t day_mask;    //
  uint8_t hour;        // 0 - 23
  uint8_t min;         // 0 - 59
  uint8_t snd;         // 0 - 9
}remote_control_body_alarm_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct remote_control_body_param
{
  uint8_t time_12;
  uint8_t acc_th;
  uint8_t bp_en;
  uint8_t alm1_en;
  uint8_t alm1_begin;
  uint8_t alm1_end;
  uint8_t mon_lt_en;
  uint8_t temp_cen;
  uint8_t power_timeo; 
  uint8_t tmr_snd;
  uint8_t ply_vol;  
}remote_control_body_param_t;
#pragma pack(pop)

// msg
#pragma pack(push,1)
typedef struct remote_control_msg
{
  remote_control_msg_header_t header;
  union {
    remote_control_body_time_t time;
    remote_control_body_alarm_t alarm; 
    remote_control_body_param_t param;
    double bat_voltage;
  }body;
}remote_control_msg_t;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif
