#ifndef __IVCLOCK_PLAYER_H__
#define __IVCLOCK_PLAYER_H__

#include <stdint.h>
#include "cext.h"

struct player_seq_node
{
  uint8_t dir;
  uint8_t file;
};

enum player_snd_dir
{
  PLAYER_SND_DIR_ALARM = 4,
  PLAYER_SND_DIR_EFFETS
};


void player_init(void);
void player_scan(void);
void player_on(void);
void player_off(void);
bool player_is_on(void);
void player_report_clk_and_temp(void);
void player_play_snd(enum player_snd_dir dir, uint8_t index);
uint8_t player_get_snd_cnt(enum player_snd_dir dir);
void player_stop_play(void);
bool player_is_playing(void);
void player_show(void);
uint8_t player_get_vol(void);
uint8_t player_get_min_vol(void);
uint8_t player_get_max_vol(void);
uint8_t player_inc_vol(void);
uint8_t player_set_vol(uint8_t vol);
void player_save_config(void);
void player_enter_powersave(void);
void player_leave_powersave(void);

#endif