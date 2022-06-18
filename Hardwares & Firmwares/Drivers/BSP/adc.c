#include "adc.h"
#include "bsp.h"
#include "debug.h"
#include "delay.h"
#include "power.h"

static ADC_HandleTypeDef hadc1; // bat
static ADC_HandleTypeDef hadc2; // light


#define ADC1_VALUE_SIZE 16
static uint16_t ADC1ValueArray[ADC1_VALUE_SIZE];
static uint8_t  ADC1ValueIndex;

#define ADC2_VALUE_SIZE 16
static uint16_t ADC2ValueArray[ADC2_VALUE_SIZE];
static uint8_t  ADC2ValueIndex;

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_ADC1_Init(void)
{
  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }

  if (ADC_Enable(&hadc1) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  /* USER CODE BEGIN ADC1_Init 2 */
  return BSP_ERROR_NONE;
  /* USER CODE END ADC1_Init 2 */
}

BSP_Error_Type BSP_ADC1_Start(void)
{
  return HAL_ADC_Start_IT(&hadc1) == HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

BSP_Error_Type BSP_ADC1_Stop(void)
{
  return HAL_ADC_Stop_IT(&hadc1) == HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

BSP_Error_Type BSP_ADC1_Set_WDG(uint16_t LowThreshold, uint16_t HiThreshold)
{
  ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};
  /** Configure Analog WatchDog 1
  */
  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
  AnalogWDGConfig.HighThreshold = HiThreshold;
  AnalogWDGConfig.LowThreshold = LowThreshold;
  AnalogWDGConfig.Channel = ADC_CHANNEL_4;
  AnalogWDGConfig.ITMode = ENABLE;
  if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }  
  return BSP_ERROR_NONE;
  
}

uint16_t BSP_ADC1_Get_Value(void)
{
  int8_t i;
  uint32_t val = 0;

  for(i = 0 ; i < ADC1_VALUE_SIZE ; i ++) {
    val += ADC1ValueArray[i];
  }

  
  return val / 16;
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO; //ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  if (ADC_Enable(&hadc2) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  if (HAL_ADCEx_Calibration_Start(&hadc2) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  
  /* USER CODE BEGIN ADC2_Init 2 */
  return BSP_ERROR_NONE;
  /* USER CODE END ADC2_Init 2 */
  
}

BSP_Error_Type BSP_ADC2_Start(void)
{
  return HAL_ADC_Start_IT(&hadc2) == HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

BSP_Error_Type BSP_ADC2_Stop(void)
{
  return HAL_ADC_Stop_IT(&hadc2) == HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

uint16_t BSP_ADC2_Get_Value(void)
{
  int8_t i;
  uint32_t val = 0;

  for(i = 0 ; i < ADC2_VALUE_SIZE ; i ++) {
    val += ADC2ValueArray[i];
  }
  
  return val / 16;
}

/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN4
    */
    GPIO_InitStruct.Pin = BATTERY_MON_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(BATTERY_MON_GPIO_Port, &GPIO_InitStruct);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, BSP_ADC_IRQ_PRIORITY, BSP_ADC_IRQ_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
  else if(hadc->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PA5     ------> ADC2_IN5
    */
    GPIO_InitStruct.Pin = LIGHT_MON_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(LIGHT_MON_GPIO_Port, &GPIO_InitStruct);

    /* ADC2 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, BSP_ADC_IRQ_PRIORITY, BSP_ADC_IRQ_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }

}

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN4
    */
    HAL_GPIO_DeInit(BATTERY_MON_GPIO_Port, BATTERY_MON_Pin);

    /* ADC1 interrupt DeInit */
  /* USER CODE BEGIN ADC1:ADC1_2_IRQn disable */
    /**
    * Uncomment the line below to disable the "ADC1_2_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
   HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
  /* USER CODE END ADC1:ADC1_2_IRQn disable */
  
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
  else if(hadc->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC2_CLK_DISABLE();

    /**ADC2 GPIO Configuration
    PA5     ------> ADC2_IN5
    */
    HAL_GPIO_DeInit(LIGHT_MON_GPIO_Port, LIGHT_MON_Pin);

    /* ADC2 interrupt DeInit */
  /* USER CODE BEGIN ADC2:ADC1_2_IRQn disable */
    /**
    * Uncomment the line below to disable the "ADC1_2_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
  HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
  /* USER CODE END ADC2:ADC1_2_IRQn disable */

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
}

/**
  * @brief This function handles ADC1 and ADC2 global interrupts.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  HAL_ADC_IRQHandler(&hadc2);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */
  /* USER CODE END ADC1_2_IRQn 1 */
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  if(hadc == &hadc1) {
    ADC1ValueArray[ADC1ValueIndex] = HAL_ADC_GetValue(&hadc1);
    ADC1ValueIndex = (ADC1ValueIndex + 1) % ADC1_VALUE_SIZE;
  } else if(hadc == &hadc2) {
    ADC2ValueArray[ADC2ValueIndex] = HAL_ADC_GetValue(&hadc2);
    ADC2ValueIndex = (ADC2ValueIndex + 1) % ADC1_VALUE_SIZE;
  }
}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
  if(hadc == &hadc1) {
    IVDBG("HAL_ADC_LevelOutOfWindowCallback %d", HAL_ADC_GetValue(&hadc1));
    power_scan();
  }
}
