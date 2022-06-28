#include "tim.h"
#include "bsp.h"
#include "debug.h"
#include "timer.h"
#include "clock.h"
#include "iv18.h"
#include "delay.h"

static TIM_HandleTypeDef htim1; // beeper pmw and iv18 pmw
static TIM_HandleTypeDef htim2; // clock int
static TIM_HandleTypeDef htim3; // adc refresh
static TIM_HandleTypeDef htim4; // iv18 refresh

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

static uint8_t BSP_TIM1_Cnt;
static uint8_t BSP_TIM1_Dur;
static uint8_t BSP_TIM1_Loop;
static uint8_t BSP_TIM1_Beeper;

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 32767;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 32767;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }  
  
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /* USER CODE BEGIN TIM1_Init 2 */
  BSP_TIM1_Cnt = 0;
  BSP_TIM1_Dur = 0;
  BSP_TIM1_Loop = 0;
  BSP_TIM1_Beeper = 0;
  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);
  
  BSP_TIM1_Start();
  BSP_TIM1_Stop_PMW(TIM_CHANNEL_2);
  
  BSP_TIM1_Start_Beeper(128, 3);

  return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_TIM1_Start(void)
{
  if(HAL_TIMEx_PWMN_Start_IT(&htim1, TIM_CHANNEL_1) != HAL_OK ||
    HAL_TIMEx_PWMN_Start_IT(&htim1, TIM_CHANNEL_2) != HAL_OK ) {
      IVERR("BSP_TIM1_Start");
      return BSP_ERROR_INTERNAL;
    }
  return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_TIM1_Stop(void)
{
  if(HAL_TIMEx_PWMN_Stop_IT(&htim1, TIM_CHANNEL_1) != HAL_OK ||
    HAL_TIMEx_PWMN_Stop_IT(&htim1, TIM_CHANNEL_2) != HAL_OK ) {
      IVERR("BSP_TIM1_Stop");
      return BSP_ERROR_INTERNAL;
    }
  return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_TIM1_Set_Duty_Cycle(uint32_t Channel, uint8_t DC)
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  
  IVDBG("BSP_TIM1_Set_Duty_Cycle %d on channel %d", DC, Channel);
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = (htim1.Init.Period * DC / 100);
  IVDBG("BSP_TIM1_Set_Duty_Cycle Pulse = %d", sConfigOC.Pulse);
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, Channel) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  TIM_CCxNChannelCmd(TIM1, Channel, TIM_CCxN_ENABLE);
  
  return BSP_ERROR_NONE;
}

void BSP_TIM1_Start_PMW(uint32_t Channel)
{
  TIM_CCxNChannelCmd(TIM1, Channel, TIM_CCxN_ENABLE);
}

void BSP_TIM1_Stop_PMW(uint32_t Channel)
{
  TIM_CCxNChannelCmd(TIM1, Channel, TIM_CCxN_DISABLE);
}

void BSP_TIM1_Start_Beeper(uint8_t Dur, uint8_t Loop)
{
  BSP_TIM1_Cnt = Dur;
  BSP_TIM1_Dur = Dur;
  BSP_TIM1_Loop = Loop;
  BSP_TIM1_Start_PMW(TIM_CHANNEL_2);
  BSP_TIM1_Beeper = 1;
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_TIM2_Init(void)
{
  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim2.Init.Period = 127;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  /* USER CODE BEGIN TIM2_Init 2 */
  return BSP_ERROR_NONE;
  /* USER CODE END TIM2_Init 2 */
}

BSP_Error_Type BSP_TIM2_Start(void)
{
  if(HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) {
    return BSP_ERROR_INTERNAL;
  }
  return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_TIM2_Stop(void)
{
  if(HAL_TIM_Base_Stop_IT(&htim2) != HAL_OK) {
    return BSP_ERROR_INTERNAL;
  }
  return BSP_ERROR_NONE;
}


/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_TIM3_Init(void)
{

 /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 4096;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /* USER CODE BEGIN TIM3_Init 2 */
  return BSP_ERROR_NONE;
  /* USER CODE END TIM3_Init 2 */
}

BSP_Error_Type BSP_TIM3_Start(void)
{
  return HAL_TIM_Base_Start(&htim3) == HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

BSP_Error_Type BSP_TIM3_Stop(void)
{
  return HAL_TIM_Base_Stop(&htim3) == HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}


/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_TIM4_Init(void)
{
  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /* USER CODE BEGIN TIM4_Init 2 */
  return BSP_ERROR_NONE;
  /* USER CODE END TIM4_Init 2 */
}

BSP_Error_Type BSP_TIM4_Start(void)
{
  return HAL_TIM_Base_Start_IT(&htim4) == HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

BSP_Error_Type BSP_TIM4_Stop(void)
{
  return HAL_TIM_Base_Stop_IT(&htim4) == HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
  /* USER CODE BEGIN TIM1_MspInit 1 */
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, BSP_TIM1_IRQ_PRIORITY, BSP_TIM1_IRQ_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
  /* USER CODE END TIM1_MspInit 1 */
  }
  else if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA15     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = CLK_32KHZ_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(CLK_32KHZ_GPIO_Port, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_TIM2_PARTIAL_1();

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, BSP_TIM2_IRQ_PRIORITY, BSP_TIM2_IRQ_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
  else if(htim_base->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */
    
  /* USER CODE END TIM3_MspInit 1 */
  }
  else if(htim_base->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();
    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, BSP_TIM4_IRQ_PRIORITY, BSP_TIM4_IRQ_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspPostInit 0 */

  /* USER CODE END TIM1_MspPostInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PA7     ------> TIM1_CH1N
    PB0     ------> TIM1_CH2N
    */
    GPIO_InitStruct.Pin = IV18_BLANK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(IV18_BLANK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BEEPER_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BEEPER_GPIO_Port, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_TIM1_PARTIAL();

  /* USER CODE BEGIN TIM1_MspPostInit 1 */

  /* USER CODE END TIM1_MspPostInit 1 */
  }

}
/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();
  /* USER CODE BEGIN TIM1_MspDeInit 1 */
    HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
  /* USER CODE END TIM1_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA15     ------> TIM2_CH1
    */
    HAL_GPIO_DeInit(CLK_32KHZ_GPIO_Port, CLK_32KHZ_Pin);

    /* TIM2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspDeInit 0 */

  /* USER CODE END TIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();

    /* TIM4 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspDeInit 1 */

  /* USER CODE END TIM4_MspDeInit 1 */
  }

}


/**
  * @brief This function handles TIM1 update interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */

  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */
  if(BSP_TIM1_Beeper) {
    BSP_TIM1_Cnt --;
    if(BSP_TIM1_Cnt < BSP_TIM1_Dur / 2) {
      BSP_TIM1_Stop_PMW(TIM_CHANNEL_2);
    }
    if(BSP_TIM1_Cnt == 0) {
      if(BSP_TIM1_Loop == 0) {
        BSP_TIM1_Beeper = 0;
        BSP_TIM1_Stop_PMW(TIM_CHANNEL_2);
      } else {
        BSP_TIM1_Loop --;
        BSP_TIM1_Cnt = BSP_TIM1_Dur;
        BSP_TIM1_Start_PMW(TIM_CHANNEL_2);
      }
    }
  }
  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
  clock_inc_ms39();
  timer_inc_ms39();
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */
  BSP_IV18_Scan();
  /* USER CODE END TIM4_IRQn 1 */
}



