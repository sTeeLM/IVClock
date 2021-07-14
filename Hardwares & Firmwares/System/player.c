#include "player.h"
#include "mp3.h"
#include "clock.h"
#include "config.h"
#include "delay.h"
#include "debug.h"
#include "rtc.h"
#include "console.h"
#include <string.h>

#define PLAYER_DIR_MISC     1
#define PLAYER_FILE_YEAR    2
#define PLAYER_FILE_MON     3
#define PLAYER_FILE_DAY     4
#define PLAYER_FILE_HOUR    5
#define PLAYER_FILE_MIN     6
#define PLAYER_FILE_MORNING   7
#define PLAYER_FILE_AFTERNOON 8
#define PLAYER_FILE_ZHENG   9
#define PLAYER_FILE_WENDU   10
#define PLAYER_FILE_SHESHI  11
#define PLAYER_FILE_HUASHI  12
#define PLAYER_FILE_FU      13
#define PLAYER_FILE_DIAN    14
#define PLAYER_FILE_NOW     15
#define PLAYER_FILE_WEEK    16
#define PLAYER_FILE_PAUSE   17
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
016：星期
017: (停顿)
*/

#define PLAYER_DIR_NUM1      2
#define PLAYER_FILE_TEN       1
#define PLAYER_FILE_HUNDRED   2
#define PLAYER_FILE_THOUSAND  3

/*
dir 02：
001 ~ 003: 十、百、千
*/

#define PLAYER_DIR_NUM2 3
#define PLAYER_FILE_ZERO       1
#define PLAYER_FILE_ONE        2
#define PLAYER_FILE_TWO        3
#define PLAYER_FILE_THREE      4
#define PLAYER_FILE_FOUR       5
#define PLAYER_FILE_FIVE       6
#define PLAYER_FILE_SIX        7
#define PLAYER_FILE_SEVEN      8
#define PLAYER_FILE_EIGHT      9
#define PLAYER_FILE_NIGHT      10
/*
dir 03:
001 ~ 010: 零-九
*/

#define PLAYER_DIR_ALARM 4
/*
dir 04（闹钟）：
001 ~ 010：闹钟声音
*/
#define PLAYER_FILE_ALARM1
#define PLAYER_FILE_ALARM2
#define PLAYER_FILE_ALARM3
#define PLAYER_FILE_ALARM4
#define PLAYER_FILE_ALARM5
#define PLAYER_FILE_ALARM6
#define PLAYER_FILE_ALARM7
#define PLAYER_FILE_ALARM8
#define PLAYER_FILE_ALARM9

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

#define PLAYER_MAX_SEQ_LEN 64

static struct player_seq_node player_seq[PLAYER_MAX_SEQ_LEN + 1];  

static uint8_t player_seq_current_index;
static bool player_seq_in_playing;
static bool _player_is_on;

static void player_play_sequence_start(void);
  
void player_init(void)
{
  memset(player_seq, 0 ,sizeof(player_seq));
  player_seq_current_index = 0;
  player_seq_in_playing = FALSE;
  _player_is_on = FALSE;
  player_on();
  player_play_sequence_start();
}

bool player_is_on(void)
{
  return _player_is_on;
}

void player_on(void)
{
  _player_is_on = TRUE;
//  BSP_MP3_Wakeup();
}

void player_off(void)
{
  _player_is_on = FALSE;
//  BSP_MP3_Standby();  
}

static void player_play_sequence_start(void)
{
  IVDBG("player_play_sequence_start");
  player_seq_in_playing = TRUE;
  player_seq_current_index = 0;
  if(player_seq[0].dir && player_seq[0].file) {
    if(BSP_MP3_Play_Dir_File(player_seq[0].dir, player_seq[0].file)
      != BSP_ERROR_NONE) {
        player_seq_in_playing = FALSE;
      }
  }
}

static void player_play_sequence_stop(void)
{
  IVDBG("player_play_sequence_stop");
    if(player_seq_in_playing) {
    BSP_MP3_Stop();
    player_seq_in_playing = FALSE;
    player_seq_current_index = 0;   
  }
}

