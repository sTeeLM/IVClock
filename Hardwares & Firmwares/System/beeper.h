#ifndef __IVCLOCK_BEEPER_H__
#define __IVCLOCK_BEEPER_H__

#include <stdint.h>
#include "cext.h"

void beeper_init(void);
void beeper_beep(void);
void beeper_beep_beep(void);
void beeper_set_enable(bool enable);
bool beeper_test_enable(void);
void beeper_save_config(void);
void beeper_enter_powersave(void);
void beeper_leave_powersave(void);
#endif