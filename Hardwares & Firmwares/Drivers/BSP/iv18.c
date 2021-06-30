#include "iv18.h"
#include "bsp.h"
#include "debug.h"

BSP_Error_Type BSP_IV18_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	IVDBG("BSP_IV18_Init");
	
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();	
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : IV18_BLANK_Pin IV18_LOAD_Pin IV18_CLK_Pin IV18_DIN_Pin */
  GPIO_InitStruct.Pin = IV18_BLANK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_BLANK_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = IV18_LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_LOAD_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = IV18_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_CLK_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = IV18_DIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_DIN_GPIO_Port, &GPIO_InitStruct);	
	
	return BSP_ERROR_NONE;
}
