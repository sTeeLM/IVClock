#ifndef __IVCLOCK_PWR_H__
#define __IVCLOCK_PWR_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_PWR_Init(void);
BSP_Error_Type BSP_PWR_SystemClock_Config(void);
void BSP_PWR_Sleep(void);
#endif