/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "debug.h"
#include "delay.h"
#include "version.h"

/* bsp */
#include "adc.h"
#include "i2c.h"
#include "iv18.h"
#include "key.h"
#include "mp3.h"
#include "tim.h"
#include "usart.h"
#include "blue_tooth.h"
#include "gpio.h"
#include "rom.h"
#include "bias.h"
#include "ds3231.h"
#include "pwr.h"

/* system software */
#include "alarm.h"
#include "beeper.h"
#include "button.h"
#include "clock.h"
#include "config.h"
#include "display.h"
#include "player.h"
#include "power.h"
#include "timer.h"
#include "console.h"
#include "motion_sensor.h"
#include "remote_control.h"
#include "thermometer.h"
#include "ticks.h"
#include "task.h"
#include "sm.h"
#include "rtc.h"

#include "build_id.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

void PrintBsp(const char * bsp, BSP_Error_Type res)
{
  IVINFO("%s %s", bsp, res == BSP_ERROR_NONE ? "OK" : "FAILED");
}

void PrintBuildID(void)
{
  uint8_t i;
  
  IVINFO("Build-ID:");
  
  IVINFO_RH;
  
  for(i = 0 ; i < sizeof(build_id); i ++) {
    IVINFO_R("%02x", build_id[i]);
  }
  
  IVINFO_RT;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  BSP_PWR_SystemClock_Config();
  
  // 必须先初始化delay子系统和ticks子系统
  delay_init();
  ticks_init();

  /* Hardware initialize */
  BSP_GPIO_Init();  
  PrintBsp("USART1", BSP_USART1_UART_Init());
  PrintBsp("USART2", BSP_USART2_UART_Init());
  PrintBsp("USART3", BSP_USART3_UART_Init());
  PrintBsp("PWR   ", BSP_PWR_Init());
  PrintBsp("Key   ", BSP_Key_Init());  
  PrintBsp("I2C   ", BSP_I2C_Init());
  PrintBsp("ROM   ", BSP_ROM_Init());
  config_init();  
  PrintBsp("DS3231", BSP_DS3231_Init());
  PrintBsp("ADC1  ", BSP_ADC1_Init());
  PrintBsp("ADC2  ", BSP_ADC2_Init());  
  PrintBsp("IV18  ", BSP_IV18_Init());
  PrintBsp("MP3   ", BSP_MP3_Init());
  PrintBsp("TIM1  ", BSP_TIM1_Init());
  PrintBsp("TIM2  ", BSP_TIM2_Init());
  PrintBsp("TIM3  ", BSP_TIM3_Init());
  PrintBsp("TIM4  ", BSP_TIM4_Init());  
//  PrintBsp("ACC   ", BSP_ACC_Init());
  PrintBsp("BIAS  ", BSP_BIAS_Init());  
  PrintBsp("Blue_Tooth ", BSP_Blue_Tooth_Init());
  
  PrintBuildID();
  
  IVDBG("initialize sub systems version %02d.%02d...", 
    IVCLOCK_VERSION_MAJOR, IVCLOCK_VERSION_MINOR);
  /* System initialize */
  rtc_init();
  power_init();
  console_init();
  clock_init();
  alarm_init();
  timer_init();
  button_init();
  beeper_init();
  display_init();
  player_init();
  motion_sensor_init();
  remote_control_init();
  thermometer_init();
  task_init();
  sm_init();
  IVDBG("sub systems initialize done");
  
  display_on();
  
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    task_run();
    remote_control_run();
    console_run();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
