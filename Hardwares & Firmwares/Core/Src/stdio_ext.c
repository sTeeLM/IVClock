#include <stdio.h>
#include "bsp.h"
#include "usart.h"

int fputc(int ch, FILE* file)
{
	BSP_USART1_Transmit((uint8_t *)&ch, 1);
	return ch;
}

int fgetc(FILE* file)
{
	return BSP_USART1_Get_Char();
}