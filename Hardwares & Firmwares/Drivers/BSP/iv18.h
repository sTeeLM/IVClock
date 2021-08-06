#ifndef __IVCLOCK_IV18_H__
#define __IVCLOCK_IV18_H__

#include <stdint.h>
#include "bsp.h"

#define BSP_IV18_BLANK   0
#define BSP_IV18_DEGREE  1

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
#endif