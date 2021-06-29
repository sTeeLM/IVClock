#ifndef __IVCLOCK_ROM_H__
#define __IVCLOCK_ROM_H__

#include <stdint.h>

int8_t BSP_ROM_Init(void);
int8_t BSP_ROM_Read(void * pData, uint32_t Size);
int8_t BSP_ROM_Write(const void * pData, uint32_t Size);

#endif
