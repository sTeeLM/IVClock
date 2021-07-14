#ifndef __IVCLOCK_PLAYER_H__
#define __IVCLOCK_PLAYER_H__

#include <stdint.h>
#include "cext.h"

struct player_seq_node
{
  uint8_t dir;
  uint8_t file;
};

void player_init(void);
void player_scan(void);
void player_on(void);
void player_off(void);
void player_report_clk(void);
void player_report_temperature(void);
void player_play_alarm(uint8_t alarm_index);
void player_stop_play(void);
bool player_is_playing(void);
void player_show(void);
bool player_is_on(void);
#endif