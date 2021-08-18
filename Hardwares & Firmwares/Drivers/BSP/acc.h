#ifndef __IVCLOCK_BLUE_ACC_H__
#define __IVCLOCK_BLUE_ACC_H__

#include <stdint.h>
#include "bsp.h"
#include "cext.h"

BSP_Error_Type BSP_ACC_Init(void);
bool BSP_Read_Int_Status(void);
void BSP_ACC_Threshold_Set(uint8_t th);
uint8_t BSP_ACC_Threshold_Get(void);
void BSP_ACC_Power_On(void);
void BSP_ACC_Power_Off(void);
bool BSP_ACC_Is_Power_On(void);

#endif