#include "motion_sensor.h"
#include "bias.h"
#include "task.h"
#include "config.h"
#include "console.h"
#include "power.h"

#define MONTION_SENSOR_MAX_TH 50

static uint8_t motion_sensor_en;

void motion_sensor_show(void)
{
  console_printf("motion sensoris %s\r\n", 
  motion_sensor_test_enable() ? "OFF" : "ON");
}


void motion_sensor_init(void)
{
  motion_sensor_en = config_read_int("acc_en");
  
}

void motion_sensor_scan(void)
{
  uint8_t val;
  val = BSP_BIAS_Read_Int();
  if(motion_sensor_en) {
    task_set(EV_ACC);
  }
}

void motion_sensor_save_config(void)
{
  config_val_t val;
  val.val8 = motion_sensor_en;
  config_write("acc_en", &val);
}


void motion_sensor_set_enable(bool enable)
{
  motion_sensor_en = enable;
}

bool motion_sensor_test_enable(void)
{
  return motion_sensor_en;
}

