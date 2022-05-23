#include "i2c.h"
#include "bsp.h"
#include "debug.h"
#include "delay.h"


#ifdef IVCLOCK_BSP_USE_SOFT_I2C


static void I2C_Delay()
{
  delay_us(10);
}

static void I2C_Init()
{
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET); 
  I2C_Delay();
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_SET); 
  I2C_Delay();
}

static void I2C_Start()
{
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_SET); 
  I2C_Delay();
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET); 
  I2C_Delay();
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_RESET);
  I2C_Delay();
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET);
  I2C_Delay();
}

static void I2C_Stop()
{
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_RESET);
  I2C_Delay();
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET);
  I2C_Delay();
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_SET);
  I2C_Delay();
  I2C_Delay();
  I2C_Delay();
  I2C_Delay();
  delay_ms(5);
}

static int I2C_GetAck()
{
  int ack;
  int wait_loop = 256;
  
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_SET);
  I2C_Delay();
  
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET);
  I2C_Delay();
  
  do {
    if(wait_loop == 0) break;
    ack = (HAL_GPIO_ReadPin(I2C_GPIO_Port, I2C_SDA_Pin) == GPIO_PIN_SET);
    I2C_Delay();
    wait_loop --;
  }while(ack);
  
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET);
  I2C_Delay();
  I2C_Delay();
  I2C_Delay();
  I2C_Delay();
  return ack;
}

static void I2C_PutAck(int ack)
{
  
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, ack == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
  I2C_Delay();
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET);
  I2C_Delay();
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET);
  I2C_Delay();
  I2C_Delay();
  I2C_Delay();
  I2C_Delay();
}

static void I2C_Write_Byte(unsigned char dat)
{

  unsigned char t = 8;
  do {
    //I2C_SDA = (bit)(dat & 0x80);
    HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, (dat & 0x80) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    dat <<= 1;
    HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET);
    I2C_Delay();
  } while ( --t != 0 );
}

unsigned char I2C_Read_Byte()
{
  char dat = 0;
  unsigned char t = 8;
  HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_SET);
  do
  {
     HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET);
     I2C_Delay();
     dat <<= 1;
     if ( HAL_GPIO_ReadPin(I2C_GPIO_Port, I2C_SDA_Pin) == GPIO_PIN_SET ) dat |= 0x01;
     HAL_GPIO_WritePin(I2C_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET);
     I2C_Delay();
  } while ( --t != 0 );
  return dat;
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_I2C_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = I2C_SCL_Pin|I2C_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(I2C_GPIO_Port, &GPIO_InitStruct);
  
  I2C_Init();
  
  return BSP_ERROR_NONE;
}

/**
  * @brief I2C1 Denitialization Function
  * @param None
  * @retval None
  */
void BSP_I2C_DeInit(void)
{
  HAL_GPIO_DeInit(I2C_GPIO_Port, I2C_SCL_Pin|I2C_SDA_Pin);
}


BSP_Error_Type BSP_I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  if(pData == NULL || Size == 0)
    return BSP_ERROR_NONE;
  
  I2C_Start();
  
  // send DevAddress
  I2C_Write_Byte((unsigned char)(DevAddress & 0xFE));
  
  if ( I2C_GetAck() ){
    I2C_Stop();
    return BSP_ERROR_TIMEOUT;
  }  
  
  if (MemAddSize == I2C_MEMADD_SIZE_8BIT) {
    I2C_Write_Byte((unsigned char)(MemAddress & 0xFF));
    if ( I2C_GetAck() ){
      I2C_Stop();
      return BSP_ERROR_TIMEOUT;
    }      
  } else {
    I2C_Write_Byte((unsigned char)(MemAddress & 0xFF));
    if ( I2C_GetAck() ){
      I2C_Stop();
      return BSP_ERROR_TIMEOUT;
    }
    I2C_Write_Byte((unsigned char)(((MemAddress & 0xFF00) >> 8) & 0xFF));
    if ( I2C_GetAck() ){
      I2C_Stop();
      return BSP_ERROR_TIMEOUT;
    }    
  }
  
  do{
    I2C_Write_Byte(*pData++);
    if ( I2C_GetAck() )
    {
     I2C_Stop();
     return BSP_ERROR_TIMEOUT;
    }
   } while ( --Size != 0 );
  
   return BSP_ERROR_NONE;
}

BSP_Error_Type BSP_I2C_Read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  if(pData == NULL || Size == 0)
    return BSP_ERROR_NONE;
  I2C_Start();
  
  // send DevAddress
  I2C_Write_Byte((unsigned char)(DevAddress & 0xFE));
  
  if ( I2C_GetAck() ){
    I2C_Stop();
    return BSP_ERROR_TIMEOUT;
  }  
  
  if (MemAddSize == I2C_MEMADD_SIZE_8BIT) {
    I2C_Write_Byte((unsigned char)(MemAddress & 0xFF));
    if ( I2C_GetAck() ){
      I2C_Stop();
      return BSP_ERROR_TIMEOUT;
    }      
  } else {
    I2C_Write_Byte((unsigned char)(MemAddress & 0xFF));
    if ( I2C_GetAck() ){
      I2C_Stop();
      return BSP_ERROR_TIMEOUT;
    }
    I2C_Write_Byte((unsigned char)(((MemAddress & 0xFF00) >> 8) & 0xFF));
    if ( I2C_GetAck() ){
      I2C_Stop();
      return BSP_ERROR_TIMEOUT;
    }    
  }

  I2C_Start(); 
  // send DevAddress
  I2C_Write_Byte((unsigned char)(DevAddress | 0x1)); 
  if ( I2C_GetAck() ){
    I2C_Stop();
    return BSP_ERROR_TIMEOUT;
  }
  
  for (;;){
    *pData++ = I2C_Read_Byte();
    if ( --Size == 0 ) {
     I2C_PutAck(1);
     break;
    }
    I2C_PutAck(0);
  }

  I2C_Stop();
  return BSP_ERROR_NONE;
}

