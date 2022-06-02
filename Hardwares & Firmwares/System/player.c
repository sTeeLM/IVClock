#include "player.h"
#include "mp3.h"
#include "clock.h"
#include "config.h"
#include "delay.h"
#include "debug.h"
#include "thermometer.h"
#include "console.h"
#include "power.h"
#include <string.h>

#define PLAYER_DIR_MISC     1

#define PLAYER_FILE_YEAR    1
#define PLAYER_FILE_MON     2
#define PLAYER_FILE_DAY     3
#define PLAYER_FILE_HOUR    4
#define PLAYER_FILE_MIN     5
#define PLAYER_FILE_SEC     6
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
#define PLAYER_FILE_DU      18
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
#define PLAYER_FILE_NIGH       10
/*
dir 03:
001 ~ 010: 零-九
*/

#define PLAYER_DIR_ALARM 4
/*
dir 04（闹钟）：
001 ~ 010：闹钟声音
*/
#define PLAYER_FILE_ALARM1  1
#define PLAYER_FILE_ALARM2  2
#define PLAYER_FILE_ALARM3  3
#define PLAYER_FILE_ALARM4  4
#define PLAYER_FILE_ALARM5  5
#define PLAYER_FILE_ALARM6  6
#define PLAYER_FILE_ALARM7  7
#define PLAYER_FILE_ALARM8  8
#define PLAYER_FILE_ALARM9  9
#define PLAYER_FILE_ALARM10  10
#define PLAYER_FILE_ALARM_CNT     10

#define PLAYER_DIR_EFECTS 5
/*
dir 04（音效）：
001 ~ 010：音效声音
*/
#define PLAYER_FILE_EFECTS1  1
#define PLAYER_FILE_EFECTS2  2
#define PLAYER_FILE_EFECTS3  3
#define PLAYER_FILE_EFECTS4  4
#define PLAYER_FILE_EFECTS5  5
#define PLAYER_FILE_EFECTS6  6
#define PLAYER_FILE_EFECTS7  7
#define PLAYER_FILE_EFECTS8  8
#define PLAYER_FILE_EFECTS9  9
#define PLAYER_FILE_EFECTS10  10
#define PLAYER_FILE_EFECTS_CNT     10
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
  
static uint8_t player_vol;
  
void player_init(void)
{
  memset(player_seq, 0 ,sizeof(player_seq));
  player_seq_current_index = 0;
  player_seq_in_playing = FALSE;
  _player_is_on = FALSE;
  player_on();
  player_vol = config_read_int("ply_vol");
  BSP_MP3_Set_Volume(player_vol);
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
    if(!BSP_MP3_Play_Dir_File(player_seq[0].dir, player_seq[0].file)) {
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
    task_set(EV_PLAYER_STOP);
  }
}

static uint8_t player_number_to_tune(uint8_t num)
{
  
  if(num > 9) {
    IVERR("player_number_to_tune error num %d > 9", num);
    num = 9;
  }
  switch (num)
  {
    case 0: return PLAYER_FILE_ZERO;
    case 1: return PLAYER_FILE_ONE;
    case 2: return PLAYER_FILE_TWO;
    case 3: return PLAYER_FILE_THREE;
    case 4: return PLAYER_FILE_FOUR;
    case 5: return PLAYER_FILE_FIVE;
    case 6: return PLAYER_FILE_SIX;
    case 7: return PLAYER_FILE_SEVEN;
    case 8: return PLAYER_FILE_EIGHT;
    case 9: return PLAYER_FILE_NIGH;
    default: ;
  }
  return PLAYER_FILE_ZERO;
}

static void player_dump_seq(void)
{
  int8_t i;
  IVDBG("player_dump_seq begin----------------------------");
  for(i = 0 ; i < sizeof(player_seq) / sizeof(struct player_seq_node) ; i++) {
    IVDBG("%02d: [%03d,%03d]", i, player_seq[i].dir, player_seq[i].file);
  }
  IVDBG("player_dump_seq end------------------------------");
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
  player_seq[start+2].file = player_number_to_tune(year / 10);
  
  player_seq[start+3].dir  = PLAYER_DIR_NUM2;
  player_seq[start+3].file = player_number_to_tune(year % 10);  
  
  player_seq[start+4].dir  = PLAYER_DIR_MISC;
  player_seq[start+4].file = PLAYER_FILE_YEAR;  
  
  return 5;
}