static uint8_t player_synthetise_year(uint8_t start, uint8_t len)
{
  uint8_t year = clock_get_year();
  /* 二零 */
  player_seq[start].dir  = PLAYER_DIR_NUM2;
  player_seq[start].file = PLAYER_FILE_TWO;  
  player_seq[start+1].dir  = PLAYER_DIR_NUM2;
  player_seq[start+1].file = PLAYER_FILE_ZERO; 

  player_seq[start+2].dir  = PLAYER_DIR_NUM2;
  player_seq[start+2].file = year / 10 + 1;
  
  player_seq[start+3].dir  = PLAYER_DIR_NUM2;
  player_seq[start+3].file = year % 10 + 1;  
  
  player_seq[start+4].dir  = PLAYER_DIR_MISC;
  player_seq[start+4].file = PLAYER_FILE_YEAR;  
  
  return 5;
}

static uint8_t player_synthetise_mon(uint8_t start, uint8_t len)
{
  uint8_t mon = clock_get_month();
  uint8_t ret = 1;
  if(mon >= 1 && mon <= 9) {
    player_seq[start].dir  = PLAYER_DIR_NUM2;
    player_seq[start].file = mon + 1;
  } else {
    player_seq[start].dir  = PLAYER_DIR_NUM1;
    player_seq[start].file = PLAYER_FILE_TEN;
    mon %= 10;
    if(mon != 0) {
      ret ++;
      player_seq[start + 1].dir  = PLAYER_DIR_NUM2;
      player_seq[start + 1].file = mon + 1;
    }
  }
  player_seq[start + ret].dir = PLAYER_DIR_MISC;
  player_seq[start + ret].file = PLAYER_FILE_MON;
  ret ++;
  
  return ret;
}

static uint8_t player_synthetise_date(uint8_t start, uint8_t len)
{
  uint8_t date = clock_get_date();
  uint8_t ret = 0;
  if(date >= 1 && date <= 9) {
    player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
    player_seq[start + ret].file = date + 1;
    ret ++;
  } else if(date >= 10 && date <= 19){
    player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
    player_seq[start + ret].file = PLAYER_FILE_TEN;
    ret ++;
    if((date % 10) != 0) {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = date % 10 + 1;
      ret ++;
    }
  } else {
    player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
    player_seq[start + ret].file = date / 10 + 1;
    ret ++;
    player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
    player_seq[start + ret].file = PLAYER_FILE_TEN;    
    ret ++;
    if((date % 10) != 0) {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = date % 10 + 1;
      ret ++;
    }
  }
  player_seq[start + ret].dir = PLAYER_DIR_MISC;
  player_seq[start + ret].file = PLAYER_FILE_DAY;
  ret ++;
  
  return ret;
}

static uint8_t player_synthetise_day(uint8_t start, uint8_t len)
{
  uint8_t day = clock_get_day();
  player_seq[start].dir  = PLAYER_DIR_NUM1;
  player_seq[start].file = PLAYER_FILE_WEEK;
  if(day == 7){
    player_seq[start + 1].dir  = PLAYER_DIR_NUM1;
    player_seq[start + 1].file = PLAYER_FILE_DAY;
  } else {
    player_seq[start + 1].dir  = PLAYER_DIR_NUM2;
    player_seq[start + 1].file = day + 1;
  }
  return 2;
}

