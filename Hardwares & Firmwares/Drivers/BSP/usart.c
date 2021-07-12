#include "usart.h"
#include "bsp.h"
#include "debug.h"
#include "delay.h"

static UART_HandleTypeDef huart1; // debug
static UART_HandleTypeDef huart2; // MP3
static UART_HandleTypeDef huart3; // Blue Tooth
static uint8_t BSP_USART2_Tx_Cplt;
static uint8_t BSP_USART2_Rx_Cplt;
static uint8_t BSP_USART2_Error;
static USART_TRANSMIT_CPLT_CB BSP_USART2_Tx_Cplt_CB;
static USART_RECEIVE_CPLT_CB BSP_USART2_Rx_Cplt_CB;
/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
  return BSP_ERROR_NONE;
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_USART2_UART_Init(void)
{
  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
  return BSP_ERROR_NONE;
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_USART3_UART_Init(void)
{
  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */
  return BSP_ERROR_NONE;
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_MspInit 1 */
    HAL_NVIC_SetPriority(USART2_IRQn, BSP_USART2_IRQ_PRIORITY, BSP_USART2_IRQ_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE END USART2_MspInit 1 */
  }
  else if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

  /* USER CODE BEGIN USART2_MspDeInit 1 */
  HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }

}

BSP_Error_Type BSP_USART1_Transmit(uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef res = HAL_UART_Transmit(&huart1, pData, Size, BSP_USART1_TRANSMIT_TIMEOUT);
  BSP_Error_Type ret = BSP_ERROR_INTERNAL;
  switch(res) {
    case HAL_ERROR:
      ret = BSP_ERROR_INTERNAL;
      break;
    case HAL_TIMEOUT:
      ret = BSP_ERROR_TIMEOUT;
      break;
    case HAL_BUSY:
      ret = BSP_ERROR_BUSY;
      break;
    case HAL_OK:
      ret = BSP_ERROR_NONE;
      break;
  }
  return ret;
}

BSP_Error_Type BSP_USART1_Receive(uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef res = HAL_UART_Receive(&huart1, pData, Size, BSP_USART1_RECEIVE_TIMEOUT);
  BSP_Error_Type ret = BSP_ERROR_INTERNAL;
  switch(res) {
    case HAL_ERROR:
      ret = BSP_ERROR_INTERNAL;
      break;
    case HAL_TIMEOUT:
      ret = BSP_ERROR_TIMEOUT;
      break;
    case HAL_BUSY:
      ret = BSP_ERROR_BUSY;
      break;
    case HAL_OK:
      ret = BSP_ERROR_NONE;
      break;
  }
  return ret;
}

int16_t BSP_USART1_Try_Get_Char(void)
{
  uint8_t ch = 0;
  HAL_StatusTypeDef res = HAL_UART_Receive(&huart1, &ch, 1, 0);
  if(res != HAL_OK) {
    return 0;
  } else {
    return ch;
  }
}

int16_t BSP_USART1_Get_Char(void)
{
  uint8_t ch = 0;
  HAL_StatusTypeDef res;
  while((res = HAL_UART_Receive(&huart1, &ch, 1, BSP_USART1_RECEIVE_TIMEOUT)) == HAL_TIMEOUT) {
    delay_us(10);
  }
  if(res == HAL_OK) {
    return ch;
  } else {
    return -1 ;
  }
}


BSP_Error_Type BSP_USART2_Transmit(uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef res = HAL_UART_Transmit(&huart2, pData, Size, BSP_USART2_TRANSMIT_TIMEOUT);
  BSP_Error_Type ret = BSP_ERROR_INTERNAL;
  switch(res) {
    case HAL_ERROR:
      ret = BSP_ERROR_INTERNAL;
      break;
    case HAL_TIMEOUT:
      ret = BSP_ERROR_TIMEOUT;
      break;
    case HAL_BUSY:
      ret = BSP_ERROR_BUSY;
      break;
    case HAL_OK:
      ret = BSP_ERROR_NONE;
      break;
  }
  return ret;
}

