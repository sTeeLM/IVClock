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

#include <stdlib.h>
#include <string.h>

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

static remote_control_cmd_t remote_control_cmd;
static remote_control_res_t remote_control_res;

typedef void 
    (*remote_control_cmd_proc_t)(remote_control_cmd_t * cmd, remote_control_res_t * res);

typedef struct remote_control_proc_type
{
  remote_control_cmd_type_t cmd;
  remote_control_cmd_proc_t  proc;
}remote_control_proc_type_t;

static void do_get_time(remote_control_cmd_t * cmd, remote_control_res_t * res);
static void do_set_time(remote_control_cmd_t * cmd, remote_control_res_t * res);
static void do_get_alarm(remote_control_cmd_t * cmd, remote_control_res_t * res);
static void do_set_alarm(remote_control_cmd_t * cmd, remote_control_res_t * res);
static void do_get_param(remote_control_cmd_t * cmd, remote_control_res_t * res);
static void do_set_param(remote_control_cmd_t * cmd, remote_control_res_t * res);
static void do_get_bat(remote_control_cmd_t * cmd, remote_control_res_t * res);
static void do_stop_alarm(remote_control_cmd_t * cmd, remote_control_res_t * res);

remote_control_proc_type_t remote_control_proc[] = 
{
  {REMOTE_CONTROL_CMD_GET_TIME, do_get_time},
  {REMOTE_CONTROL_CMD_GET_TIME, do_set_time}, 
  {REMOTE_CONTROL_CMD_GET_ALARM, do_get_alarm}, 
  {REMOTE_CONTROL_CMD_SET_ALARM, do_set_alarm}, 
  {REMOTE_CONTROL_CMD_GET_PARAM, do_get_param}, 
  {REMOTE_CONTROL_CMD_SET_PARAM, do_set_param},
  {REMOTE_CONTROL_CMD_GET_BAT, do_get_bat},
  {REMOTE_CONTROL_CMD_STOP_ALARM, do_stop_alarm},  
};

static void remote_control_read_cmd(remote_control_cmd_t * cmd)
{
  
}

static void remote_control_send_res(remote_control_res_t * res)
{
  
}

static void remote_control_deal_cmd(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  uint8_t i;
  if(cmd->cmd > REMOTE_CONTROL_CMD_BASE && cmd->cmd <= REMOTE_CONTROL_CMD_SET_PARAM) {
    for(i = 0 ; i < sizeof(remote_control_proc) / sizeof(remote_control_proc_type_t) ; i ++) {
      if(remote_control_proc[i].cmd == cmd->cmd) {
        remote_control_proc[i].proc(cmd, res);
        return;
      }
    }
  }
  // 错误的消息，没有找到的消息
  res->res = cmd->cmd + REMOTE_CONTROL_RES_BASE;
  // no body
  res->code = REMOTE_CONTROL_CODE_ERROR;
  res->header.length = sizeof(res->code) + sizeof(res->res);
}

void remote_control_run(void)
{
  while(remote_control_connected()) {
    remote_control_read_cmd(&remote_control_cmd);
    remote_control_deal_cmd(&remote_control_cmd, &remote_control_res);
    remote_control_send_res(&remote_control_res);
  }
}

static void do_get_time(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  memcpy(&res->body.time.time, &clk, sizeof(clk));
  
  res->res = cmd->cmd + REMOTE_CONTROL_RES_BASE;
  res->code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->code) + sizeof(res->res) + sizeof(res->body.time);
}

static void do_set_time(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  clock_enable_interrupt(FALSE);
  memcpy(&clk, &cmd->body.time.time, sizeof(clk));
  clock_sync_to_rtc(CLOCK_SYNC_TIME);
  clock_sync_to_rtc(CLOCK_SYNC_DATE);
  clock_enable_interrupt(TRUE);
  alarm_resync_rtc();
  res->code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->code) + sizeof(res->res);  
}

static void do_get_alarm(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  memcpy(&res->body.alarm.alarm0, &alarm0, sizeof(alarm0));
  
  res->res = cmd->cmd + REMOTE_CONTROL_RES_BASE;
  res->code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->code) + sizeof(res->res) + sizeof(res->body.alarm);
}

static void do_set_alarm(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  uint8_t i;
  memcpy(&alarm0, &res->body.alarm.alarm0, sizeof(alarm0));
  for(i = 0 ; i < ALARM0_CNT ; i ++)
    alarm_save_config(ALARM_SYNC_ALARM0, i);
  alarm_resync_rtc();
  
  res->res = cmd->cmd + REMOTE_CONTROL_RES_BASE;
  res->code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->code) + sizeof(res->res);
}

static void do_get_param(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  res->body.param.acc_th = motion_sensor_get_th();
  res->body.param.alm1_en = alarm1_test_enable();  
  res->body.param.bp_en = config_read_int("bp_en"); 
  res->body.param.mon_lt_en = config_read_int("mon_lt_en");  
  res->body.param.power_timeo = config_read_int("power_timeo"); 
  res->body.param.temp_cen = config_read_int("temp_cen");
  res->body.param.time_12 = config_read_int("time_12"); 

  res->res = cmd->cmd + REMOTE_CONTROL_RES_BASE;
  res->code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->code) + sizeof(res->res) + sizeof(res->body.param);
}

static void do_set_param(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  config_val_t val;
  
  val.val8 = cmd->body.param.acc_th;
  if(val.val8 != motion_sensor_get_th()) {
    motion_sensor_set_th(val.val8);
    motion_sensor_save_config();
  }
  
  val.val8 = cmd->body.param.alm1_en;
  if(val.val8 != alarm1_test_enable()) {
    alarm1_set_enable(val.val8);
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
  if(val.val8 != config_read_int("temp_cen")) {
    config_write("temp_cen", &val);
  }  

  val.val8 = cmd->body.param.time_12;
  if(val.val8 != config_read_int("time_12")) {
    config_write("time_12", &val);
  } 

  res->res = cmd->cmd + REMOTE_CONTROL_RES_BASE;
  res->code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->code) + sizeof(res->res);  
}

static void do_get_bat(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  res->body.bat_voltage = power_get_bat_voltage();
  
  res->res = cmd->cmd + REMOTE_CONTROL_RES_BASE;
  res->code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->code) + sizeof(res->res) + sizeof(res->body.bat_voltage);
}

static void do_stop_alarm(remote_control_cmd_t * cmd, remote_control_res_t * res)
{
  alarm0_stop_snd();
  
  res->res = cmd->cmd + REMOTE_CONTROL_RES_BASE;
  res->code = REMOTE_CONTROL_CODE_OK;
  res->header.length = sizeof(res->code) + sizeof(res->res);
}