static uint8_t player_synthetise_time(uint8_t start, uint8_t len)
{
  uint8_t hour = clock_get_hour();
  uint8_t min = clock_get_min();
  uint8_t ret = 0;
  
  if(clock_is_hour_12format()) {
    hour = clock_get_hour12();
    player_seq[start + ret].dir  = PLAYER_DIR_MISC;
    player_seq[start + ret].file = clock_is_pm() ? PLAYER_FILE_AFTERNOON : PLAYER_FILE_MORNING;
    ret ++;
    if(hour <= 9) {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = hour + 1;
      ret ++;
    } else if(hour == 10) {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
      player_seq[start + ret].file = PLAYER_FILE_TEN;
      ret ++;
    } else {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
      player_seq[start + ret].file = PLAYER_FILE_TEN;
      ret ++;
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = hour % 10 + 1;
      ret ++;      
    }
  } else {
    if(hour == 0) {
      player_seq[start + ret].dir  = PLAYER_DIR_MISC;
      player_seq[start + ret].file = PLAYER_FILE_ZERO;
      ret ++;
    } else if(hour >= 1 && hour <= 9) {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = hour + 1;
      ret ++;
    } else if(hour >= 10 && hour <= 19){
      player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
      player_seq[start + ret].file = PLAYER_FILE_TEN;
      ret ++;
      if((hour % 10) != 0) {
        player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
        player_seq[start + ret].file = hour % 10 + 1;
        ret ++;
      }
    } else {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = hour / 10 + 1;
      ret ++;
      player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
      player_seq[start + ret].file = PLAYER_FILE_TEN;    
      ret ++;
      if((hour % 10) != 0) {
        player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
        player_seq[start + ret].file = hour % 10 + 1;
        ret ++;
      }
    }
  }
  
  player_seq[start + ret].dir  = PLAYER_DIR_MISC;
  player_seq[start + ret].file = PLAYER_FILE_DIAN;
  ret ++;
  
  if(min == 0) {
    player_seq[start + ret].dir  = PLAYER_DIR_MISC;
    player_seq[start + ret].file = PLAYER_FILE_ZHENG;
    ret ++;
  } else {
    if(min >= 1 && min <= 9) {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = min + 1;
      ret ++;
    } else if(min >= 10 && min <= 19){
      player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
      player_seq[start + ret].file = PLAYER_FILE_TEN;
      ret ++;
      if((min % 10) != 0) {
        player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
        player_seq[start + ret].file = min % 10 + 1;
        ret ++;
      }
    } else {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = min / 10 + 1;
      ret ++;
      player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
      player_seq[start + ret].file = PLAYER_FILE_TEN;    
      ret ++;
      if((min % 10) != 0) {
        player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
        player_seq[start + ret].file = min % 10 + 1;
        ret ++;
      }
    }
    player_seq[start + ret].dir  = PLAYER_DIR_MISC;
    player_seq[start + ret].file = PLAYER_FILE_MIN;    
    ret ++;
  }
  return ret;
}

void player_report_clk(void)
{
  uint8_t i, len = 0;
  /* 现在时间 */
  player_seq[0].dir  = PLAYER_DIR_MISC;
  player_seq[0].file = PLAYER_FILE_NOW;
  player_seq[1].dir  = PLAYER_DIR_MISC;
  player_seq[1].file = PLAYER_FILE_PAUSE; 
  len = 2;
  

  /* XXXX 年 */
  len += player_synthetise_year(len, PLAYER_MAX_SEQ_LEN - len);
  if(len > PLAYER_MAX_SEQ_LEN - 1) goto err;
  player_seq[len].dir  = PLAYER_DIR_MISC;
  player_seq[len].file = PLAYER_FILE_PAUSE;  
  len ++;
  
  /* XXXX 月 */
  len += player_synthetise_mon(len, PLAYER_MAX_SEQ_LEN - len);
  if(len > PLAYER_MAX_SEQ_LEN - 1) goto err;
  player_seq[len].dir  = PLAYER_DIR_MISC;
  player_seq[len].file = PLAYER_FILE_PAUSE;  
  len ++;  

  /* XXXX 日 */
  len += player_synthetise_date(len, PLAYER_MAX_SEQ_LEN - len);
  if(len > PLAYER_MAX_SEQ_LEN - 1) goto err;
  player_seq[len].dir  = PLAYER_DIR_MISC;
  player_seq[len].file = PLAYER_FILE_PAUSE;  
  len ++;  
  
  /* XXXX 星期 */
  len += player_synthetise_day(len, PLAYER_MAX_SEQ_LEN - len);
  if(len > PLAYER_MAX_SEQ_LEN - 1) goto err;
  player_seq[len].dir  = PLAYER_DIR_MISC;
  player_seq[len].file = PLAYER_FILE_PAUSE;  
  len ++;   
  
  /* XXXX 时|xxx分|整 */
  len += player_synthetise_time(len, PLAYER_MAX_SEQ_LEN - len);
  if(len > PLAYER_MAX_SEQ_LEN - 1) goto err;
  player_seq[len].dir  = PLAYER_DIR_MISC;
  player_seq[len].file = PLAYER_FILE_PAUSE;  
  len ++;
  
  player_seq[len].dir = 0;
  player_seq[len].file = 0;
  
  player_play_sequence_start();
  return;

err:
  player_seq[PLAYER_MAX_SEQ_LEN].dir = 0;
  player_seq[PLAYER_MAX_SEQ_LEN].file = 0;  
}