BSP_Error_Type BSP_USART2_Receive(uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef res = HAL_UART_Receive(&huart2, pData, Size, BSP_USART2_RECEIVE_TIMEOUT);
  BSP_Error_Type ret = BSP_ERROR_INTERNAL;
  switch(res) {
    case HAL_ERROR:
      ret = BSP_ERROR_INTERNAL;
      break;
    case HAL_TIMEOUT:
      ret = BSP_ERROR_TIMEOUT;
      break;
    case HAL_BUSY:
      ret = BSP_ERROR_BUSY;
      break;
    case HAL_OK:
      ret = BSP_ERROR_NONE;
      break;
  }
  return ret;
}


BSP_Error_Type BSP_USART2_Transmit_IT(uint8_t *pData, uint16_t Size)
{
  BSP_Error_Type ret = BSP_ERROR_INTERNAL;
  HAL_StatusTypeDef res;
  
  BSP_USART2_Tx_Cplt = 0;
  BSP_USART2_Error   = 0;  
  res = HAL_UART_Transmit_IT(&huart1, pData, Size);
  switch(res) {
    case HAL_ERROR:
      ret = BSP_ERROR_INTERNAL;
      break;
    case HAL_BUSY:
      ret = BSP_ERROR_BUSY;
      break;
    case HAL_OK:
      ret = BSP_ERROR_NONE;
      break;
    default:;
  }
  return ret;
}

BSP_Error_Type BSP_USART2_Receive_IT(uint8_t *pData, uint16_t Size)
{
  BSP_Error_Type ret = BSP_ERROR_INTERNAL;
  HAL_StatusTypeDef res;
  
  BSP_USART2_Rx_Cplt = 0;
  BSP_USART2_Error   = 0;
  res = HAL_UART_Receive_IT(&huart1, pData, Size);
  switch(res) {
    case HAL_ERROR:
      ret = BSP_ERROR_INTERNAL;
      break;
    case HAL_BUSY:
      ret = BSP_ERROR_BUSY;
      break;
    case HAL_OK:
      ret = BSP_ERROR_NONE;
      break;
    default:;
  }
  return ret;
}

void BSP_USART2_Set_Transmit_CB(USART_TRANSMIT_CPLT_CB cb)
{
  BSP_USART2_Tx_Cplt_CB = cb;
}

void BSP_USART2_Set_Receive_CB(USART_RECEIVE_CPLT_CB cb)
{
  BSP_USART2_Rx_Cplt_CB = cb;
}

BSP_Error_Type BSP_USART2_Wait_Transmit_Cplt(uint16_t Timeout)
{
  while(BSP_USART2_Tx_Cplt != 1) {
    delay_us(10);
    if(Timeout == 0)
      break;
    Timeout --;    
  }
  if(BSP_USART2_Error)
    return BSP_ERROR_INTERNAL;
  else if(BSP_USART2_Tx_Cplt != 1 && Timeout == 0)
    return BSP_ERROR_TIMEOUT;
  return BSP_ERROR_NONE;  
}

BSP_Error_Type BSP_USART2_Wait_Receive_Cplt(uint16_t Timeout)
{
  while(BSP_USART2_Rx_Cplt != 1) {
    delay_us(10);
    if(Timeout == 0)
      break;
    Timeout --;
  }
  if(BSP_USART2_Error)
    return BSP_ERROR_INTERNAL;
  else if(BSP_USART2_Rx_Cplt != 1 && Timeout == 0)
    return BSP_ERROR_TIMEOUT;  
  return BSP_ERROR_NONE;
}

void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) 
{
  if(huart == &huart2) {
    BSP_USART2_Tx_Cplt = 1;
  }
  if(BSP_USART2_Tx_Cplt_CB)
  {
    BSP_USART2_Tx_Cplt_CB();
  }  
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &huart2) {
    BSP_USART2_Rx_Cplt = 1;
  }
  if(BSP_USART2_Rx_Cplt_CB)
  {
    BSP_USART2_Rx_Cplt_CB();
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  BSP_USART2_Error = 1;
}
