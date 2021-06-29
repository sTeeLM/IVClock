#include "rom.h"
#include "bsp.h"

#include "i2c.h"
#include "delay.h"

#define ROM_ADDR 0xA0

/**
  * @brief ROM Initialization Function
  * @param None
  * @retval None
  */
int8_t BSP_ROM_Init(void)
{
  return BSP_ERROR_NONE;
}

int8_t BSP_ROM_Read(void * pData, uint32_t Size)
{
	return BSP_I2C_Read(ROM_ADDR, 0, I2C_MEMADD_SIZE_8BIT, (uint8_t*)pData, Size);
}


int8_t BSP_ROM_Write(const void * pData, uint32_t Size)
{
	delay_ms(20);
	return BSP_I2C_Write(ROM_ADDR, 0, I2C_MEMADD_SIZE_8BIT, (uint8_t*)pData, Size);
}
