#include "mp3.h"
#include "bsp.h"
#include "debug.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

static struct BSP_MP3_Msg _Cmd;

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

bool BSP_MP3_Wait_Response(struct BSP_MP3_Msg * Msg)
{
  uint8_t * p;
  uint8_t buf, index;
  BSP_Error_Type err;
  
  index = 0;
  p = (uint8_t *)Msg;
  while( (err = BSP_USART2_Receive(&buf, 1) ) == BSP_ERROR_NONE) {
    if(buf == 0x7E && index != 0 && (p[0] != 0x7E || p[1] != 0xFF)) {
      index = 0;
    }
    p[index ++] = buf;
    if(index >= sizeof(struct BSP_MP3_Msg))
      break;
  }
  
  if(err != BSP_ERROR_NONE || index != sizeof(struct BSP_MP3_Msg))
    return FALSE;
  
  IVDBG("BSP_MP3_Wait_Response receive new msg");
  BSP_MP3_Dump_Msg(Msg);
  
  if(!BSP_MP3_Check_Checksum(Msg)) {
    IVERR("checksum error");
    return FALSE;
  }
  return TRUE;
}

BSP_Error_Type BSP_MP3_Send_Message(struct BSP_MP3_Msg* Msg)
{
  IVDBG("BSP_MP3_Send_Message send new msg");
  BSP_MP3_Fill_Checksum(&_Cmd);
  BSP_MP3_Dump_Msg(Msg);
  BSP_USART2_Transmit((uint8_t *)&_Cmd, sizeof(_Cmd));
  memset(&_Cmd, 0, sizeof(_Cmd));
  if(BSP_MP3_Wait_Response(&_Cmd)) {
    if(_Cmd.Command != BSP_MP3_CMD_QUERY_REPLY) {
      IVERR("BSP_MP3_Send_Message failed!");
      return BSP_ERROR_INTERNAL;
    }
  }
  return BSP_ERROR_NONE;
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
  
  BSP_MP3_Send_Message(&_Cmd);
}



BSP_Error_Type BSP_MP3_Init(void)
{

  BSP_MP3_Reset();
 
  while(!BSP_MP3_Wait_Response(&_Cmd)){
    delay_ms(10);
  }
  
  return BSP_ERROR_NONE;
}