#ifndef __IVCLOCK_TIM_H__
#define __IVCLOCK_TIM_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_TIM1_Init(void);
BSP_Error_Type BSP_TIM2_Init(void);
BSP_Error_Type BSP_TIM3_Init(void);
BSP_Error_Type BSP_TIM4_Init(void);
BSP_Error_Type BSP_TIM1_Start(uint8_t Dur, uint8_t Loop);
BSP_Error_Type BSP_TIM1_Stop(void);
BSP_Error_Type BSP_TIM3_Start(void);
BSP_Error_Type BSP_TIM3_Stop(void);
BSP_Error_Type BSP_TIM4_Start(void);
BSP_Error_Type BSP_TIM4_Stop(void);
BSP_Error_Type BSP_TIM3_Set_Duty_Cycle(uint8_t dc);

#endif
