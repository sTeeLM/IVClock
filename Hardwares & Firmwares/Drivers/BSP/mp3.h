#ifndef __IVCLOCK_MP3_H__
#define __IVCLOCK_MP3_H__

#include <stdint.h>
#include "bsp.h"
#include "cext.h"


typedef struct BSP_MP3_Msg {
  uint8_t Signature;  // 7E
  uint8_t Version;  // FF
  uint8_t Length;  // lenth of cmd - sig - ver
  uint8_t Command;
  uint8_t Feedback; 
  uint8_t DH;
  uint8_t DL; 
  uint8_t ChecksumHi;
  uint8_t ChecksumLow;  
  uint8_t End;  // EF
}BSP_MP3_Msg_Type;

typedef enum BSP_MP3_Eq
{
  BSP_MP3_EQ_NORMAL = 0,
  BSP_MP3_EQ_POP,
  BSP_MP3_EQ_ROCK,
  BSP_MP3_EQ_JAZZ,
  BSP_MP3_EQ_CLASSICAL,
  BSP_MP3_EQ_BASE
}BSP_MP3_Eq_Type;

typedef enum BSP_MP3_Mode
{
  BSP_MP3_MODE_REPEAT = 0,
  BSP_MP3_MODE_FOLDER_REPEAT,
  BSP_MP3_MODE_SINGAL_REPEAT,
  BSP_MP3_MODE_RANDOM
}BSP_MP3_Mode_Type;

typedef enum BSP_MP3_Dev
{
  BSP_MP3_DEV_U = 0,
  BSP_MP3_DEV_TF,
  BSP_MP3_DEV_AUX,
  BSP_MP3_DEV_SLEEP,
  BSP_MP3_DEV_FLASH
}BSP_MP3_Dev_Type;

typedef enum BSP_MP3_Cmd {
  BSP_MP3_CMD_NONE          = 0,
  BSP_MP3_CMD_NEXT_TRACK    = 1,
  BSP_MP3_CMD_PREV_TRACK    = 2,  
  BSP_MP3_CMD_PLAY_TRACK    = 3, //0 - 2999
  BSP_MP3_CMD_INC_VOL       = 4, 
  BSP_MP3_CMD_DEC_VOL       = 5,
  BSP_MP3_CMD_SET_VOL       = 6,    // 0 - 30
  BSP_MP3_CMD_SET_EQ        = 7,     // 0 - 5 (Normal,Pop,Rock,Jazz,Class,Base)
  BSP_MP3_CMD_SET_MODE      = 8,   // 0 - 3 (Repeat,Folder Repeat,Single Repeat,Random)
  BSP_MP3_CMD_SET_DEV       = 9,    // 0 - 4 (U,TF,Aux,Sleep,Flash)
  BSP_MP3_CMD_STANDBY       = 0x0A,
  BSP_MP3_CMD_NORMAL        = 0x0B,
  BSP_MP3_CMD_RESET         = 0x0C,
  BSP_MP3_CMD_PLAY          = 0x0D,
  BSP_MP3_CMD_PAUSE         = 0x0E,
  BSP_MP3_CMD_SET_TARGET    = 0x0F, // 1-10
  BSP_MP3_CMD_VOL_ADJ_SET   = 0x10, // {DH=1: Open volume adjust} {DL: set volume gain 0-31}
  BSP_MP3_CMD_REPEAT        = 0x11, // 1: start repeat, 0: stop play
}BSP_MP3_Cmd_Type;

typedef enum BSP_MP3_Res {
  BSP_MP3_RES_PUSHIN  = 0x3A,
  BSP_MP3_RES_PUSHOUT = 0x3B,
  BSP_MP3_RES_STAY0   = 0x3C,
  BSP_MP3_RES_STAY1   = 0x3D,
  BSP_MP3_RES_STAY2   = 0x3E,
  BSP_MP3_RES_INIT    = 0x3F, 
  BSP_MP3_RES_ERROR   = 0x40,
  BSP_MP3_RES_REPLY   = 0x41,
  BSP_MP3_RES_STATUS  = 0x42,
  BSP_MP3_RES_VOL     = 0x43,
  BSP_MP3_RES_EQ      = 0x44,
  BSP_MP3_RES_MODE    = 0x45,
  BSP_MP3_RES_VER     = 0x46,
  BSP_MP3_RES_TF_FILES       = 0x47,
  BSP_MP3_RES_U_FILES        = 0x48,
  BSP_MP3_RES_FLASH_FILES    = 0x49,
  BSP_MP3_RES_KEEPON         = 0x4A,
  BSP_MP3_RES_TF_TRACKS      = 0x4B,
  BSP_MP3_RES_U_TRACKS       = 0x4C,
  BSP_MP3_RES_FLASH_TRACKS   = 0x4D
}BSP_MP3_Res_Type;


BSP_Error_Type BSP_MP3_Init(void);
bool BSP_MP3_Reset(void);
bool BSP_MP3_Standby(void);
bool BSP_MP3_Wakeup(void);
bool BSP_MP3_Next_Track(void);
bool BSP_MP3_Prev_Track(void);
bool BSP_MP3_Play_Track(uint16_t Track);
bool BSP_MP3_Inc_Volume(void);
bool BSP_MP3_Dec_Volume(void);
bool BSP_MP3_Set_Volume(uint8_t Volume);
bool BSP_MP3_Set_Eq(BSP_MP3_Eq_Type Eq);
bool BSP_MP3_Set_Mode(BSP_MP3_Mode_Type Mode);
bool BSP_MP3_Set_Dev(BSP_MP3_Dev_Type Dev);
bool BSP_MP3_Set_Repeat(bool repeat);
bool BSP_MP3_Play(void);
bool BSP_MP3_Pause(void);
bool BSP_MP3_Select_File(uint16_t Dir, uint16_t File);

#endif