#else

static I2C_HandleTypeDef hi2c1;

static void BSP_I2C_DisableIRQ(void)
{
//  HAL_NVIC_DisableIRQ(INT_KEY_SET_EXTI_IRQn);
//  HAL_NVIC_DisableIRQ(INT_KEY_MOD_EXTI_IRQn); 
//  HAL_NVIC_DisableIRQ(INT_BT_EXTI_IRQn);
//  HAL_NVIC_DisableIRQ(INT_ALARM_EXTI_IRQn); 
//  HAL_NVIC_DisableIRQ(INT_ACC_EXTI_IRQn);  
//  HAL_NVIC_DisableIRQ(INT_MP3_EXTI_IRQn);   
}

static void BSP_I2C_EnableIRQ(void)
{
//  HAL_NVIC_EnableIRQ(INT_KEY_SET_EXTI_IRQn);
//  HAL_NVIC_EnableIRQ(INT_KEY_MOD_EXTI_IRQn); 
//  HAL_NVIC_EnableIRQ(INT_BT_EXTI_IRQn);
//  HAL_NVIC_EnableIRQ(INT_ALARM_EXTI_IRQn); 
//  HAL_NVIC_EnableIRQ(INT_ACC_EXTI_IRQn);  
//  HAL_NVIC_EnableIRQ(INT_MP3_EXTI_IRQn);  
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
BSP_Error_Type BSP_I2C_Init(void)
{
  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
//  hi2c1.Init.ClockSpeed = 100000;
//  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; 
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    return BSP_ERROR_INTERNAL;
  }
  /* USER CODE BEGIN I2C1_Init 2 */
  /* USER CODE END I2C1_Init 2 */
  return BSP_ERROR_NONE;
}

/**
  * @brief I2C1 Denitialization Function
  * @param None
  * @retval None
  */
void BSP_I2C_DeInit(void)
{
  HAL_I2C_DeInit(&hi2c1);
}

static void BSP_I2C_Quit_Busy(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /**I2C1 GPIO Configuration
  PB6     ------> I2C1_SCL
  PB7     ------> I2C1_SDA
  */
  IVDBG("enter BSP_I2C_Quit_Busy");
  
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;  //此行原有
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;   //GPIO配置为输出
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;      //强上拉
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); 
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  
  IVDBG("wait I2C1_SDA up...");
  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != GPIO_PIN_SET) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    delay_us(10);
  }
  IVDBG("done!");
  
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
  
  delay_ms(10);
  // SDA 输出一个低脉冲
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
  delay_us(10);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); 
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);  
  
  delay_ms(10);
  I2C1->CR1 = I2C_CR1_SWRST;
  delay_ms(10);
  I2C1->CR1 = 0;
  
  __HAL_RCC_I2C1_FORCE_RESET();
  
  // 恢复控制器
  IVDBG("reinit!");
  BSP_I2C_DeInit();
  delay_ms(10);
  BSP_I2C_Init();
//  I2C1->CR1 = I2C_INIT_SAVE_CR1;
//  I2C1->CR2 = I2C_INIT_SAVE_CR2; 
//  I2C1->OAR1 = I2C_INIT_SAVE_OAR1;
//  I2C1->OAR2 = I2C_INIT_SAVE_OAR2;
//  I2C1->CCR = I2C_INIT_SAVE_CCR; 
//  I2C1->TRISE = I2C_INIT_SAVE_TRISE; 
//  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  IVDBG("done!");
}

#define BSP_I2C_MAX_WAIT_CNT 5000

BSP_Error_Type BSP_I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef ret;
  uint32_t wait_cnt;
  wait_cnt = 0;
  BSP_I2C_DisableIRQ();
  while((ret = HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, MemAddSize, pData, Size, BSP_I2C_SEND_TIMEOUT)) == HAL_BUSY) {
    delay_us(100);
    wait_cnt ++;
    if(wait_cnt > BSP_I2C_MAX_WAIT_CNT && ret == HAL_BUSY) {
      IVERR("BSP_I2C_Write busy too long");
      BSP_I2C_Quit_Busy();
      delay_ms(10);
    }
  };
  BSP_I2C_EnableIRQ();
  if(ret != HAL_OK) IVERR("BSP_I2C_Write ret %d", ret);
  return ret ==  HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

BSP_Error_Type BSP_I2C_Read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef ret;
  uint32_t wait_cnt;
  wait_cnt = 0;
  BSP_I2C_DisableIRQ();
  while((ret = HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, MemAddSize, pData, Size, BSP_I2C_RECV_TIMEOUT)) == HAL_BUSY) {
    delay_us(100);
    wait_cnt ++;
    if(wait_cnt > BSP_I2C_MAX_WAIT_CNT && ret == HAL_BUSY) {
      IVERR("BSP_I2C_Read busy too long");
      BSP_I2C_Quit_Busy();
      delay_ms(10);
    } 
  };
  BSP_I2C_EnableIRQ();
  if(ret != HAL_OK) IVERR("BSP_I2C_Read ret %d", ret);
  return ret ==  HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}


/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  int16_t i;
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */
  
  /* USER CODE END I2C1_MspInit 0 */
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); 
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    
    for( i = 0 ; i < 100; i ++) {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
      delay_us(10);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
      delay_us(10);
    }
    
    I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0;
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* Peripheral clock enable */
  /* USER CODE BEGIN I2C1_MspInit 1 */ 
    
  /* USER CODE END I2C1_MspInit 1 */
  }
}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }

}

#endif