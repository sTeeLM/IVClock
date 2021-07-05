#include "acc.h"
#include "bsp.h"
#include "i2c.h"
#include "config.h"
#include "debug.h"
#include "delay.h"


#define ACC_I2C_ADDRESS  0xA6 //1010 0110

#define ACC_THRESHOLD_INVALID (0xFF)
#define ACC_THRESHOLD_MAX (99)
#define ACC_THRESHOLD_MIN (1)

bool BSP_Read_Int_Status(void)
{
	uint8_t val;
	BSP_I2C_Read(ACC_I2C_ADDRESS, 0x30, I2C_MEMADD_SIZE_8BIT, &val, 1);
	return (val & 0x80) != 0;
}

uint8_t BSP_ACC_Threshold_Get(void)
{
  unsigned char val;
  BSP_I2C_Read(ACC_I2C_ADDRESS, 0x24, I2C_MEMADD_SIZE_8BIT, &val, 1);
  return val;
}

void BSP_ACC_Threshold_Set(uint8_t th)
{
  if(th != ACC_THRESHOLD_INVALID) {
    if(th > ACC_THRESHOLD_MAX) {
      th = ACC_THRESHOLD_MAX;
    }
    if(th < ACC_THRESHOLD_MIN) {
      th = ACC_THRESHOLD_MIN;
    }
  }	
	BSP_I2C_Write(ACC_I2C_ADDRESS, 0x24, I2C_MEMADD_SIZE_8BIT, &th, 1);
}

void BSP_ACC_Power_On(void)
{
	uint8_t val;
	
  BSP_ACC_Threshold_Set(config_read("acc_th")->val8);
  
  // Register 0x2E—INT_ENABLE (Read/Write), enable Activity
  val = 0x10;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x2E, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // Register 0x2D—POWER_CTL (Read/Write), don't into autosleep
  // 00
  // Link = 0
  // AUTO_SLEEP = 0
  // Measure Bit = 1
  // Sleep = 0
  // Wakeup = 00
  val = 0x08;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x2D, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
	delay_ms(10);
	
	BSP_Read_Int_Status();	
}

void BSP_ACC_Power_Off(void)
{
  uint8_t val = 0;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x2E, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // Register 0x2D—POWER_CTL (Read/Write) into stand by
  // 00
  // Link = 0
  // AUTO_SLEEP = 0
  // Measure = 0
  // Sleep = 1
  // Wakeup = 00
  val = 0x40;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x2D, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
	delay_ms(10);
	
  BSP_Read_Int_Status();	
}

static void BSP_ACC_Init_Device(void)
{
	uint8_t val;
	
  // Register 0x31—DATA_FORMAT (Read/Write)
  // INT_INVERT = 1， interrupts to active low
  // Range Bits = 00， +-2G
	val = 0x20;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x31, I2C_MEMADD_SIZE_8BIT, &val, 1);

  // Register 0x2E—INT_ENABLE (Read/Write), all DISABLE
	val = 0;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x2E, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // Register 0x2F—INT_MAP (R/W), all to INT1
	val = 0;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x2F, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // THRESH_ACT (Read/Write)
  BSP_ACC_Threshold_Set(config_read("acc_th")->val8);
  
  //Register 0x27—ACT_INACT_CTL (Read/Write)
  // ACT ac/dc = 1 (ac)
  // ACT_X enable = 1
  // ACT_X enable = 1
  // ACT_Y enable = 1
  // INACT ac/dc = 0 (ac)
  // INACT_X enable = 0
  // INACT_Y enable = 0
  // INACT_Z enable = 0
	val = 0xF0;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x27, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // Register 0x25—THRESH_INACT (Read/Write) -> disable
	val = 0;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x25, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // Register 0x26—TIME_INACT (Read/Write) -> disable
	val = 0;
	BSP_I2C_Write(ACC_I2C_ADDRESS, 0x26, I2C_MEMADD_SIZE_8BIT, &val, 1);

  // Register 0x2D—POWER_CTL (Read/Write) into stand by
  // 00
  // Link = 0
  // AUTO_SLEEP = 0
  // Measure = 0
  // Sleep = 1
  // Wakeup = 00
	val = 0x40;
  BSP_I2C_Write(ACC_I2C_ADDRESS, 0x2D, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  BSP_Read_Int_Status();
}

BSP_Error_Type BSP_ACC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : INT_ACC_Pin */
  GPIO_InitStruct.Pin = INT_ACC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_ACC_GPIO_Port, &GPIO_InitStruct);
	
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_ACC_EXTI_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(INT_ACC_EXTI_IRQn);
	
	return BSP_ERROR_NONE;
}
