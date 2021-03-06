#include "con_player.h"
#include "console.h"
#include "player.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int8_t con_player(char arg1, char arg2)
{

  if(arg1 == 0) {
    player_show();
  } else if(!strcmp(console_buf + arg1, "on")) {
    player_on();
  } else if(!strcmp(console_buf + arg1, "off")) {
    player_off();
  } else if(!strcmp(console_buf + arg1, "clk")) {
    player_report_clk_and_temp();
  } else if(!strcmp(console_buf + arg1, "alm")) {
    if(arg2 == 0) {
      return -1;
    } else {
      player_play_snd(PLAYER_SND_DIR_ALARM, atoi(console_buf + arg2));
    }
  } else if(!strcmp(console_buf + arg1, "eft")) {
    if(arg2 == 0) {
      return -1;
    } else {
      player_play_snd(PLAYER_SND_DIR_EFFETS, atoi(console_buf + arg2));
    }
  } else if(!strcmp(console_buf + arg1, "stp")) {
    player_stop_play();
  } else {
    return -1;
  }
  return 0;
}

