#include <string.h>
#include "iv18.h"
#include "bsp.h"
#include "debug.h"
#include "delay.h"


/*
Out0  G9
Out1  G1
Out2  G3
Out3  G5
Out4  G8
Out5  G7
Out6  G6
Out7  G4
Out8  G2
Out9  a
Out10 dp
Out11 d
Out12 c
Out13 e
Out14 g
Out15 b
Out16 f

   a
f     b
   g
e     c
   d    dp
*/

/*
   -------------|b|scan code|8 dig code
   000000000000000|000000000|0d000000
   b: blink
*/

#define BSP_DP    0x40
#define BSP_BLINK 0x20000

static uint8_t BSP_IV18_Cur_Index;
static uint16_t BSP_IV18_Cur_Loop;
static uint32_t BSP_IV18_Data[9];

static const uint16_t BSP_IV18_Scan_Code[9] = 
{
  0x100, // 100000000 
  0x010, // 000010000 
  0x008, // 000001000
  0x004, // 000000100
  0x020, // 000100000
  0x002, // 000000010
  0x040, // 001000000
  0x001, // 000000001  
  0x080, // 010000000
};

static const uint8_t BSP_IV18_Dig_SPEICAL[] =
{
        /* aPdcegbf */
   0x00, //00000000  BLANK
   0x87, //10000111  o  degree assume ascii 1!! 
   0x82, //10000010  progress 0,ab
   0x06, //00000110  progress 1,bg
   0x0c, //00001100  progress 2,ge 
   0x28, //00101000  progress 3,ed
   0x30, //00110000  progress 4,dc
   0x14, //00010100  progress 5,cg
   0x05, //00000101  progress 6,gf
   0x81, //10000001  progress 7,fa
};

static const uint8_t BSP_IV18_Dig_ASCII[] =
{
        /* aPdcegbf */
   0x04, //00000100  -  ascii: 0x2D
   0x00, //00000000  .
   0x00, //00000000  /
   0xBB, //10111011  0    
   0x12, //00010010  1
   0xAE, //10101110  2
   0xB6, //10110110  3
   0x17, //00010111  4
   0xB5, //10110101  5
   0xBD, //10111101  6
   0x93, //10010011  7
   0xBF, //10111111  8
   0xB7, //10110111  9
   0x00, //00000000  :
   0x00, //00000000  ;
   0x28, //00101000  <
   0x24, //00100100  =  
   0x30, //00110000  >
   0x00, //00000000  ?
   0x00, //00000000  @
   
        /* aPdcegbf */   
   0x9F, //10011111  A
   0x3D, //00111101  B
   0xA9, //10101001  C
   0x3E, //00111110  D
   0xAD, //10101101  E
   0x8D, //10001101  F
   0xB9, //10111001  G
   0x1F, //00011111  H
   0x09, //00001001  I
   0x32, //00110010  J 
   0x00, //00000000  K
   0x29, //00101001  L
   0x15, //00010101  M
   0x9B, //10011011  N
   0x3C, //00111100  O
   0x8F, //10001111  P
   0x00, //10010111  Q
   0x0C, //00001100  R
   0xB5, //10110101  S == 5
   0x2D, //00101101  T
   0x3B, //00111011  U
   0x70, //01110000  V
   0x0E, //00001110  W
   0x00, //00000000  X
   0x0F, //00001111  Y
   0x00, //00000000  Z ascii 0x5A
};

