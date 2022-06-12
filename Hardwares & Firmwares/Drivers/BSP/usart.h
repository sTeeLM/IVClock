#ifndef __IVCLOCK_USART_H__
#define __IVCLOCK_USART_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_USART1_UART_Init(void);
BSP_Error_Type BSP_USART2_UART_Init(void);
BSP_Error_Type BSP_USART3_UART_Init(void);

BSP_Error_Type BSP_USART1_Transmit(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART1_Receive(uint8_t *pData, uint16_t Size);
int16_t BSP_USART1_Get_Char(void);
int16_t BSP_USART1_Try_Get_Char(void);

BSP_Error_Type BSP_USART2_Transmit(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART2_Receive(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART2_Transmit_IT(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART2_Receive_IT(uint8_t *pData, uint16_t Size);
void BSP_UART2_Enter_High_Impedance(void);

BSP_Error_Type BSP_USART3_Receive(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART3_Transmit(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART3_Receive_IT(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART3_Transmit_IT(uint8_t *pData, uint16_t Size);

#endif
