#include "remote_control.h"
#include "debug.h"
#include "bsp.h"
#include "cext.h"
#include "delay.h"
#include "power.h"
#include "config.h"
#include "motion_sensor.h"
#include "beeper.h"
#include "display.h"
#include "power.h"
#include "usart.h"
#include "timer.h"
#include "player.h"
#include "thermometer.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

void remote_control_init(void)
{
  
}

static bool remote_control_connected(void)
{
  return HAL_GPIO_ReadPin(INT_BT_GPIO_Port, INT_BT_Pin) == RESET;
}

void remote_control_scan(void)
{
  if(remote_control_connected()) {
    IVDBG("remote_control connected");
  } else {
    IVDBG("remote_control disconnected");
  }
}

static remote_control_msg_t remote_control_cmd;
static remote_control_msg_t remote_control_res;

typedef void 
    (*remote_control_cmd_proc_t)(remote_control_msg_t * cmd, remote_control_msg_t * res);

typedef struct remote_control_proc_type
{
  remote_control_cmd_type_t cmd;
  remote_control_cmd_proc_t  proc;
}remote_control_proc_type_t;

static void do_ping(remote_control_msg_t * cmd, remote_control_msg_t * res);
static void do_get_time(remote_control_msg_t * cmd, remote_control_msg_t * res);
static void do_set_time(remote_control_msg_t * cmd, remote_control_msg_t * res);
static void do_get_alarm(remote_control_msg_t * cmd, remote_control_msg_t * res);
static void do_set_alarm(remote_control_msg_t * cmd, remote_control_msg_t * res);
static void do_get_param(remote_control_msg_t * cmd, remote_control_msg_t * res);
static void do_set_param(remote_control_msg_t * cmd, remote_control_msg_t * res);
static void do_get_bat(remote_control_msg_t * cmd, remote_control_msg_t * res);
static void do_stop_alarm(remote_control_msg_t * cmd, remote_control_msg_t * res);

remote_control_proc_type_t remote_control_proc[] = 
{
  {REMOTE_CONTROL_CMD_PING, do_ping},  
  {REMOTE_CONTROL_CMD_GET_TIME, do_get_time},
  {REMOTE_CONTROL_CMD_SET_TIME, do_set_time}, 
  {REMOTE_CONTROL_CMD_GET_ALARM, do_get_alarm}, 
  {REMOTE_CONTROL_CMD_SET_ALARM, do_set_alarm}, 
  {REMOTE_CONTROL_CMD_GET_PARAM, do_get_param}, 
  {REMOTE_CONTROL_CMD_SET_PARAM, do_set_param},
  {REMOTE_CONTROL_CMD_STOP_ALARM, do_stop_alarm},   
  {REMOTE_CONTROL_CMD_GET_BAT, do_get_bat}, 
};

static bool remote_control_check_header(remote_control_msg_header_t * header)
{
  if(header->magic != REMOTE_CONTROL_MSG_HEADER_MAGIC) {
    IVERR("remote_control_check_header: magic %d mismatch!", header->magic);
    return FALSE;
  }
  if(header->length > sizeof(remote_control_msg_t) 
    - sizeof(remote_control_msg_header_t)) {
      IVERR("remote_control_check_header: length too long!", header->length);
      return FALSE;
      }
  return TRUE;
}

static void remote_control_dump_msg(remote_control_msg_t * cmd)
{
  uint8_t i;
  uint8_t *p;
  IVDBG("msg header.magic  = %d",cmd->header.magic); 
  IVDBG("msg header.length = %d",cmd->header.length);   
  IVDBG("msg header.cmd    = %d",cmd->header.cmd);
  IVDBG("msg header.res    = %d",cmd->header.res);   
  IVDBG("msg header.code   = %d",cmd->header.code);
  IVDBG("msg header.body:");
  p = (uint8_t *)&cmd->body;
  for(i = 0; i < cmd->header.length / 8 ; i ++) {
    IVDBG(" %02x %02x %02x %02x %02x %02x %02x %02x", 
    p[i+0], p[i+1], p[i+2], p[i+3],
    p[i+4], p[i+5], p[i+6], p[i+7]);
    p += 8;
  }
  IVDBG_RH;
  for(i = 0 ; i < cmd->header.length % 8 ; i ++) {
    IVDBG_R(" %02x", p[i]);
  }
  IVDBG_RT;
}

