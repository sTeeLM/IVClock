#ifndef __IVCLOCK_USART_H__
#define __IVCLOCK_USART_H__

#include <stdint.h>
#include "bsp.h"

BSP_Error_Type BSP_USART1_UART_Init(void);
BSP_Error_Type BSP_USART2_UART_Init(void);
BSP_Error_Type BSP_USART3_UART_Init(void);

BSP_Error_Type BSP_USART1_Transmit(uint8_t *pData, uint16_t Size);
BSP_Error_Type BSP_USART1_Try_Get_Char(uint8_t * ch);
BSP_Error_Type BSP_USART1_Get_Char(uint8_t * ch);

#endif
