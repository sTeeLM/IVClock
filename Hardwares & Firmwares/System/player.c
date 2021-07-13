#include "player.h"
#include "mp3.h"
#include "clock.h"
#include "config.h"
#include "delay.h"
#include "debug.h"
#include <string.h>

#define PLAYER_MAX_ALARM_INDEX 9

#define PLAYER_DIR_MISC 1
/*
dir 01（misc）:
001: 年
002: 月
003：日
004：时
005：分
006：秒
007: 上午
008: 下午
009：整
010：气温
011：摄氏
012：华氏
013：负
014：点
015：现在时间
*/

#define PLAYER_DIR_NUM1 2
/*
dir 02：
001 ~ 003: 十、百、千
*/

#define PLAYER_DIR_NUM2 3
/*
dir 03:
001 ~ 010: 零-九
*/

#define PLAYER_DIR_ALARM 4
/*
dir 04（闹钟）：
001 ~ 010：闹钟声音
*/

/*
现在时间            1
二 零 一 二         4
年                  1
十 一               2
月                  1
十 一               2
日                  1
上午                1
十 一               2
点                  1
三 十 七            3
分                  1
气温                1
华氏                1
一 百 二 十 三      5
点                  1
三 七               2
度                  1
*/

#define PLAYER_MAX_SEQ_LEN 32

static struct player_seq_node player_seq[PLAYER_MAX_SEQ_LEN]= {
  {1,15,0},
  {3,3,0}, 
  {3,1,0},  
  {3,2,0}, 
  {3,3,0},
  {1,1,0}, 
  {3,8,0},  
  {1,2,0}, 
  {2,1,0},
  {3,4,0},
  {1,3,0},
  {0,0,0},   
};
static uint8_t player_seq_current_index;
static bool player_seq_in_playing;
  

static void player_play_sequence_start(void);
  
void player_init(void)
{
//  memset(player_seq, 0 ,sizeof(player_seq));
  player_seq_current_index = 0;
  player_seq_in_playing = FALSE;
  
  player_on();
  player_play_sequence_start();
}

void player_on(void)
{
//  BSP_MP3_Wakeup();
}

void player_off(void)
{
//  BSP_MP3_Standby();  
}

uint8_t player_get_alarm_index(void)
{
  return config_read("alm0_snd")->val8;
}

void player_set_alarm_index(uint8_t alarm_index)
{
  config_val_t val;
  if(alarm_index > PLAYER_MAX_ALARM_INDEX)
    alarm_index = PLAYER_MAX_ALARM_INDEX;
  val.val8 = alarm_index;
  config_write("alm0_snd", &val);
}

static void player_play_sequence_start(void)
{
  IVDBG("player_play_sequence_start");
  player_seq_in_playing = TRUE;
  player_seq_current_index = 0;
  if(player_seq[0].dir && player_seq[0].file)
    BSP_MP3_Play_Dir_File(player_seq[0].dir, player_seq[0].file);
}

static void player_play_sequence_stop(void)
{
  IVDBG("player_play_sequence_stop");
  BSP_MP3_Stop();
  player_seq_in_playing = FALSE;
  player_seq_current_index = 0;   
}

void player_scan(void)
{
  IVDBG("player_scan");
  if(player_seq_in_playing) {
    player_seq_current_index ++;
    if(player_seq_current_index < PLAYER_MAX_SEQ_LEN) {
      if(player_seq[player_seq_current_index].dir 
        && player_seq[player_seq_current_index].file) { 
          BSP_MP3_Play_Dir_File(player_seq[player_seq_current_index].dir, 
          player_seq[player_seq_current_index].file);
        } else {
          player_play_sequence_stop();
        }
    } else {
      player_play_sequence_stop();
    }
  }
}

void player_play_alarm(void)
{
  
}

void player_report_time(void)
{
  
}
