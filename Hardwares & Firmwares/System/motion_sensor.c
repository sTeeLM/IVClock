#include "motion_sensor.h"
#include "acc.h"
#include "task.h"
#include "config.h"
#include "console.h"

void motion_sensor_show(void)
{
  console_printf("motion sensoris %s, th is %d\r\n", 
  motion_sensor_is_on() ? "on" : "off", motion_sensor_get_th());
}

bool motion_sensor_is_on(void)
{
  return BSP_ACC_Is_Power_On();
}

void motion_sensor_init(void)
{
	if(config_read("acc_on")->val8)
		motion_sensor_on();
}

void motion_sensor_on(void)
{
  BSP_ACC_Power_On();
}

void motion_sensor_off(void)
{
  BSP_ACC_Power_Off();
}

void motion_sensor_isr(void)
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