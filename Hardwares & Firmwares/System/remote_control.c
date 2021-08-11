#include "remote_control.h"
#include "debug.h"
#include "bsp.h"
#include "cext.h"
#include "delay.h"
#include "power.h"

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

void remote_run(void)
{
  while(remote_control_connected()) {

  }
}