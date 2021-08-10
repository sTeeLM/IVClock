#include "motion_sensor.h"
#include "acc.h"
#include "task.h"
#include "config.h"
#include "console.h"
#include "power.h"

#define MONTION_SENSOR_MAX_TH 50

static uint8_t motion_sensor_th;

void motion_sensor_show(void)
{
  console_printf("motion sensoris %s, th is %d\r\n", 
  motion_sensor_get_th() == 0 ? "OFF" : "ON", motion_sensor_get_th());
}


void motion_sensor_init(void)
{
  motion_sensor_th = config_read("acc_th")->val8;
  if(motion_sensor_th > MONTION_SENSOR_MAX_TH)
    motion_sensor_th = MONTION_SENSOR_MAX_TH;
  if(motion_sensor_th != 0) {
    BSP_ACC_Power_On();
    BSP_ACC_Threshold_Set(motion_sensor_th);
  } else {
    BSP_ACC_Power_Off();
  }
}

void motion_sensor_scan(void)
{
  if(BSP_Read_Int_Status())
    task_set(EV_ACC);
}

uint8_t motion_sensor_inc_th(void)
{
  motion_sensor_th ++;
  if(motion_sensor_th > MONTION_SENSOR_MAX_TH)
    motion_sensor_th = 0;
  
  if(motion_sensor_th) {
    BSP_ACC_Power_On();
    BSP_ACC_Threshold_Set(motion_sensor_th);
  } else {
    BSP_ACC_Power_Off();
  }
  
  return motion_sensor_th;
}

uint8_t motion_sensor_get_th(void)
{
  return motion_sensor_th;
}
