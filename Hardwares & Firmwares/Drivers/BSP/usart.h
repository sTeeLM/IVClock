#ifndef __IVCLOCK_USART_H__
#define __IVCLOCK_USART_H__

#include <stdint.h>

int8_t BSP_USART1_UART_Init(void);
int8_t BSP_USART2_UART_Init(void);
int8_t BSP_USART3_UART_Init(void);

int8_t BSP_USART1_Transmit(uint8_t *pData, uint16_t Size);
int8_t BSP_USART1_Try_Get_Char(uint8_t * ch);
int8_t BSP_USART1_Get_Char(uint8_t * ch);

#endif
