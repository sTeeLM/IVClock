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

typedef enum BSP_MP3_Dev
{
  BSP_MP3_DEV_U = 0,
  BSP_MP3_DEV_TF,
  BSP_MP3_DEV_AUX,
  BSP_MP3_DEV_SLEEP,
  BSP_MP3_DEV_FLASH
}BSP_MP3_Dev_Type;

typedef enum BSP_MP3_Status
{
  BSP_MP3_STATUS_STOPPED = 0,
  BSP_MP3_STATUS_PLAYING = 1,
  BSP_MP3_STATUS_PAUSED  = 2,
  BSP_MP3_STATUS_SLEEP   = 8
}BSP_MP3_Status_Type;

typedef enum BSP_MP3_Cmd {
  BSP_MP3_CMD_NONE          = 0,
  BSP_MP3_CMD_NEXT_TRACK    = 1,
  BSP_MP3_CMD_PREV_TRACK    = 2,  
  BSP_MP3_CMD_PLAY_TRACK    = 3, //0 - 2999
  BSP_MP3_CMD_INC_VOL       = 4, 
  BSP_MP3_CMD_DEC_VOL       = 5,
  BSP_MP3_CMD_SET_VOL       = 6,    // 0 - 30
  BSP_MP3_CMD_SET_EQ        = 7,    // 0 - 5 (Normal,Pop,Rock,Jazz,Class,Base)
  BSP_MP3_CMD_REPEAT_TRACK  = 8,    // 0 - 3 (Repeat,Folder Repeat,Single Repeat,Random)
  BSP_MP3_CMD_SET_DEV       = 9,    // 0 - 4 (U,TF,Aux,Sleep,Flash)
  BSP_MP3_CMD_STANDBY       = 0x0A,
  BSP_MP3_CMD_NORMAL        = 0x0B,
  BSP_MP3_CMD_RESET         = 0x0C,
  BSP_MP3_CMD_PLAY          = 0x0D,
  BSP_MP3_CMD_PAUSE         = 0x0E,
  BSP_MP3_CMD_PlAY_DIR_FILE = 0x0F, // 1-10
  BSP_MP3_CMD_AUDIO_AMP     = 0x10, // {DH=1: Open volume adjust} {DL: set volume gain 0-31}
  BSP_MP3_CMD_REPEAT_DEV    = 0x11, // 1: start repeat, 0: stop play
  BSP_MP3_CMD_PLAY_DIR_MP3  = 0x12, // Specify playback of folder named "MP3"
  BSP_MP3_CMD_PLAY_DIR_ADVERT = 0x13,   // Insert track in the folder "ADVERT"
  BSP_MP3_CMD_PLAY_HUGE_DIR  = 0x14,
  BSP_MP3_CMD_STOP_ADVERT = 0x15,
  BSP_MP3_CMD_STOP          = 0x16,
  BSP_MP3_CMD_REPEAT_DIR    = 0x17,
  BSP_MP3_CMD_RANDOM_DEV    = 0x18,
  BSP_MP3_CMD_REPEAT_CUR_TRACK  = 0x19,  
  BSP_MP3_CMD_SET_DAC       = 0x1A,
  BSP_MP3_CMD_QUERY_STATUS  = 0x42,
  BSP_MP3_CMD_QUERY_VOL     = 0x43,
  BSP_MP3_CMD_QUERY_EQ      = 0x44,
  BSP_MP3_CMD_QUERY_TRACKS_U = 0x47,
  BSP_MP3_CMD_QUERY_TRACKS_TF = 0x48,
  BSP_MP3_CMD_QUERY_TRACKS_FLASH = 0x49,  
//  BSP_MP3_CMD_KEEP_ON         = 0x4A,
  BSP_MP3_CMD_QUERY_CUR_TRACK_U = 0x4B,
  BSP_MP3_CMD_QUERY_CUR_TRACK_TF = 0x4C,
  BSP_MP3_CMD_QUERY_CUR_TRACK_FLASH = 0x4D,  
  BSP_MP3_CMD_QUERY_FILE_IN_DIR = 0x4E,
  BSP_MP3_CMD_QUERY_DIR_IN_DEV  = 0x4F,
  
}BSP_MP3_Cmd_Type;

typedef enum BSP_MP3_Res {
  BSP_MP3_RES_INIT    = 0x3F,
  BSP_MP3_RES_ERROR   = 0x40,
  BSP_MP3_RES_REPLY   = 0x41,
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
uint8_t BSP_MP3_Get_Max_Volume(void);
uint8_t BSP_MP3_Get_Min_Volume(void);
bool BSP_MP3_Set_Volume(uint8_t Volume);
bool BSP_MP3_Set_Eq(BSP_MP3_Eq_Type Eq);
bool BSP_MP3_Set_Dev(BSP_MP3_Dev_Type Dev);
bool BSP_MP3_Set_Audio_AMP(bool Enable, uint8_t Gain);
bool BSP_MP3_Set_DAC(bool On);

bool BSP_MP3_Play(void);
bool BSP_MP3_Pause(void);
bool BSP_MP3_Stop(void);

bool BSP_MP3_Play_Dir_File(uint8_t Dir, uint8_t File);
bool BSP_MP3_Play_MP3_Dir_File(uint16_t File);
bool BSP_MP3_Play_ADVERT_Dir_File(uint16_t File);
bool BSP_MP3_Stop_Advert(void);
bool BSP_MP3_Play_Huge_Dir_File(uint8_t Dir, uint16_t File);
bool BSP_MP3_Repeat_Dev(bool Start);
bool BSP_MP3_Repeat_Dir(uint8_t Dir);
bool BSP_MP3_Repeat_File(uint16_t File);
bool BSP_MP3_Random_Dev(void);
bool BSP_MP3_Repeat_Current_Track(bool On);

bool BSP_MP3_Query_Status(BSP_MP3_Status_Type * Status);
bool BSP_MP3_Query_Volume(uint8_t * Volume);
bool BSP_MP3_Query_Eq(BSP_MP3_Eq_Type * Eq);

#endif