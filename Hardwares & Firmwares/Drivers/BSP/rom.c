#include "rom.h"
#include "bsp.h"
#include "debug.h"
#include "i2c.h"
#include "delay.h"

#define ROM_ADDR 0xAE
// AT24C32
// page Size is 32 bytes
// total size is 4096 bytes (32768 bits)
#define ROM_PAGE_SIZE 32
#define ROM_TOTAL_SIZE 4096
#define ROM_PAGES  128
#define ROM_MEM_ADDRSIZE I2C_MEMADD_SIZE_16BIT


void _rom_dump(void) 
{
	int32_t i, j;
	uint8_t buf[ROM_PAGE_SIZE];
	IVDBG("dump rom content begin------------------------");
	for(i = 0 ; i < ROM_PAGES ; i ++) {
			BSP_ROM_Read(i * ROM_PAGE_SIZE, buf, ROM_PAGE_SIZE);
			IVDBG_N("[DBG ] ");
			for(j = 0 ; j < ROM_PAGE_SIZE ; j ++) {
				IVDBG_N("%02X ", buf[j]);
			}
			IVDBG_N("\r\n");
	}
	IVDBG("dump rom content end------------------------");
}

/**
  * @brief ROM Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_ROM_Init(void)
{
	_rom_dump();
  return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_ROM_Read(uint32_t Addr, uint8_t * pData, uint32_t Size)
{
	if ((Addr + Size) > ROM_TOTAL_SIZE)
		return BSP_ERROR_INTERNAL;
	return BSP_I2C_Read(ROM_ADDR, Addr, ROM_MEM_ADDRSIZE, (uint8_t*)pData, Size);
}

BSP_Error_Type BSP_ROM_Write(uint32_t Addr, uint8_t * pData, uint32_t Size)
{
	uint32_t head_size, index = 0, i;
	if ((Addr + Size) > ROM_TOTAL_SIZE)
		return BSP_ERROR_INTERNAL;
	
	head_size =(ROM_PAGE_SIZE - Addr % ROM_PAGE_SIZE) % ROM_PAGE_SIZE;
	head_size = head_size > Size ? Size : head_size;

	for(i = 0 ; i < head_size; i ++) {
		if(BSP_I2C_Write(ROM_ADDR, Addr, ROM_MEM_ADDRSIZE, &(pData[index]), 1) != BSP_ERROR_NONE)
			return BSP_ERROR_NONE;
		delay_ms(10);
		Size --;
		Addr ++;
		index ++;
	}
	
	for(i = 0 ; i < Size / ROM_PAGE_SIZE; i ++) {
		if(BSP_I2C_Write(ROM_ADDR, Addr, ROM_MEM_ADDRSIZE, &(pData[index]), ROM_PAGE_SIZE) != BSP_ERROR_NONE)
			return BSP_ERROR_NONE;
		delay_ms(10);
		Size -= ROM_PAGE_SIZE;
		Addr  += ROM_PAGE_SIZE;
		index += ROM_PAGE_SIZE;
	}
	
	for(i = 0 ; i < Size % ROM_PAGE_SIZE; i ++) {
		if(BSP_I2C_Write(ROM_ADDR, Addr, ROM_MEM_ADDRSIZE, &(pData[index]), 1) != BSP_ERROR_NONE)
			return BSP_ERROR_NONE;
		delay_ms(10);
		Size --;
		Addr ++;
		index ++;
	}
	return BSP_ERROR_NONE;
}
