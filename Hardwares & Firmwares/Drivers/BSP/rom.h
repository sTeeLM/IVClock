#ifndef __IVCLOCK_ROM_H__
#define __IVCLOCK_ROM_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_ROM_Init(void);
BSP_Error_Type BSP_ROM_Read(void * pData, uint32_t Size);
BSP_Error_Type BSP_ROM_Write(const void * pData, uint32_t Size);

#endif
