#ifndef __IVCLOCK_PLAYER_H__
#define __IVCLOCK_PLAYER_H__

#include <stdint.h>

struct player_seq_node
{
  uint8_t dir;
  uint8_t file;
};

void player_init(void);
void player_scan(void);

#endif