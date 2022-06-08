#include "rom.h"
#include "bsp.h"
#include "debug.h"
#include "i2c.h"
#include "delay.h"

#define ROM_ADDR 0xAE
// AT24C32
// page Size is 32 bytes
// total size is 4096 bytes (32768 bits)
//#define ROM_PAGE_SIZE 32
//#define ROM_TOTAL_SIZE 4096
//#define ROM_PAGES  128
//#define ROM_MEM_ADDRSIZE I2C_MEMADD_SIZE_16BIT


// AT24C02
// page Size is 32 bytes
// total size is 4096 bytes (32768 bits)
#define ROM_PAGE_SIZE 8
#define ROM_TOTAL_SIZE 256
#define ROM_PAGES  32
#define ROM_MEM_ADDRSIZE I2C_MEMADD_SIZE_8BIT

void _rom_dump(void) 
{
  int32_t i, j;
  uint8_t buf[ROM_PAGE_SIZE];
  IVDBG("dump rom content begin------------------------");
  for(i = 0 ; i < ROM_PAGES ; i ++) {
      BSP_ROM_Read(i * ROM_PAGE_SIZE, buf, ROM_PAGE_SIZE);
      IVDBG_RH;
      for(j = 0 ; j < ROM_PAGE_SIZE ; j ++) {
        IVDBG_R("%02X ", buf[j]);
      }
      IVDBG_RT;
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
  BSP_Error_Type ret;
  if ((Addr + Size) > ROM_TOTAL_SIZE)
    return BSP_ERROR_INTERNAL;
  ret = BSP_I2C_Read(ROM_ADDR, Addr, ROM_MEM_ADDRSIZE, (uint8_t*)pData, Size);
  
  if(ret != BSP_ERROR_NONE) {
    IVERR("BSP_ROM_Read[%04x] pData = %04x Size = %04x ret = %d", Addr, pData, Size, ret);
  } else {
    IVDBG("BSP_ROM_Read[%04x] pData = %04x Size = %04x ret = %d", Addr, pData, Size, ret);
  }
  
  return ret;
}

static BSP_Error_Type _BSP_I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  return  BSP_I2C_Write(DevAddress, MemAddress, MemAddSize, pData, Size);
}

BSP_Error_Type BSP_ROM_Write(uint32_t Addr, uint8_t * pData, uint32_t Size)
{
  uint32_t i, header_size;
  BSP_Error_Type ret;
  
  if ((Addr + Size) > ROM_TOTAL_SIZE) {
    ret =  BSP_ERROR_INTERNAL;
    goto out;
  }
  
  header_size = ROM_PAGE_SIZE - (Size % ROM_PAGE_SIZE);
  header_size = header_size > Size ? Size : header_size;
  
  for( i = 0 ; i < header_size ; i ++) {
    delay_ms(10);
    if((ret = _BSP_I2C_Write(ROM_ADDR, Addr + i, ROM_MEM_ADDRSIZE, pData +  i, 1)) != BSP_ERROR_NONE) {
      goto out;
    }
  }
  
  Size  -= header_size;
  Addr  += header_size;
  pData += header_size;
  
  for( i = 0 ; i < Size / ROM_PAGE_SIZE ; i ++) {
    delay_ms(10);
    if((ret = _BSP_I2C_Write(ROM_ADDR, Addr, ROM_MEM_ADDRSIZE, pData, ROM_PAGE_SIZE)) != BSP_ERROR_NONE) {
      goto out;
    }
    Size  -= ROM_PAGE_SIZE;
    Addr  += ROM_PAGE_SIZE;
    pData += ROM_PAGE_SIZE;
  }
  
  for( i = 0 ; i < Size % ROM_PAGE_SIZE ; i ++) {
    delay_ms(10);
    if((ret = _BSP_I2C_Write(ROM_ADDR, Addr + i, ROM_MEM_ADDRSIZE, pData +  i, 1)) != BSP_ERROR_NONE) {
      goto out;
    }
  }
  
out:
  
  if(ret != BSP_ERROR_NONE) {
    IVERR("BSP_ROM_Write[%04x] pData = %04x Size = %04x ret = %d", Addr, pData, Size);
  } else {
    IVDBG("BSP_ROM_Write[%04x] pData = %04x Size = %04x ret = %d", Addr, pData, Size);
  }
  return ret;
}
