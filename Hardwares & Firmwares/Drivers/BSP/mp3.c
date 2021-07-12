#include "mp3.h"
#include "bsp.h"
#include "debug.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

#define BSP_MP3_MAX_DIR_INDEX 99
#define BSP_MP3_MIN_DIR_INDEX 1

#define BSP_MP3_MAX_FILE_INDEX 255
#define BSP_MP3_MIN_FILE_INDEX 1

#define BSP_MP3_MAX_TRACK_INDEX 2999
#define BSP_MP3_MIN_TRACK_INDEX 0

#define BSP_MP3_MAX_VOLUME  30
#define BSP_MP3_MIN_VOLUME  0

#define BSP_MP3_MAX_RESET_WAIT_MS 4000


#define BSP_MP3_DEV_MASK_U      1
#define BSP_MP3_DEV_MASK_TF     2
#define BSP_MP3_DEV_MASK_PC     4
#define BSP_MP3_DEV_MASK_FLASH  8

static BSP_MP3_Msg_Type BSP_MP3_Cmd;

void BSP_MP3_Dump_Msg(BSP_MP3_Msg_Type * Msg)
{
  IVDBG("msg: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
    Msg->Signature,
    Msg->Version,
    Msg->Length,
    Msg->Command,
    Msg->Feedback,  
    Msg->DH,
    Msg->DL,
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
static uint16_t BSP_MP3_Cal_Checksum(BSP_MP3_Msg_Type* Msg)
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

static void BSP_MP3_Fill_Checksum(BSP_MP3_Msg_Type* Msg)
{
  uint8_t * cmd = (uint8_t *)Msg;
  uint16_t checksum = BSP_MP3_Cal_Checksum(Msg);
  cmd[7] = (uint8_t)(checksum >> 8); 
  cmd[8] = (uint8_t)(checksum);   
}

static bool BSP_MP3_Check_Checksum(BSP_MP3_Msg_Type* Msg)
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

static bool BSP_MP3_Wait_Response(BSP_MP3_Msg_Type * Msg)
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
    if(index >= sizeof(BSP_MP3_Msg_Type))
      break;
  }
  
  if(err != BSP_ERROR_NONE || index != sizeof(BSP_MP3_Msg_Type))
    return FALSE;
  
  IVDBG("BSP_MP3_Wait_Response receive new msg");
  BSP_MP3_Dump_Msg(Msg);
  
  if(!BSP_MP3_Check_Checksum(Msg)) {
    IVERR("checksum error");
    return FALSE;
  }
  return TRUE;
}

static bool BSP_MP3_Send_Message(BSP_MP3_Msg_Type * Msg)
{
  IVDBG("BSP_MP3_Send_Message send new msg");
  BSP_MP3_Fill_Checksum(&BSP_MP3_Cmd);
  BSP_MP3_Dump_Msg(Msg);
  BSP_USART2_Transmit((uint8_t *)&BSP_MP3_Cmd, sizeof(BSP_MP3_Cmd));
  memset(&BSP_MP3_Cmd, 0, sizeof(BSP_MP3_Cmd));
  if(BSP_MP3_Wait_Response(&BSP_MP3_Cmd)) {
    if(BSP_MP3_Cmd.Command != BSP_MP3_RES_REPLY) {
      IVERR("BSP_MP3_Send_Message failed!");
      return FALSE;
    }
  }
  return TRUE;
}

static void BSP_MP3_Fill_Msg(BSP_MP3_Msg_Type * Msg)
{
  Msg->Signature = 0x7E;
  Msg->Version   = 0xFF;   
  Msg->Length    = 6;
  Msg->Command   = 0;
  Msg->Feedback  = 1;
  Msg->DH  = 0;
  Msg->DL  = 0; 
  Msg->ChecksumHi = 0;
  Msg->ChecksumLow = 0;  
  Msg->End        = 0xEF;  
}

