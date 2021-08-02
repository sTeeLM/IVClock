#include "remote_control.h"
#include "debug.h"
#include "bsp.h"
#include "cext.h"
#include "delay.h"

static bool remote_control_conn;

void remote_control_init(void)
{
  remote_control_conn = FALSE;
}

void remote_control_scan(void)
{
  if(HAL_GPIO_ReadPin(INT_BT_GPIO_Port, INT_BT_Pin) == SET) {
    IVDBG("remote_control_scan: BLE disconnect");
    remote_control_conn = FALSE;
  } else {
    IVDBG("remote_control_scan: BLE connect");
    remote_control_conn = TRUE;
  }
}

void remote_run(void)
{
  while(remote_control_conn) {
    delay_ms(1000);
    IVDBG("remote_run");
  }
}