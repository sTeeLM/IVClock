#ifndef __IVCLOCK_BIAS_H__
#define __IVCLOCK_BIAS_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_BIAS_Init(void);
uint8_t BSP_BIAS_Read_Int(void);

#endif