bool BSP_MP3_Reset(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_RESET;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Standby(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_STANDBY;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Wakeup(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_NORMAL;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Next_Track(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_NEXT_TRACK;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Prev_Track(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_PREV_TRACK;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Play_Track(uint16_t Track)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_PLAY_TRACK;
  if(Track > BSP_MP3_MAX_TRACK_INDEX) Track = BSP_MP3_MAX_TRACK_INDEX;
  BSP_MP3_Cmd.DH = (Track & 0xFF00) >> 8;
  BSP_MP3_Cmd.DH = Track & 0xFF;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Inc_Volume(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_INC_VOL;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Dec_Volume(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_DEC_VOL;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Set_Volume(uint8_t Volume)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_DEC_VOL;
  if(Volume > BSP_MP3_MAX_VOLUME) Volume = BSP_MP3_MAX_VOLUME;
  BSP_MP3_Cmd.DL = Volume;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Set_Eq(BSP_MP3_Eq_Type Eq)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_SET_EQ;
  BSP_MP3_Cmd.DL = Eq;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Set_Mode(BSP_MP3_Mode_Type Mode)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_SET_MODE;
  BSP_MP3_Cmd.DL = Mode;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Set_Dev(BSP_MP3_Dev_Type Dev)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_SET_DEV;
  BSP_MP3_Cmd.DL = Dev;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Set_Repeat(bool repeat)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_REPEAT;
  BSP_MP3_Cmd.DL = repeat ? 1 : 0;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Play(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_PLAY;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Pause(void)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_PAUSE;
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

bool BSP_MP3_Select_File(uint16_t Dir, uint16_t File)
{
  BSP_MP3_Fill_Msg(&BSP_MP3_Cmd);
  BSP_MP3_Cmd.Command   = BSP_MP3_CMD_SET_TARGET;
  if(Dir > BSP_MP3_MAX_DIR_INDEX) Dir = BSP_MP3_MAX_DIR_INDEX;
  if(Dir < BSP_MP3_MIN_DIR_INDEX) Dir = BSP_MP3_MIN_DIR_INDEX;  
  if(File > BSP_MP3_MAX_FILE_INDEX) Dir = BSP_MP3_MAX_FILE_INDEX;
  if(File < BSP_MP3_MIN_FILE_INDEX) Dir = BSP_MP3_MIN_FILE_INDEX;  
  BSP_MP3_Cmd.DH = Dir;
  BSP_MP3_Cmd.DL = File;  
  return BSP_MP3_Send_Message(&BSP_MP3_Cmd);
}

BSP_Error_Type BSP_MP3_Init(void)
{
  uint32_t ResetSaveMS;
  
  while(1) {
    if(!BSP_MP3_Reset()) {
      IVERR("BSP_MP3_Reset Error!");
      return BSP_ERROR_INTERNAL;
    } else {
      ResetSaveMS = HAL_GetTick();
      while(!BSP_MP3_Wait_Response(&BSP_MP3_Cmd)){
        if(((uint32_t)(HAL_GetTick() - ResetSaveMS)) > BSP_MP3_MAX_RESET_WAIT_MS) {
          IVERR("BSP_MP3_Init: wait online msg time out!");
          return BSP_ERROR_INTERNAL; 
        }
      }
      IVINFO("MP3 Dev Online: U[%d]|TF[%d]|PC[%d]|Flash[%d]",
        BSP_MP3_Cmd.DL & BSP_MP3_DEV_MASK_U ? 1 : 0,
        BSP_MP3_Cmd.DL & BSP_MP3_DEV_MASK_TF ? 1 : 0,
        BSP_MP3_Cmd.DL & BSP_MP3_DEV_MASK_PC ? 1 : 0, 
        BSP_MP3_Cmd.DL & BSP_MP3_DEV_MASK_FLASH ? 1 : 0      
      );
      if(!(BSP_MP3_Cmd.DL & BSP_MP3_DEV_MASK_TF)) {
        IVERR("BSP_MP3_Init: no TF found!");
        return BSP_ERROR_INTERNAL;
      }
    }
  }
 
  if(!BSP_MP3_Set_Dev(BSP_MP3_DEV_TF)) {
    IVERR("BSP_MP3_Init: can not set dev to TF");
    return BSP_ERROR_INTERNAL;
  }
  
  if(!BSP_MP3_Set_Mode(BSP_MP3_MODE_SINGAL_REPEAT)) {
    IVERR("BSP_MP3_Init: can not set mode");
    return BSP_ERROR_INTERNAL;
  }
  
  if(!BSP_MP3_Standby()) {
    IVERR("BSP_MP3_Init: can not into standby");
    return BSP_ERROR_INTERNAL;
  }
  
  return BSP_ERROR_NONE;
}