static uint8_t player_synthetise_number(uint8_t num, uint8_t start, uint8_t len)
{
  uint8_t ret = 0;
  if(num == 0) {
    player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
    player_seq[start + ret].file = PLAYER_FILE_ZERO;
    ret ++;
  } else if(num >= 1 && num <= 9) {
    player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
    player_seq[start + ret].file = player_number_to_tune(num);
    ret ++;
  } else if(num >= 10 && num <= 19){
    player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
    player_seq[start + ret].file = PLAYER_FILE_TEN;
    ret ++;
    if((num % 10) != 0) {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = player_number_to_tune(num % 10);
      ret ++;
    }
  } else {
    player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
    player_seq[start + ret].file = player_number_to_tune(num / 10);
    ret ++;
    player_seq[start + ret].dir  = PLAYER_DIR_NUM1;
    player_seq[start + ret].file = PLAYER_FILE_TEN;    
    ret ++;
    if((num % 10) != 0) {
      player_seq[start + ret].dir  = PLAYER_DIR_NUM2;
      player_seq[start + ret].file = player_number_to_tune(num % 10);
      ret ++;
    }
  }
  return ret;
}

static uint8_t player_synthetise_mon(uint8_t start, uint8_t len)
{
  uint8_t mon = clock_get_month();
  uint8_t ret = 0;
  ret += player_synthetise_number(mon, start + ret, len - ret);
  player_seq[start + ret].dir = PLAYER_DIR_MISC;
  player_seq[start + ret].file = PLAYER_FILE_MON;
  ret ++;
  
  return ret;
}

static uint8_t player_synthetise_date(uint8_t start, uint8_t len)
{
  uint8_t date = clock_get_date();
  uint8_t ret = 0;
  ret += player_synthetise_number(date, start + ret, len - ret);
  player_seq[start + ret].dir = PLAYER_DIR_MISC;
  player_seq[start + ret].file = PLAYER_FILE_DAY;
  ret ++;
  
  return ret;
}

static uint8_t player_synthetise_day(uint8_t start, uint8_t len)
{
  uint8_t day = clock_get_day();
  player_seq[start].dir  = PLAYER_DIR_MISC;
  player_seq[start].file = PLAYER_FILE_WEEK;
  if(day == 7){
    player_seq[start + 1].dir  = PLAYER_DIR_NUM1;
    player_seq[start + 1].file = PLAYER_FILE_DAY;
  } else {
    player_seq[start + 1].dir  = PLAYER_DIR_NUM2;
    player_seq[start + 1].file = player_number_to_tune(day);
  }
  return 2;
}

static uint8_t player_synthetise_time(uint8_t start, uint8_t len)
{
  uint8_t hour = clock_get_hour();
  uint8_t min = clock_get_min();
  uint8_t ret = 0, hour12;
  bool ispm;
  
  if(clock_test_hour12()) {
    ispm = cext_cal_hour12(hour, &hour12);
    hour = hour12;
    player_seq[start + ret].dir  = PLAYER_DIR_MISC;
    player_seq[start + ret].file = ispm ? PLAYER_FILE_AFTERNOON : PLAYER_FILE_MORNING;
    ret ++;
    ret += player_synthetise_number(hour, start + ret, len - ret);
  } else {
    ret += player_synthetise_number(hour, start + ret, len - ret);
  }
  
  player_seq[start + ret].dir  = PLAYER_DIR_MISC;
  player_seq[start + ret].file = PLAYER_FILE_DIAN;
  ret ++;
  
  if(min == 0) {
    player_seq[start + ret].dir  = PLAYER_DIR_MISC;
    player_seq[start + ret].file = PLAYER_FILE_ZHENG;
    ret ++;
  } else {
    ret += player_synthetise_number(min, start + ret, len - ret);
    player_seq[start + ret].dir  = PLAYER_DIR_MISC;
    player_seq[start + ret].file = PLAYER_FILE_MIN;    
    ret ++;
  }
  return ret;
}

void player_report_clk(void)
{
  uint8_t i, len = 0;
  
  memset(player_seq, 0 ,sizeof(player_seq));
  
  /* 现在时间 */
  player_seq[0].dir  = PLAYER_DIR_MISC;
  player_seq[0].file = PLAYER_FILE_NOW;
  player_seq[1].dir  = PLAYER_DIR_MISC;
  player_seq[1].file = PLAYER_FILE_PAUSE; 
  len = 2;
  

  /* XXXX 年 */
  len += player_synthetise_year(len, PLAYER_MAX_SEQ_LEN - len);
  if(len > PLAYER_MAX_SEQ_LEN - 1) goto err;
//  player_seq[len].dir  = PLAYER_DIR_MISC;
//  player_seq[len].file = PLAYER_FILE_PAUSE;  
//  len ++;
  
  /* XXXX 月 */
  len += player_synthetise_mon(len, PLAYER_MAX_SEQ_LEN - len);
  if(len > PLAYER_MAX_SEQ_LEN - 1) goto err;
//  player_seq[len].dir  = PLAYER_DIR_MISC;
//  player_seq[len].file = PLAYER_FILE_PAUSE;  
//  len ++;  

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
  
  player_dump_seq();
  player_play_sequence_start();
  return;

err:
  player_seq[0].dir = 0;
  player_seq[0].file = 0;  
}

