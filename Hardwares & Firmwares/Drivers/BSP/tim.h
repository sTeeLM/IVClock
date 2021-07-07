#ifndef __IVCLOCK_TIM_H__
#define __IVCLOCK_TIM_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_TIM1_Init(void);
BSP_Error_Type BSP_TIM2_Init(void);
BSP_Error_Type BSP_TIM3_Init(void);
BSP_Error_Type BSP_TIM4_Init(void);
BSP_Error_Type BSP_TIM1_Start(void);
BSP_Error_Type BSP_TIM1_Stop(void);
BSP_Error_Type BSP_TIM3_Start(void);
BSP_Error_Type BSP_TIM3_Stop(void);
BSP_Error_Type BSP_TIM4_Start(void);
BSP_Error_Type BSP_TIM4_Stop(void);

#endif
