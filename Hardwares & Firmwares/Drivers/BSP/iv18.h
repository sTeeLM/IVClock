#ifndef __IVCLOCK_IV18_H__
#define __IVCLOCK_IV18_H__

#include <stdint.h>
#include "bsp.h"

// 特殊字符
#define BSP_IV18_BLANK   0
#define BSP_IV18_DEGREE  1

#define BSP_IV18_PROGRESS_BASE 2
#define BSP_IV18_PROGRESS0  2
#define BSP_IV18_PROGRESS1  3
#define BSP_IV18_PROGRESS2  4
#define BSP_IV18_PROGRESS3  5
#define BSP_IV18_PROGRESS4  6
#define BSP_IV18_PROGRESS5  7
#define BSP_IV18_PROGRESS6  8
#define BSP_IV18_PROGRESS7  9
#define BSP_IV18_PROGRESS_CNT 8

#define BSP_IV18_SPECIAL_MAX  9

BSP_Error_Type BSP_IV18_Init(void);
void BSP_IV18_Clr(void);
void BSP_IV18_Load_Data(uint8_t index);
void BSP_IV18_Clr_Data(uint8_t index);
void BSP_IV18_Set_Dig(uint8_t index, uint8_t ascii);
void BSP_IV18_Set_DP(uint8_t index);
void BSP_IV18_Clr_DP(uint8_t index);
void BSP_IV18_Set_Blink(uint8_t index);
void BSP_IV18_Clr_Blink(uint8_t index);
void BSP_IV18_Scan(void);
void BSP_IV18_Enter_High_Impedance(void);
void BSP_IV18_Leave_High_Impedance(void);
#endif