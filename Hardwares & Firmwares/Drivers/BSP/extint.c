#include "extint.h"
#include "bsp.h"
#include "task.h"
#include "alarm.h"
#include "motion_sensor.h"
#include "remote_control.h"
#include "player.h"
#include "power.h"

/**
  * @brief ROM Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_ExtInt_Init(void)
{
  return BSP_ERROR_NONE;
}

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(INT_MP3_Pin);
  /* USER CODE BEGIN EXTI3_IRQn 1 */

  /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

  /* USER CODE END EXTI3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(INT_KEY_SET_Pin);
  /* USER CODE BEGIN EXTI3_IRQn 1 */
  /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */

  /* USER CODE END EXTI4_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(INT_KEY_MOD_Pin);
  
  /* USER CODE BEGIN EXTI4_IRQn 1 */
  /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(INT_BT_Pin);
  HAL_GPIO_EXTI_IRQHandler(INT_ALARM_Pin);
  HAL_GPIO_EXTI_IRQHandler(INT_ACC_Pin);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == INT_ALARM_Pin ) {
    alarm_scan();
  } else if(GPIO_Pin == INT_ACC_Pin ) {
    motion_sensor_scan();
    power_wakeup();
  } else if(GPIO_Pin == INT_BT_Pin ) {
    remote_control_scan();
    power_wakeup();
  } else if(GPIO_Pin == INT_MP3_Pin) {
    player_scan();
    power_wakeup();
  } else if(GPIO_Pin == INT_KEY_SET_Pin || GPIO_Pin == INT_KEY_MOD_Pin) {
    power_wakeup();
    player_stop_play();
  }
  power_reset_timeo();
}