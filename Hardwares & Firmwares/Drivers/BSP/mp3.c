#include "mp3.h"
#include "bsp.h"
#include "debug.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

static struct BSP_MP3_Msg _Cmd;
static struct BSP_MP3_Msg _Res;
static uint8_t _Buf;
static uint8_t _Index;

void BSP_MP3_Dump_Msg(struct BSP_MP3_Msg* Msg)
{
  IVDBG("msg: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
    Msg->Signature,
    Msg->Version,
    Msg->Length,
    Msg->Command,
    Msg->Feedback,  
    Msg->Parament1,
    Msg->Parament2,
    Msg->ChecksumHi,
    Msg->ChecksumLow,
    Msg->End 
  );
}

//
// for (int i=1; i<7; i++) {
//		checksum += cmd[i];
// uint16_t leftsum = 0x010000 - checksum
// cmd[7] = (uint8_t)(leftsum >> 8);
// cmd[8] = (uint8_t)(leftsum);
// 7E FF 06 03 00 00 01 FF E6 EF
uint16_t BSP_MP3_Cal_Checksum(struct BSP_MP3_Msg* Msg)
{
  uint8_t * cmd = (uint8_t *)Msg;
  uint8_t i;
  uint16_t checksum = 0, leftsum;
  
  for (int i=1; i<7; i++) {
      checksum += cmd[i];
  }
  
  leftsum = 0x010000 - checksum;
  
  return leftsum; 
}

void BSP_MP3_Fill_Checksum(struct BSP_MP3_Msg* Msg)
{
  uint8_t * cmd = (uint8_t *)Msg;
  uint16_t checksum = BSP_MP3_Cal_Checksum(Msg);
  cmd[7] = (uint8_t)(checksum >> 8); 
  cmd[8] = (uint8_t)(checksum);   
}

bool BSP_MP3_Check_Checksum(struct BSP_MP3_Msg* Msg)
{
  uint8_t * cmd = (uint8_t *)Msg;
  uint16_t checksum = BSP_MP3_Cal_Checksum(Msg);
  if((cmd[7] != (uint8_t)(checksum >> 8)) ||
    (cmd[8] != (uint8_t)(checksum))) 
  {
    return FALSE;
  }
  return TRUE;
}

void BSP_MP3_Reset(void)
{
  _Cmd.Signature = 0x7E;
  _Cmd.Version   = 0xFF;   
  _Cmd.Length    = 6;
  _Cmd.Command   = BSP_MP3_CMD_RESET;
  _Cmd.Feedback  = 1;
  _Cmd.Parament1  = 0;
  _Cmd.Parament2  = 0; 
  _Cmd.End        = 0xEF;
  BSP_MP3_Cal_Checksum(&_Cmd);
  BSP_MP3_Dump_Msg(&_Cmd);
  BSP_USART2_Transmit((uint8_t *)&_Cmd, sizeof(_Cmd));
  
  delay_ms(10); 
  memset(&_Cmd, 0 , sizeof(_Cmd));
  BSP_USART2_Receive((uint8_t *)&_Cmd, sizeof(_Cmd)); 
  BSP_MP3_Dump_Msg(&_Cmd);
}

void BSP_MP3_Receive_Byte(void)
{
  uint8_t * res = (uint8_t *) &_Res;
  if(_Buf == 0x7E && _Index == 0) { // a new mesg!
    ;
  }
  
  res[_Index++] = _Buf;
  
  if(_Index == sizeof(_Buf)) { // drop some bytes
    _Index = 0;
  }
  
  if(_Buf == 0xEF) { // a whole mesg!
    _Index = 0;
    IVDBG("receive a new message!");
    BSP_MP3_Dump_Msg(&_Res);
    if(BSP_MP3_Check_Checksum(&_Res)) {
      
    }
  }
}

BSP_Error_Type BSP_MP3_Init(void)
{
  _Index = 0;
  BSP_USART2_Set_Receive_CB(BSP_MP3_Receive_Byte);
  BSP_USART2_Receive_IT((uint8_t *)&_Buf, 1);
  
  BSP_MP3_Reset();  
 
  return BSP_ERROR_NONE;
}