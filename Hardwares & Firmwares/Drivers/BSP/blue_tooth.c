#include "blue_tooth.h"
#include "bsp.h"
#include "debug.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

static BSP_Error_Type BSP_Blue_Tooth_AT_Cmd(char * Command, 
  uint8_t * Response, uint16_t * ResLen)
{
  uint8_t Buf, Len, CmdLen;
  BSP_Error_Type Ret = BSP_ERROR_INTERNAL;
  

  if(Response == NULL || ResLen == NULL || *ResLen == 0) {
    return BSP_ERROR_INTERNAL;
  }
  
  Len = *ResLen;
  (*ResLen) = 0;
  CmdLen = strlen(Command);  
  
  if(BSP_USART3_Transmit((uint8_t *)Command, CmdLen) == BSP_ERROR_NONE) {
    while((Ret = BSP_USART3_Receive(&Buf, 1)) == BSP_ERROR_NONE) {
      if(Buf == 0xd) {
        if((Ret = BSP_USART3_Receive(&Buf, 1)) == BSP_ERROR_NONE) {
          if(Buf != 0xa) {
            Ret = BSP_ERROR_INTERNAL;
            IVERR("BSP_Blue_Tooth_AT_Cmd: unknown char %c", Buf);
          }
          break;
        } else {
          break;
        }
      }
      Response[(*ResLen) ++] = Buf;
      if((*ResLen) >= Len - 1) {
        break;
      }
    }
    Response[*ResLen] = 0;
    return Ret;
  } else {
    IVERR("BSP_USART3_Transmit error");
  }
  return Ret;
}

BSP_Error_Type BSP_Blue_Tooth_Rst(void)
{
  uint8_t res[32];
  uint16_t len = sizeof(res);
  BSP_Error_Type ret = BSP_Blue_Tooth_AT_Cmd("AT+RST\r\n", res, &len);
  return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_Blue_Tooth_Disc(void)
{
  uint8_t res[32];
  uint16_t len = sizeof(res);
  BSP_Error_Type ret = BSP_Blue_Tooth_AT_Cmd("AT+DISC\r\n", res, &len);
  return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_Blue_Tooth_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : INT_BT_Pin */
  GPIO_InitStruct.Pin = INT_BT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_BT_GPIO_Port, &GPIO_InitStruct);
  
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_BT_EXTI_IRQn, BSP_BT_IRQ_PRIORITY, BSP_BT_IRQ_SUB_PRIORITY);
  HAL_NVIC_EnableIRQ(INT_BT_EXTI_IRQn);
  
  BSP_Blue_Tooth_Rst();
  
  return BSP_ERROR_NONE;
}

