#include "config.h"
#include "rom.h"
#include "debug.h"
#include <stddef.h>
#include <string.h>

static config_slot_t _slot[] = {
	{"time_fmt", CONFIG_TYPE_UINT8},
};

static config_val_t _val;

static config_slot_t * _find_config(const char * name, uint32_t * offset)
{
	uint32_t i;
	*offset = 0;
	for(i = 0 ; i < sizeof(_slot) / sizeof(config_slot_t) ; i ++) {	
		if(strcmp(name, _slot[i].name) == 0) {
			IVDBG("find_config %s offset at %d", name, *offset);
			return &_slot[i];
		}
		switch(_slot[i].type) {
			case CONFIG_TYPE_UINT8:
				*offset += 1;
				break;
			case CONFIG_TYPE_UINT16:
				*offset += 2;
				break;
			case CONFIG_TYPE_UINT32:
				*offset += 4;
				break;
			default: ;
		}		
	}
	return NULL;
}

static _config_dump(void)
{
	uint32_t i, offset;
	IVDBG("dump all config begin ----------------------");
	for(i = 0 ; i < sizeof(_slot) / sizeof(config_slot_t) ; i ++) {	
		switch(_slot[i].type) {
			case CONFIG_TYPE_UINT8:
				offset += 1;
				break;
			case CONFIG_TYPE_UINT16:
				offset += 2;
				break;
			case CONFIG_TYPE_UINT32:
				offset += 4;
				break;
			default: ;
		}		
		IVDBG("%s at%08x ");
	}	
	IVDBG("dump all config end ----------------------");
}

void config_init(void)
{

}

const config_val_t * config_read(const char * name)
{
	uint32_t off = 0, size = 0;
	config_slot_t * p = _find_config(name, &off);
	if(p) {
		switch (p->type) {
			case CONFIG_TYPE_UINT8:
				BSP_ROM_Read(off, (uint8_t *)(&_val.val8), 1);
				break;
			case CONFIG_TYPE_UINT16:
				BSP_ROM_Read(off, (uint8_t *)(&_val.val16), 2);
				break;
			case CONFIG_TYPE_UINT32:
				BSP_ROM_Read(off, (uint8_t *)(&_val.val32), 4);
				break;
			default: ;
		}
	} else {
		IVERR("config_read: %s not exist!", name);
	}
	return &_val;
}

void config_write(const char * name, const config_val_t * val)
{
	uint32_t off = 0, size = 0;
	config_slot_t * p = _find_config(name, &off);
	memcpy(&_val, val, sizeof(_val));
	if(p) {
		switch (p->type) {
			case CONFIG_TYPE_UINT8:
				BSP_ROM_Write(off, (uint8_t *)(&_val.val8), 1);
				break;
			case CONFIG_TYPE_UINT16:
				BSP_ROM_Write(off, (uint8_t *)(&_val.val16), 2);
				break;
			case CONFIG_TYPE_UINT32:
				BSP_ROM_Write(off, (uint8_t *)(&_val.val32), 4);
				break;
			default: ;
		}
	} else {
		IVERR("config_write: %s not exist!", name);
	}
}
