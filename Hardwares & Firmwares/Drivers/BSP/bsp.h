#ifndef __IVCLOCK_BSP_H__
#define __IVCLOCK_BSP_H__

#include "stm32f1xx_hal.h"

#define POWER_33_EN_Pin GPIO_PIN_13
#define POWER_33_EN_GPIO_Port GPIOC
#define POWER_50_EN_Pin GPIO_PIN_1
#define POWER_50_EN_GPIO_Port GPIOA
#define POWER_490_EN_Pin GPIO_PIN_11
#define POWER_490_EN_GPIO_Port GPIOA
#define POWER_IV18_EN_Pin GPIO_PIN_12
#define POWER_IV18_EN_GPIO_Port GPIOA
#define BATTERY_MON_Pin GPIO_PIN_4
#define BATTERY_MON_GPIO_Port GPIOA
#define BEEPER_Pin GPIO_PIN_0
#define BEEPER_GPIO_Port GPIOB
#define I2C_GPIO_Port GPIOB
#define I2C_SCL_Pin GPIO_PIN_6
#define I2C_SDA_Pin GPIO_PIN_7
#define IV18_BLANK_Pin GPIO_PIN_7
#define IV18_BLANK_GPIO_Port GPIOA
#define LIGHT_MON_Pin GPIO_PIN_5
#define LIGHT_MON_GPIO_Port GPIOA
#define IV18_LOAD_Pin GPIO_PIN_14
#define IV18_LOAD_GPIO_Port GPIOB
#define IV18_CLK_Pin GPIO_PIN_15
#define IV18_CLK_GPIO_Port GPIOB
#define IV18_DIN_Pin GPIO_PIN_8
#define IV18_DIN_GPIO_Port GPIOA
#define CLK_32KHZ_Pin GPIO_PIN_15
#define CLK_32KHZ_GPIO_Port GPIOA
#define INT_KEY_SET_Pin GPIO_PIN_3
#define INT_KEY_SET_GPIO_Port GPIOB
#define INT_KEY_SET_EXTI_IRQn EXTI3_IRQn
#define INT_KEY_MOD_Pin GPIO_PIN_4
#define INT_KEY_MOD_GPIO_Port GPIOB
#define INT_KEY_MOD_EXTI_IRQn EXTI4_IRQn
#define INT_BT_Pin GPIO_PIN_5
#define INT_BT_GPIO_Port GPIOB
#define INT_BT_EXTI_IRQn EXTI9_5_IRQn
#define INT_ALARM_Pin GPIO_PIN_8
#define INT_ALARM_GPIO_Port GPIOB
#define INT_ALARM_EXTI_IRQn EXTI9_5_IRQn
#define INT_ACC_Pin GPIO_PIN_9
#define INT_ACC_GPIO_Port GPIOB
#define INT_ACC_EXTI_IRQn EXTI9_5_IRQn
#define INT_MP3_Pin GPIO_PIN_1
#define INT_MP3_GPIO_Port GPIOB
#define INT_MP3_EXTI_IRQn EXTI1_IRQn

// clock
#define BSP_TIM2_IRQ_PRIORITY     0
#define BSP_TIM2_IRQ_SUB_PRIORITY 0

// display refresh
#define BSP_TIM4_IRQ_PRIORITY     1
#define BSP_TIM4_IRQ_SUB_PRIORITY 0

// beeper pmw
#define BSP_TIM1_IRQ_PRIORITY     1
#define BSP_TIM1_IRQ_SUB_PRIORITY 1

// key
#define BSP_KEY_IRQ_PRIORITY      1
#define BSP_KEY_IRQ_SUB_PRIORITY  2

#define BSP_USART_IRQ_PRIORITY       2
#define BSP_USART_IRQ_SUB_PRIORITY   0

#define BSP_MP3_IRQ_PRIORITY          2
#define BSP_MP3_IRQ_SUB_PRIORITY      1

// alarm
#define BSP_ALARM_IRQ_PRIORITY        3
#define BSP_ALARM_IRQ_SUB_PRIORITY    0

// blue tooth
#define BSP_BT_IRQ_PRIORITY       3
#define BSP_BT_IRQ_SUB_PRIORITY   1

// acc
#define BSP_ACC_IRQ_PRIORITY      3
#define BSP_ACC_IRQ_SUB_PRIORITY  2

// adc
#define BSP_ADC_IRQ_PRIORITY      3
#define BSP_ADC_IRQ_SUB_PRIORITY  3

#define BSP_I2C_TRAILS       5U
#define BSP_I2C_SEND_TIMEOUT 50000U
#define BSP_I2C_RECV_TIMEOUT 50000U

#define BSP_USART1_TRANSMIT_TIMEOUT 1000U
#define BSP_USART1_RECEIVE_TIMEOUT  1000U

#define BSP_USART2_TRANSMIT_TIMEOUT 10000U
#define BSP_USART2_RECEIVE_TIMEOUT  10000U

typedef enum {
  BSP_ERROR_NONE = 0,
  BSP_ERROR_INTERNAL,
  BSP_ERROR_TIMEOUT,
  BSP_ERROR_BUSY
}BSP_Error_Type;

#endif