static bool remote_control_read_cmd(remote_control_msg_t * cmd)
{
  uint8_t * p = (uint8_t *)cmd;
  BSP_Error_Type ret = BSP_ERROR_INTERNAL;
  if(BSP_USART3_Receive((uint8_t *)p, sizeof(cmd->header)) == BSP_ERROR_NONE) {
    IVDBG("receive cmd: magic = %x, len = %d", cmd->header.magic, cmd->header.length);
    if(remote_control_check_header(&cmd->header)) {
      if(cmd->header.length) {
        ret = BSP_USART3_Receive((uint8_t *)(p + sizeof(cmd->header)), cmd->header.length);
        if(ret == BSP_ERROR_NONE) {
          remote_control_dump_msg(cmd);
          ret = BSP_ERROR_NONE;
        } else {
          IVERR("remote_control_read_cmd: body read error %d", ret);
        }
      } else {
        ret = BSP_ERROR_NONE;
        remote_control_dump_msg(cmd);
      }
    }
  }
  return ret == BSP_ERROR_NONE;
}

static void remote_control_send_res(remote_control_msg_t * res)
{
  res->header.magic = REMOTE_CONTROL_MSG_HEADER_MAGIC;
  IVDBG("send res: magic = %x, len = %d", res->header.magic, res->header.length);
  remote_control_dump_msg(res);
  BSP_USART3_Transmit((uint8_t *)res, res->header.length + sizeof(res->header));
}

static void remote_control_deal_cmd(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  uint8_t i;
  if(cmd->header.cmd > REMOTE_CONTROL_CMD_BASE && cmd->header.cmd < REMOTE_CONTROL_CMD_CNT) {
    for(i = 0 ; i < sizeof(remote_control_proc) / sizeof(remote_control_proc_type_t) ; i ++) {
      if(remote_control_proc[i].cmd == cmd->header.cmd) {
        remote_control_proc[i].proc(cmd, res);
        return;
      }
    }
  }
  // 错误的消息，没有找到的消息
  res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
  // no body
  res->header.code = REMOTE_CONTROL_CODE_ERROR;
  res->header.length = 0;
}

void remote_control_run(void)
{
  while(remote_control_connected()) {
    IVDBG("into remote_control_run");
    if(remote_control_read_cmd(&remote_control_cmd)) {
      remote_control_deal_cmd(&remote_control_cmd, &remote_control_res);
      remote_control_send_res(&remote_control_res);
    }
  }
}

static void do_ping(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
  res->header.code = REMOTE_CONTROL_CODE_OK;
  res->header.length = 0;
}

static void do_get_time(remote_control_msg_t * cmd, remote_control_msg_t * res)
{

  res->body.time.year = clock_get_year();
  res->body.time.mon = clock_get_month();
  res->body.time.date = clock_get_date(); 
  res->body.time.hour = clock_get_hour(); 
  res->body.time.min = clock_get_min();
  res->body.time.sec = clock_get_sec();  
  
  res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
  res->header.code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->body.time);
}

static void do_set_time(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  clock_enable_interrupt(FALSE);

  clock_set_year(cmd->body.time.year);
  clock_set_month(cmd->body.time.mon);
  clock_set_date(cmd->body.time.date); 
  clock_set_hour(cmd->body.time.hour); 
  clock_set_min(cmd->body.time.min);
  clock_set_sec(cmd->body.time.sec); 
  
  clock_sync_to_rtc(CLOCK_SYNC_TIME);
  clock_sync_to_rtc(CLOCK_SYNC_DATE);
  clock_enable_interrupt(TRUE);
  alarm_resync_rtc();
  res->header.code = REMOTE_CONTROL_CODE_OK;
  res->header.length = 0;  
}

static void do_get_alarm(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  if(cmd->body.alarm.alarm_index == (uint8_t)(-1)) {
    res->body.alarm.alarm_index = ALARM0_CNT;
    res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
    res->header.code = REMOTE_CONTROL_CODE_OK;
    res->header.length = sizeof(res->body.alarm);
    
  } else if(cmd->body.alarm.alarm_index < ALARM0_CNT) {
    res->body.alarm.alarm_index = cmd->body.alarm.alarm_index;
    res->body.alarm.day_mask = alarm0_get_day_mask(cmd->body.alarm.alarm_index);
    res->body.alarm.hour = alarm0_get_hour(cmd->body.alarm.alarm_index);   
    res->body.alarm.min = alarm0_get_min(cmd->body.alarm.alarm_index);
    res->body.alarm.snd = alarm0_get_snd(cmd->body.alarm.alarm_index);
  
    res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
    res->header.code = REMOTE_CONTROL_CODE_OK;
    res->header.length = sizeof(res->body.alarm);
  } else {
    res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
    res->header.code = REMOTE_CONTROL_CODE_ERROR;
    res->header.length = 0;
  }
}