void player_report_temperature(void)
{
  uint8_t integer, flt, ret = 0;
  bool sign;
  
  sign = BSP_RTC_Get_Temperature(&integer, &flt);
  
  player_seq[ret].dir  = PLAYER_DIR_MISC;
  player_seq[ret].file = PLAYER_FILE_WENDU;
  ret ++;
  
  player_seq[ret].dir  = PLAYER_DIR_MISC;
  player_seq[ret].file = config_read("temp_cel")->val8 ? 
    PLAYER_FILE_SHESHI : PLAYER_FILE_HUASHI;
  ret ++;  
  
  if(sign) {
    player_seq[ret].dir  = PLAYER_DIR_MISC;
    player_seq[ret].file = PLAYER_FILE_FU;
    ret ++;
  }
    if(integer == 0) {
      player_seq[ret].dir  = PLAYER_DIR_NUM2;
      player_seq[ret].file = PLAYER_FILE_ZERO;
      ret ++;
    } else if(integer >= 1 && integer <= 9) {
      player_seq[ret].dir  = PLAYER_DIR_NUM2;
      player_seq[ret].file = integer + 1;
      ret ++;
    } else if(integer >= 10 && integer <= 19){
      player_seq[ret].dir  = PLAYER_DIR_NUM1;
      player_seq[ret].file = PLAYER_FILE_TEN;
      ret ++;
      if((integer % 10) != 0) {
        player_seq[ret].dir  = PLAYER_DIR_NUM2;
        player_seq[ret].file = integer % 10 + 1;
        ret ++;
      }
    } else {
      player_seq[ret].dir  = PLAYER_DIR_NUM2;
      player_seq[ret].file = integer / 10 + 1;
      ret ++;
      player_seq[ret].dir  = PLAYER_DIR_NUM1;
      player_seq[ret].file = PLAYER_FILE_TEN;    
      ret ++;
      if((integer % 10) != 0) {
        player_seq[ret].dir  = PLAYER_DIR_NUM2;
        player_seq[ret].file = integer % 10 + 1;
        ret ++;
      }
    }
    
  if(flt != 0) {    
    player_seq[ret].dir  = PLAYER_DIR_MISC;
    player_seq[ret].file = PLAYER_FILE_DIAN;
    ret ++;
    
    player_seq[ret].dir  = PLAYER_DIR_NUM2;
    player_seq[ret].file = flt / 10 + 1;
    ret ++; 

    player_seq[ret].dir  = PLAYER_DIR_NUM2;
    player_seq[ret].file = flt % 10 + 1;
    ret ++;     
  }

  player_seq[ret].dir = 0;  
  player_seq[ret].file = 0;
  
  player_play_sequence_start();
}


void player_play_alarm(uint8_t alarm_index)
{
  player_seq[player_seq_current_index].dir  = PLAYER_DIR_ALARM;
  player_seq[player_seq_current_index].file = alarm_index + 1;
  player_play_sequence_start();
}

void player_stop_play(void)
{
  player_play_sequence_stop();
}

bool player_is_playing(void)
{
  return player_seq_in_playing;
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

void player_show(void)
{
  console_printf("player is %s, is playing: %s",
    _player_is_on ? "on" : "off",
    player_seq_in_playing ? "on" : "off"
  );
}

