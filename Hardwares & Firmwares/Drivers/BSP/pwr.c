#include "pwr.h"
#include "bsp.h"
#include "debug.h"

BSP_Error_Type BSP_PWR_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(POWER_33_EN_GPIO_Port, POWER_33_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(POWER_50_EN_GPIO_Port, POWER_50_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(POWER_490_EN_GPIO_Port, POWER_490_EN_Pin, GPIO_PIN_RESET);
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(POWER_IV18_EN_GPIO_Port, POWER_IV18_EN_Pin, GPIO_PIN_RESET);
  
  /*Configure GPIO pin : POWER_33_EN_Pin */
  GPIO_InitStruct.Pin = POWER_33_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(POWER_33_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : POWER_50_EN_Pin POWER_490_EN_Pin */
  GPIO_InitStruct.Pin = POWER_50_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(POWER_50_EN_GPIO_Port, &GPIO_InitStruct); 
  
  /*Configure GPIO pins : POWER_490_EN_Pin */
  GPIO_InitStruct.Pin = POWER_490_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(POWER_490_EN_GPIO_Port, &GPIO_InitStruct);  
  
  /*Configure GPIO pins : POWER_490_EN_Pin */
  GPIO_InitStruct.Pin = POWER_IV18_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(POWER_IV18_EN_GPIO_Port, &GPIO_InitStruct); 
  
  /* USER CODE END ADC1_Init 2 */
  return BSP_ERROR_NONE;
}

bool BSP_GPIO_PWR_33_Enabled(void)
{
  return HAL_GPIO_ReadPin(POWER_33_EN_GPIO_Port, POWER_33_EN_Pin) == SET;
}

bool BSP_GPIO_PWR_50_Enabled(void)
{
  return HAL_GPIO_ReadPin(POWER_50_EN_GPIO_Port, POWER_50_EN_Pin) == SET;
}

bool BSP_GPIO_PWR_490_Enabled(void)
{
  return HAL_GPIO_ReadPin(POWER_490_EN_GPIO_Port, POWER_490_EN_Pin) == SET;
}

bool BSP_GPIO_PWR_IV18_Enabled(void)
{
  return HAL_GPIO_ReadPin(POWER_IV18_EN_GPIO_Port, POWER_IV18_EN_Pin) == SET;
}

void BSP_GPIO_PWR_33_Enable(FlagStatus enable)
{
  HAL_GPIO_WritePin(POWER_33_EN_GPIO_Port, POWER_33_EN_Pin, enable == SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void BSP_GPIO_PWR_50_Enable(FlagStatus enable)
{
  HAL_GPIO_WritePin(POWER_50_EN_GPIO_Port, POWER_50_EN_Pin, enable == SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void BSP_GPIO_PWR_490_Enable(FlagStatus enable)
{
  HAL_GPIO_WritePin(POWER_490_EN_GPIO_Port, POWER_490_EN_Pin, enable == SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void BSP_GPIO_PWR_IV18_Enable(FlagStatus enable)
{
  HAL_GPIO_WritePin(POWER_IV18_EN_GPIO_Port, POWER_IV18_EN_Pin, enable == SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


static void BSP_PWR_Sleep_Pre(void)
{
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE(); 
  __HAL_RCC_GPIOB_CLK_DISABLE();   
  __HAL_RCC_AFIO_CLK_DISABLE();
  __HAL_RCC_ADC1_CLK_DISABLE();
  __HAL_RCC_ADC2_CLK_DISABLE();
  __HAL_RCC_TIM1_CLK_DISABLE();
  __HAL_RCC_TIM2_CLK_DISABLE();
  __HAL_RCC_TIM3_CLK_DISABLE(); 
  __HAL_RCC_TIM4_CLK_DISABLE();   
  __HAL_RCC_USART1_CLK_DISABLE();
  __HAL_RCC_USART2_CLK_DISABLE();  
  __HAL_RCC_USART3_CLK_DISABLE(); 
  __HAL_RCC_PWR_CLK_ENABLE();
}

static void BSP_PWR_Sleep_Post(void)
{
  
}

void BSP_PWR_Sleep(void)
{
  //BSP_PWR_Sleep_Pre();
  //HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
  HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  BSP_PWR_SystemClock_Config();
  //BSP_PWR_Sleep_Post();
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
BSP_Error_Type BSP_PWR_SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  return BSP_ERROR_NONE;
}