static void do_set_alarm(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  uint8_t i;
  if(cmd->body.alarm.alarm_index < ALARM0_CNT) {
    alarm0_set_day_mask(cmd->body.alarm.alarm_index, cmd->body.alarm.day_mask);
    alarm0_set_hour(cmd->body.alarm.alarm_index, cmd->body.alarm.hour);
    alarm0_set_min(cmd->body.alarm.alarm_index, cmd->body.alarm.min); 
    alarm0_set_snd(cmd->body.alarm.alarm_index, cmd->body.alarm.snd);     
    alarm_save_config(ALARM_SYNC_ALARM0, cmd->body.alarm.alarm_index);
    alarm_resync_rtc();
    res->header.code = REMOTE_CONTROL_CODE_OK; 
  } else {
    res->header.code = REMOTE_CONTROL_CODE_ERROR;
  }
  res->header.length = 0;
  res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
}

static void do_get_param(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  // config
  res->body.param.acc_en = motion_sensor_test_enable();
  res->body.param.alm1_en = alarm1_test_enable();
  res->body.param.alm1_begin = alarm1_get_begin();
  res->body.param.alm1_end = alarm1_get_end();  
  res->body.param.bp_en = beeper_test_enable(); 
  res->body.param.mon_lt_en = display_mon_light_test_enable();  
  res->body.param.power_timeo = power_get_timeo(); 
  res->body.param.temp_cen = thermometer_get_unit();
  res->body.param.time_12 = clock_test_hour12(); 
  res->body.param.tmr_snd = timer_get_snd(); 
  res->body.param.ply_vol = player_get_vol();
  
  // other const can not set
  res->body.param.min_ply_vol = player_get_min_vol();  
  res->body.param.max_ply_vol = player_get_max_vol();  
  res->body.param.min_power_timeo = POWER_MIN_TIMEO;  
  res->body.param.max_power_timeo = POWER_MAX_TIMEO; 
  res->body.param.step_power_timeo = POWER_STEP_TIMEO;
  res->body.param.tmr_snd_cnt = PLAYER_SND_CNT;
  
  res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
  res->header.code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->body.param);
}

static void do_set_param(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  config_val_t val;
  
  val.val8 = cmd->body.param.acc_en;
  if(val.val8 != motion_sensor_test_enable()) {
    motion_sensor_set_enable(val.val8);
    motion_sensor_save_config();
  }
  
  val.val8 = cmd->body.param.alm1_en;
  if(val.val8 != alarm1_test_enable()) {
    alarm1_set_enable(val.val8);
    alarm_save_config(ALARM_SYNC_ALARM1, 0);
  }  
  
  val.val8 = cmd->body.param.alm1_begin;
  if(val.val8 != alarm1_get_begin()) {
    alarm1_set_begin(val.val8);
    alarm_save_config(ALARM_SYNC_ALARM1, 0);
  }    

  val.val8 = cmd->body.param.alm1_end;
  if(val.val8 != alarm1_get_end()) {
    alarm1_set_begin(val.val8);
    alarm_save_config(ALARM_SYNC_ALARM1, 0);
  }
  
  val.val8 = cmd->body.param.bp_en;
  if(val.val8 != beeper_test_enable()) {
    beeper_set_enable(val.val8);
    beeper_save_config();
  }  

  val.val8 = cmd->body.param.mon_lt_en;
  if(val.val8 != display_mon_light_test_enable()) {
    display_mon_light_set_enable(val.val8);
    display_mon_light_save_config();
  }  
  
  val.val8 = cmd->body.param.power_timeo;
  if(val.val8 != power_get_timeo()) {
    power_set_timeo(val.val8);
    power_timeo_save_config();
  }   

  val.val8 = cmd->body.param.temp_cen;
  if(val.val8 != thermometer_get_unit()) {
    thermometer_set_unit(val.val8);
    thermometer_save_config();
  }  

  val.val8 = cmd->body.param.time_12;
  if(val.val8 != clock_test_hour12()) {
    clock_set_hour12(val.val8);
    clock_save_config();
  } 
  
  val.val8 = cmd->body.param.tmr_snd;
  if(val.val8 != timer_get_snd()) {
    timer_set_snd(val.val8);
    timer_save_config();
  }   

  val.val8 = cmd->body.param.ply_vol;
  if(val.val8 != player_get_vol()) {
    player_set_vol(val.val8);
    player_save_config();
  }   
  
  res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
  res->header.code = REMOTE_CONTROL_CODE_OK;
  res->header.length = 0;  
}

static void do_get_bat(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  res->body.bat_voltage = power_get_bat_voltage();
  
  res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
  res->header.code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->body.bat_voltage);
}

static void do_stop_alarm(remote_control_msg_t * cmd, remote_control_msg_t * res)
{
  alarm0_stop_snd();
  
  res->header.res = cmd->header.cmd + REMOTE_CONTROL_RES_BASE;
  res->header.code = REMOTE_CONTROL_CODE_OK;
  res->header.length = 0;
}

