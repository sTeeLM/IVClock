#ifndef __IVCLOCK_PLAYER_H__
#define __IVCLOCK_PLAYER_H__

#include <stdint.h>
#include "cext.h"

struct player_seq_node
{
  uint8_t dir;
  uint8_t file;
};

enum player_snd_index
{
  PLAYER_SND_1,
  PLAYER_SND_2,
  PLAYER_SND_3,
  PLAYER_SND_4,
  PLAYER_SND_5,
  PLAYER_SND_6,
  PLAYER_SND_7,
  PLAYER_SND_8,
  PLAYER_SND_9,
  PLAYER_SND_10,  
};

void player_init(void);
void player_scan(void);
void player_on(void);
void player_off(void);
bool player_is_on(void);
void player_report_clk(void);
void player_report_temperature(void);
void player_play_snd(enum player_snd_index index);
void player_stop_play(void);
bool player_is_playing(void);
void player_show(void);

#endif