static void BSP_IV18_Show_Data(uint32_t data)
{
  uint8_t i;
  for(i = 0 ; i < 17 ; i ++) {   
    HAL_GPIO_WritePin(IV18_DIN_GPIO_Port, IV18_DIN_Pin, (data & 0x1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IV18_CLK_GPIO_Port, IV18_CLK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(IV18_CLK_GPIO_Port, IV18_CLK_Pin, GPIO_PIN_RESET);    
    data >>= 1;
  }
    HAL_GPIO_WritePin(IV18_LOAD_GPIO_Port, IV18_LOAD_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(IV18_LOAD_GPIO_Port, IV18_LOAD_Pin, GPIO_PIN_RESET);  
};

void BSP_IV18_Scan(void)
{
  uint32_t empty;
  if(BSP_IV18_Data[BSP_IV18_Cur_Index] & BSP_BLINK) {
    if(BSP_IV18_Cur_Loop % 128 < 64) {
      BSP_IV18_Load_Data(BSP_IV18_Cur_Index);
    } else {
      empty = BSP_IV18_Data[BSP_IV18_Cur_Index];
      empty &= 0x1FF40; // keep scan code and dp
      BSP_IV18_Show_Data(empty);
    }
  } else {
    BSP_IV18_Load_Data(BSP_IV18_Cur_Index);
  }
  BSP_IV18_Cur_Index ++;
  BSP_IV18_Cur_Index %= 9;
  if(BSP_IV18_Cur_Index == 8)
    BSP_IV18_Cur_Loop ++;
}

static void BSP_IV18_Dev_Init(void)
{
  uint8_t i, j;
  uint32_t data = 0;
  
  memset(BSP_IV18_Data, 0, sizeof(BSP_IV18_Data));
  
  for(i = 0 ; i < 9 ; i ++)
    BSP_IV18_Set_Dig(i, 0);
  
  BSP_IV18_Set_Dig(2, 'H');
  BSP_IV18_Load_Data(2);
  BSP_IV18_Set_Dig(3, 'E'); 
  BSP_IV18_Load_Data(3);  
  BSP_IV18_Set_Dig(4, 'L');
  BSP_IV18_Load_Data(4);   
  BSP_IV18_Set_Dig(5, 'L'); 
  BSP_IV18_Load_Data(5);   
  BSP_IV18_Set_Dig(6, 'O'); 
  BSP_IV18_Load_Data(6);   
  
  BSP_IV18_Cur_Index = 0;
  BSP_IV18_Cur_Loop = 0;
}

void BSP_IV18_Leave_High_Impedance(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  HAL_GPIO_DeInit(IV18_LOAD_GPIO_Port, IV18_LOAD_Pin);
  HAL_GPIO_DeInit(IV18_CLK_GPIO_Port, IV18_CLK_Pin); 
  HAL_GPIO_DeInit(IV18_DIN_GPIO_Port, IV18_DIN_Pin); 
  
  HAL_GPIO_WritePin(IV18_LOAD_GPIO_Port, IV18_LOAD_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IV18_CLK_GPIO_Port, IV18_CLK_Pin, GPIO_PIN_RESET);  
  HAL_GPIO_WritePin(IV18_DIN_GPIO_Port, IV18_DIN_Pin, GPIO_PIN_RESET);
  
  GPIO_InitStruct.Pin = IV18_LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_LOAD_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = IV18_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_CLK_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = IV18_DIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_DIN_GPIO_Port, &GPIO_InitStruct);  
}

void BSP_IV18_Enter_High_Impedance(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  HAL_GPIO_DeInit(IV18_LOAD_GPIO_Port, IV18_LOAD_Pin);
  HAL_GPIO_DeInit(IV18_CLK_GPIO_Port, IV18_CLK_Pin); 
  HAL_GPIO_DeInit(IV18_DIN_GPIO_Port, IV18_DIN_Pin);   
  
  GPIO_InitStruct.Pin = IV18_LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(IV18_LOAD_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = IV18_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(IV18_CLK_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = IV18_DIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(IV18_DIN_GPIO_Port, &GPIO_InitStruct);  
}

BSP_Error_Type BSP_IV18_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE(); 
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : IV18_BLANK_Pin IV18_LOAD_Pin IV18_CLK_Pin IV18_DIN_Pin */
//  GPIO_InitStruct.Pin = IV18_BLANK_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(IV18_BLANK_GPIO_Port, &GPIO_InitStruct);
  
  /* IV18_BLANK_Pin initialize in tim.c */
  
  HAL_GPIO_WritePin(IV18_LOAD_GPIO_Port, IV18_LOAD_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IV18_CLK_GPIO_Port, IV18_CLK_Pin, GPIO_PIN_RESET);  
  HAL_GPIO_WritePin(IV18_DIN_GPIO_Port, IV18_DIN_Pin, GPIO_PIN_RESET);
  
  GPIO_InitStruct.Pin = IV18_LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_LOAD_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = IV18_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_CLK_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = IV18_DIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IV18_DIN_GPIO_Port, &GPIO_InitStruct);  
  
  BSP_IV18_Dev_Init();
  
  return BSP_ERROR_NONE;
}

void BSP_IV18_Set_Dig(uint8_t index, uint8_t code)
{
  uint32_t mask;
  if(index > 8)
    index = 8;

  mask = BSP_IV18_Data[index] & (BSP_BLINK | BSP_DP);
  
  if(code >= 0x2D) {
    code = code - 0x2D;
    if(code >= sizeof(BSP_IV18_Dig_ASCII))
      code = sizeof(BSP_IV18_Dig_ASCII) - 1;
    BSP_IV18_Data[index] = BSP_IV18_Scan_Code[index];
    BSP_IV18_Data[index] <<= 8;
    BSP_IV18_Data[index] |= BSP_IV18_Dig_ASCII[code];
    BSP_IV18_Data[index] |= mask;    
  } else if(code <= BSP_IV18_SPECIAL_MAX) {
    if(code >= sizeof(BSP_IV18_Dig_SPEICAL))
      code = sizeof(BSP_IV18_Dig_SPEICAL) - 1;
    BSP_IV18_Data[index] = BSP_IV18_Scan_Code[index];
    BSP_IV18_Data[index] <<= 8;
    BSP_IV18_Data[index] |= BSP_IV18_Dig_SPEICAL[code];
    BSP_IV18_Data[index] |= mask;     
  } 
}

void BSP_IV18_Set_DP(uint8_t index)
{
  if(index > 8)
    index = 8;
  
  BSP_IV18_Data[index] |= BSP_DP;
}

void BSP_IV18_Clr_DP(uint8_t index)
{
  if(index > 8)
    index = 8;
  
  BSP_IV18_Data[index] &= ~BSP_DP;  
}

void BSP_IV18_Set_Blink(uint8_t index)
{
  if(index > 8)
    index = 8;
  
  BSP_IV18_Data[index] |= BSP_BLINK;
}

void BSP_IV18_Clr_Blink(uint8_t index)
{
  if(index > 8)
    index = 8;
  
  BSP_IV18_Data[index] &= ~BSP_BLINK;
}

void BSP_IV18_Clr(void)
{
  uint8_t i;
  for(i = 0 ; i < 9 ; i++) {
    BSP_IV18_Data[i] = BSP_IV18_Scan_Code[i];
    BSP_IV18_Data[i] <<= 8; 
  }
}

void BSP_IV18_Clr_Data(uint8_t index)
{
  if(index > 8)
    index = 8;
  BSP_IV18_Data[index] = BSP_IV18_Scan_Code[index];
  BSP_IV18_Data[index] <<= 8;
}

void BSP_IV18_Load_Data(uint8_t index)
{
  if(index > 8)
    index = 8;
  BSP_IV18_Show_Data(BSP_IV18_Data[index]);
}
