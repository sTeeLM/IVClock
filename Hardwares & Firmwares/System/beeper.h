#ifndef __IVCLOCK_BEEPER_H__
#define __IVCLOCK_BEEPER_H__

#include <stdint.h>
#include "cext.h"

void beeper_init(void);
void beeper_beep(void);
void beeper_beep_beep(void);
void beeper_enable(bool enable);
bool beeper_enabled(void);

#endif