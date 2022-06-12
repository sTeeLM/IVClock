#ifndef __IVCLOCK_ADC_H__
#define __IVCLOCK_ADC_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_ADC1_Init(void);
uint16_t BSP_ADC1_Get_Value(void);
BSP_Error_Type BSP_ADC1_Set_WDG(uint16_t LowThreshold, uint16_t HiThreshold);
BSP_Error_Type BSP_ADC1_Start(void);
BSP_Error_Type BSP_ADC1_Stop(void);
uint16_t BSP_ADC1_Get_Value(void);

BSP_Error_Type BSP_ADC2_Init(void);
uint16_t BSP_ADC2_Get_Value(void);
BSP_Error_Type BSP_ADC2_Start(void);
BSP_Error_Type BSP_ADC2_Stop(void);
#endif
