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

typedef void (*USART_TRANSMIT_CPLT_CB)(void);
typedef void (*USART_RECEIVE_CPLT_CB)(void);
void BSP_USART2_Set_Transmit_CB(USART_TRANSMIT_CPLT_CB cb);
void BSP_USART2_Set_Receive_CB(USART_RECEIVE_CPLT_CB cb);
BSP_Error_Type BSP_USART2_Transmit(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART2_Receive(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART2_Transmit_IT(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART2_Receive_IT(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART2_Wait_Transmit_Cplt(uint16_t Timeout);
BSP_Error_Type BSP_USART2_Wait_Receive_Cplt(uint16_t Timeout);

#endif
