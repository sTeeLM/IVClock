#ifndef __IVCLOCK_I2C_H__
#define __IVCLOCK_I2C_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_I2C_Init(void);
void BSP_I2C_DeInit(void);
BSP_Error_Type BSP_I2C_Write(uint16_t DevAddress, 
  uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_I2C_Read(uint16_t DevAddress, 
  uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
#endif
