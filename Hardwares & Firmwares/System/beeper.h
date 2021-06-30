#ifndef __IVCLOCK_BEEPER_H__
#define __IVCLOCK_BEEPER_H__

#include <stdint.h>
#include "cext.h"

bool beeper_init(void);
void beeper_beep(void);

#endif