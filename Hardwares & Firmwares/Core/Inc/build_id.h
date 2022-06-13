#ifndef __IVCLOCK_BUILD_ID_H__
#define __IVCLOCK_BUILD_ID_H__

#include <stdint.h>

#define BUILD_ID_SIZE (16)

extern const uint8_t build_id[BUILD_ID_SIZE];
extern const char * build_id_str;

#endif