#include "i2c.h"
#include "bsp.h"
#include "debug.h"
#include "delay.h"

static I2C_HandleTypeDef hi2c1;


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
//  hi2c1.Init.ClockSpeed = 400000;
//  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
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
  
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;  //此行原有
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;   //GPIO配置为输出
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;      //强上拉
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); 
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  
  // SCL输出脉冲，直到SDA被拉高
  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != GPIO_PIN_SET) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    delay_us(10);
  }
  // SCL拉高
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
  
  delay_ms(10);
  // SDA 输出一个低脉冲
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
  delay_us(10);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  
  hi2c1.Instance->CR1 = I2C_CR1_SWRST;          //复位I2C控制器
  delay_us(10);
  hi2c1.Instance->CR1 = 0;
  
  // 恢复控制器
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  __HAL_RCC_I2C1_CLK_DISABLE();
  __HAL_RCC_I2C1_CLK_ENABLE();
}

#define BSP_I2C_MAX_WAIT_CNT 5000

BSP_Error_Type BSP_I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef ret;
  uint32_t wait_cnt;
  wait_cnt = 0;
  while((ret = HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, MemAddSize, pData, Size, BSP_I2C_SEND_TIMEOUT)) == HAL_BUSY) {
    delay_us(100);
    wait_cnt ++;
    if(wait_cnt > BSP_I2C_MAX_WAIT_CNT && ret == HAL_BUSY) {
      IVERR("BSP_I2C_Write busy too long");
      BSP_I2C_Quit_Busy();
    }
  };
  if(ret != HAL_OK) IVERR("BSP_I2C_Write ret %d", ret);
  return ret ==  HAL_OK ? BSP_ERROR_NONE : BSP_ERROR_INTERNAL;
}

BSP_Error_Type BSP_I2C_Read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef ret;
  uint32_t wait_cnt;
  wait_cnt = 0;
  while((ret = HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, MemAddSize, pData, Size, BSP_I2C_RECV_TIMEOUT)) == HAL_BUSY) {
    delay_us(100);
    wait_cnt ++;
    if(wait_cnt > BSP_I2C_MAX_WAIT_CNT && ret == HAL_BUSY) {
      IVERR("BSP_I2C_Read busy too long");
      BSP_I2C_Quit_Busy();
      break;
    } 
  };
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
