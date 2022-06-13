#ifndef __IVCLOCK_DELAY_H__
#define __IVCLOCK_DELAY_H__

#include <stdint.h>

#include "stm32f1xx_hal.h"

uint8_t delay_init(void);

/**
 * @brief  This function provides a delay (in microseconds)
 * @param  microseconds: delay in microseconds
 */
__STATIC_INLINE void delay_us(volatile uint32_t us)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;

  /* Go to number of cycles for system */
  us *= (HAL_RCC_GetHCLKFreq() / 1000000);

  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < us);
} 

__STATIC_INLINE  void delay_ms(uint32_t ms)
{
  uint32_t i;
  for(i = 0 ; i < ms ; i ++) {
    delay_us(1000);
  }
}

#endif