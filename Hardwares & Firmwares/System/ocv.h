#ifndef __IVCLOCK_OCV_H__
#define __IVCLOCK_OCV_H__

#include <stdint.h>

typedef struct ocv {
  float voltage;
  uint8_t quantity;
}ocv_t;

extern ocv_t ocv_table[12];

#endif