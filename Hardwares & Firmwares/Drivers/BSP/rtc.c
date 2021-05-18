#include "rtc.h"
#include "stm32f1xx_hal.h"
#include "bsp.h"

int8_t BSP_RTC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : INT_ALARM_Pin */
  GPIO_InitStruct.Pin = INT_ALARM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_ALARM_GPIO_Port, &GPIO_InitStruct);
	
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_ALARM_EXTI_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(INT_ALARM_EXTI_IRQn);
	
	return BSP_ERROR_NONE;
}
