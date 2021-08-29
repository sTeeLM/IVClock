#include "bias.h"
#include "i2c.h"

#define BSP_BIAS_I2C_ADDR 0x4E // 0b01001110

#define BSP_BIAS_INPUT_PORT    0
#define BSP_BIAS_OUTPUT_PORT   1
#define BSP_BIAS_POLARITY_PORT 2
#define BSP_BIAS_CONFIG_PORT   3

static BSP_Error_Type BSP_BIAS_Init_Dev(void);

BSP_Error_Type BSP_BIAS_Init(void)
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
  HAL_NVIC_SetPriority(INT_ACC_EXTI_IRQn, BSP_ACC_IRQ_PRIORITY, BSP_ACC_IRQ_SUB_PRIORITY);
  HAL_NVIC_EnableIRQ(INT_ACC_EXTI_IRQn);
  
  return BSP_BIAS_Init_Dev();
}

static BSP_Error_Type BSP_BIAS_Init_Dev(void)
{
  uint8_t  val = 0;
  
  // set polarity
  val = 0;
  BSP_I2C_Write(BSP_BIAS_I2C_ADDR, BSP_BIAS_POLARITY_PORT, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // set all port to input
  val = 0xFF;
  BSP_I2C_Write(BSP_BIAS_I2C_ADDR, BSP_BIAS_CONFIG_PORT, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // read input
  BSP_I2C_Read(BSP_BIAS_I2C_ADDR, BSP_BIAS_INPUT_PORT, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  // write output
  BSP_I2C_Write(BSP_BIAS_I2C_ADDR, BSP_BIAS_OUTPUT_PORT, I2C_MEMADD_SIZE_8BIT, &val, 1);
  
  return BSP_ERROR_NONE;
}

uint8_t BSP_BIAS_Read_Int(void)
{
  uint8_t  val = 0;
  BSP_I2C_Read(BSP_BIAS_I2C_ADDR, BSP_BIAS_INPUT_PORT, I2C_MEMADD_SIZE_8BIT, &val, 1);
  return val;
}

