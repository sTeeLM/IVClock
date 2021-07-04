#ifndef __IVCLOCK_CONFIG_H__
#define __IVCLOCK_CONFIG_H__

#include <stdint.h>


typedef enum config_type {
	CONFIG_TYPE_UINT8,
	CONFIG_TYPE_UINT16,	
	CONFIG_TYPE_UINT32,
	CONFIG_TYPE_CNT
}config_type_t;

typedef union {
		uint8_t  val8;
		uint16_t val16;
		uint32_t val32;
}config_val_t;

#define CONFIG_NAME_LEN 16
typedef struct
{
	const char name[CONFIG_NAME_LEN];
	config_type_t type;
}config_slot_t;



void config_init(void);
const config_val_t * config_read(const char * name);
void config_write(const char * name, const config_val_t * val);

#endif