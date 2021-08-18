#ifndef __IVCLOCK_KEY_H__
#define __IVCLOCK_KEY_H__

#include <stdint.h>
#include "bsp.h"
#include "cext.h"

BSP_Error_Type BSP_Key_Init(void);
bool BSP_Key_Is_Factory_Reset(void);
bool BSP_Key_Set_Pressed(void);
bool BSP_Key_Mod_Pressed(void);
#endif