void player_report_temperature(void)
{
  uint16_t integer, flt, ret = 0;
  uint8_t start = 0;
  uint8_t len = PLAYER_MAX_SEQ_LEN;
  bool sign;
  
  memset(player_seq, 0 ,sizeof(player_seq));
  
  if(config_read_int("temp_cen")) {
    sign = thermometer_read_cen(&integer, &flt);
  } else {
    sign = thermometer_read_fah(&integer, &flt);
  }
  
  player_seq[ret].dir  = PLAYER_DIR_MISC;
  player_seq[ret].file = PLAYER_FILE_WENDU;
  ret ++;
  
  player_seq[ret].dir  = PLAYER_DIR_MISC;
  player_seq[ret].file = config_read_int("temp_cen") ? 
    PLAYER_FILE_SHESHI : PLAYER_FILE_HUASHI;
  ret ++;  
  
  if(sign) {
    player_seq[ret].dir  = PLAYER_DIR_MISC;
    player_seq[ret].file = PLAYER_FILE_FU;
    ret ++;
  }

  ret += player_synthetise_number(integer, start + ret, len - ret);
    
  if(flt != 0) {    
    player_seq[ret].dir  = PLAYER_DIR_MISC;
    player_seq[ret].file = PLAYER_FILE_DIAN;
    ret ++;
    
    player_seq[ret].dir  = PLAYER_DIR_NUM2;
    player_seq[ret].file = player_number_to_tune(flt / 10);
    ret ++; 

    player_seq[ret].dir  = PLAYER_DIR_NUM2;
    player_seq[ret].file = player_number_to_tune(flt % 10);
    ret ++;     
  }
  
  player_seq[ret].dir  = PLAYER_DIR_MISC;
  player_seq[ret].file = PLAYER_FILE_DU;
  ret ++;

  player_seq[ret].dir = 0;  
  player_seq[ret].file = 0;
  
  player_dump_seq();
  player_play_sequence_start();
}

static uint8_t player_play_snd_index_to_file(enum player_snd_dir dir, uint8_t index)
{
  if(dir == PLAYER_SND_DIR_ALARM) {
    if(index > PLAYER_FILE_ALARM_CNT - 1)
      index = PLAYER_FILE_ALARM_CNT - 1;
    return index + 1;
  } else if (dir == PLAYER_SND_DIR_EFFETS) {
    if(index >  PLAYER_FILE_EFECTS_CNT - 1)
      index = PLAYER_FILE_EFECTS_CNT - 1;
    return index + 1;
  } else {
    return 0;
  }
}

uint8_t player_get_snd_cnt(enum player_snd_dir dir)
{
  if(dir == PLAYER_SND_DIR_ALARM) {
    return PLAYER_FILE_ALARM_CNT;
  } else if (dir == PLAYER_SND_DIR_EFFETS) {
    return PLAYER_FILE_EFECTS_CNT;
  } else {
    return 0;
  }
}

void player_play_snd(enum player_snd_dir dir, uint8_t index)
{
  player_seq[player_seq_current_index].dir  = dir;
  player_seq[player_seq_current_index].file = player_play_snd_index_to_file(dir, index);
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
  console_printf("player is %s, is playing: %s\r\n",
    _player_is_on ? "on" : "off",
    player_seq_in_playing ? "on" : "off"
  );
}

uint8_t player_get_vol(void)
{
  return player_vol;
}

uint8_t player_get_min_vol(void)
{
  return BSP_MP3_Get_Min_Volume();
}

uint8_t player_get_max_vol(void)
{
  return BSP_MP3_Get_Max_Volume();
}

uint8_t player_inc_vol(void)
{
  player_vol ++;
  if(player_vol > BSP_MP3_Get_Max_Volume())
    player_vol = BSP_MP3_Get_Min_Volume();
  
  BSP_MP3_Set_Volume(player_vol);
  
  return player_vol;
}

uint8_t player_set_vol(uint8_t vol)
{
  player_vol = vol;
  if(player_vol > BSP_MP3_Get_Max_Volume())
    player_vol = BSP_MP3_Get_Max_Volume();
  
  if(player_vol < BSP_MP3_Get_Min_Volume())
    player_vol = BSP_MP3_Get_Min_Volume();
  
  BSP_MP3_Set_Volume(player_vol);
  
  return player_vol;
}

void player_save_config(void)
{
  config_val_t val;
  val.val8 = player_vol;
  config_write("ply_vol", &val);
}

void player_enter_powersave(void)
{
  power_player_enable(FALSE);
}

void player_leave_powersave(void)
{
  power_player_enable(TRUE);
  BSP_MP3_Init();
}
