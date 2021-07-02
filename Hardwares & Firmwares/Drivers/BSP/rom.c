#include "rom.h"
#include "bsp.h"
#include "debug.h"
#include "i2c.h"
#include "delay.h"

#define ROM_ADDR 0xA0

/**
  * @brief ROM Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_ROM_Init(void)
{
	char buf [32] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
	char buf1[32] = {0};
	BSP_Error_Type res = 0;
	int i;
	res = BSP_ROM_Write(buf, sizeof(buf));
	IVDBG("BSP_ROM_Write res %d", res);
	delay_ms(10);
	res = BSP_ROM_Read(buf1, sizeof(buf1));
	IVDBG("BSP_ROM_Read res %d", res);
	for(i  = 0 ; i < 8; i ++)
		IVDBG("%d", buf1[i]);
  return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_ROM_Read(void * pData, uint32_t Size)
{
	return BSP_I2C_Read(ROM_ADDR, 0, I2C_MEMADD_SIZE_16BIT, (uint8_t*)pData, 32);
}


BSP_Error_Type BSP_ROM_Write(const void * pData, uint32_t Size)
{
	delay_ms(20);
	return BSP_I2C_Write(ROM_ADDR, 0, I2C_MEMADD_SIZE_16BIT, (uint8_t*)pData, 32);
}
