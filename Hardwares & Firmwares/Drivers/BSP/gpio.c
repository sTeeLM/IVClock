#include "gpio.h"
#include "bsp.h"

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_GPIO_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* USER CODE BEGIN ADC1_Init 2 */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(POWER_33_EN_GPIO_Port, POWER_33_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(POWER_50_EN_GPIO_Port, POWER_50_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(POWER_490_EN_GPIO_Port, POWER_490_EN_Pin, GPIO_PIN_RESET);
  
  
  /*Configure GPIO pin : POWER_33_EN_Pin */
  GPIO_InitStruct.Pin = POWER_33_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(POWER_33_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : POWER_50_EN_Pin POWER_490_EN_Pin */
  GPIO_InitStruct.Pin = POWER_50_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(POWER_50_EN_GPIO_Port, &GPIO_InitStruct); 
  
  /*Configure GPIO pins : POWER_490_EN_Pin */
  GPIO_InitStruct.Pin = POWER_490_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(POWER_490_EN_GPIO_Port, &GPIO_InitStruct);  
  
  /* USER CODE END ADC1_Init 2 */
  return BSP_ERROR_NONE;
}

bool BSP_GPIO_Power_33_Enabled(void)
{
  return HAL_GPIO_ReadPin(POWER_33_EN_GPIO_Port, POWER_33_EN_Pin) == SET;
}

bool BSP_GPIO_Power_50_Enabled(void)
{
  return HAL_GPIO_ReadPin(POWER_50_EN_GPIO_Port, POWER_50_EN_Pin) == SET;
}

bool BSP_GPIO_Power_490_Enabled(void)
{
  return HAL_GPIO_ReadPin(POWER_490_EN_GPIO_Port, POWER_490_EN_Pin) == SET;
}

void BSP_GPIO_Power_33_Enable(FlagStatus enable)
{
  HAL_GPIO_WritePin(POWER_33_EN_GPIO_Port, POWER_33_EN_Pin, enable == SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void BSP_GPIO_Power_50_Enable(FlagStatus enable)
{
  HAL_GPIO_WritePin(POWER_50_EN_GPIO_Port, POWER_50_EN_Pin, enable == SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void BSP_GPIO_Power_490_Enable(FlagStatus enable)
{
  HAL_GPIO_WritePin(POWER_490_EN_GPIO_Port, POWER_490_EN_Pin, enable == SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
