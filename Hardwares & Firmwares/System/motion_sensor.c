#include "motion_sensor.h"
#include "acc.h"
#include "task.h"
#include "config.h"
#include "console.h"
#include "power.h"

void motion_sensor_show(void)
{
  console_printf("motion sensoris %s, th is %d\r\n", 
  motion_sensor_enabled() ? "en" : "dis", motion_sensor_get_th());
}

bool motion_sensor_enabled(void)
{
  return BSP_ACC_Is_Power_On();
}

void motion_sensor_init(void)
{
  motion_sensor_enable(config_read("acc_en")->val8);
}

void motion_sensor_enable(bool enable)
{
  config_val_t val;
  val.val8 = enable;
  enable ? BSP_ACC_Power_On() : BSP_ACC_Power_Off();
  config_write("acc_en", &val);
}

void motion_sensor_scan(void)
{
  if(BSP_Read_Int_Status())
    task_set(EV_ACC);
}

uint8_t motion_sensor_get_th(void)
{
  return BSP_ACC_Threshold_Get();
}

void motion_sensor_set_th(uint8_t th)
{
  config_val_t val;
  BSP_ACC_Threshold_Set(th);
  val.val8 = th;
  config_write("acc_th", &val);
}