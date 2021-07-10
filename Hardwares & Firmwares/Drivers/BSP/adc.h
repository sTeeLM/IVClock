#ifndef __IVCLOCK_ADC_H__
#define __IVCLOCK_ADC_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_ADC1_Init(void);
BSP_Error_Type BSP_ADC1_Set_WDG(uint32_t HighThreshold, uint32_t LowThreshold);
BSP_Error_Type BSP_ADC1_Start(void);
BSP_Error_Type BSP_ADC1_Stop(void);

BSP_Error_Type BSP_ADC2_Init(void);
BSP_Error_Type BSP_ADC2_Start(void);
BSP_Error_Type BSP_ADC2_Stop(void);

#endif
