#include "player.h"
#include "mp3.h"
#include "clock.h"
#include "config.h"

#define PLAYER_MAX_ALARM_INDEX 9

#define PLAYER_DIR_MISC 1
/*
dir 1（misc）:
001: 年
002: 月
003：日
004：时
005：分
006：秒
007: 上午
008: 下午
009：整
010: 二零
011：气温
012：摄氏
013：负
014：点
*/

#define PLAYER_DIR_NUM1 2
/*
dir 2（年、气温小数）:
001 ~ 100: 零零-九九
*/

#define PLAYER_DIR_NUM2 3
/*
dir 3（月日时分秒）:
001 ~ 100: 零-九十九
*/

#define PLAYER_DIR_ALARM 4
/*
dir 4（闹钟）：
001 ~ 010：闹钟声音
*/




void player_init(void)
{
  
}

void player_on(void)
{
  BSP_MP3_Wakeup();
}

void player_off(void)
{
  BSP_MP3_Standby();  
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

static void player_play_sequence(void)
{
  
}

void player_play_alarm(void)
{
  
}

void player_report_time(void)
{
  
}
