#ifndef __IVCLOCK_PLAYER_H__
#define __IVCLOCK_PLAYER_H__

#include <stdint.h>

struct player_seq_node
{
  uint8_t dir;
  uint8_t file;
  uint16_t delay_ms;
};

void player_init(void);
void player_scan(void);
void player_on(void);
void player_off(void);
#endif