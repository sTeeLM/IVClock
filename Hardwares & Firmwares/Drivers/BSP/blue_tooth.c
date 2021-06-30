#include "blue_tooth.h"
#include "bsp.h"
#include "debug.h"

BSP_Error_Type BSP_Blue_Tooth_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	IVDBG("BSP_Blue_Tooth_Init");
	
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : INT_BT_Pin */
  GPIO_InitStruct.Pin = INT_BT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_BT_GPIO_Port, &GPIO_InitStruct);
	
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_BT_EXTI_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(INT_BT_EXTI_IRQn);
	
	return BSP_ERROR_NONE;
}
