#include "blue_tooth.h"
#include "bsp.h"
#include "debug.h"
#include "usart.h"
#include "delay.h"

#define BSP_BLUE_TOOTH_RECV_TIMEOUT 6000 // 60ms

BSP_Error_Type BSP_Blue_Tooth_AT_Cmd(uint8_t * Command, uint8_t CmdLen, 
  uint8_t * Response, uint16_t * ResLen)
{
  uint8_t Buf, Len = *ResLen;
  
  if(BSP_USART3_Start_Receive_String(NULL) == BSP_ERROR_NONE) {
    if(BSP_USART3_Transmit(Command, CmdLen) == BSP_ERROR_NONE) {
      if(BSP_USART3_End_Receive_String(Response, ResLen, BSP_BLUE_TOOTH_RECV_TIMEOUT) 
        == BSP_ERROR_NONE) {
          return BSP_ERROR_NONE;
       } else {
         IVERR("BSP_USART3_End_Receive_String error");
       }
    } else {
      IVERR("BSP_USART3_Transmit error");
    }
  } else {
    IVERR("BSP_USART3_Start_Receive_String error");
  }
  return BSP_ERROR_INTERNAL;
}

BSP_Error_Type BSP_Blue_Tooth_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  uint8_t buffer[64];
  uint16_t len;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : INT_BT_Pin */
  GPIO_InitStruct.Pin = INT_BT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_BT_GPIO_Port, &GPIO_InitStruct);
  
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_BT_EXTI_IRQn, BSP_BT_IRQ_PRIORITY, BSP_BT_IRQ_SUB_PRIORITY);
  HAL_NVIC_EnableIRQ(INT_BT_EXTI_IRQn);
  
  len = 64;
  BSP_Blue_Tooth_AT_Cmd("AT+VERSION\r\n", 12, buffer, &len);
  buffer[sizeof(buffer) - 1] = 0;
  IVDBG("BSP_Blue_Tooth_Init return %s", buffer);
  
  return BSP_ERROR_NONE;
}

