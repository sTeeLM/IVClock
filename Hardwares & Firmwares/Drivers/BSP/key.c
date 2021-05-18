#include "key.h"
#include "stm32f1xx_hal.h"
#include "bsp.h"

int8_t BSP_Key_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : INT_KEY_MOD_Pin */
  GPIO_InitStruct.Pin = INT_KEY_MOD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_KEY_MOD_GPIO_Port, &GPIO_InitStruct);
	
  /*Configure GPIO pins : INT_KEY_SET_Pin */
  GPIO_InitStruct.Pin = INT_KEY_SET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_KEY_SET_GPIO_Port, &GPIO_InitStruct);	
	
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_KEY_MOD_EXTI_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(INT_KEY_MOD_EXTI_IRQn);

  HAL_NVIC_SetPriority(INT_KEY_SET_EXTI_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(INT_KEY_SET_EXTI_IRQn);	
	
	return BSP_ERROR_NONE;
}
