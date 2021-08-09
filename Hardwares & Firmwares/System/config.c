#include "config.h"
#include "rom.h"
#include "debug.h"
#include "button.h"
#include <stddef.h>
#include <string.h>

static config_slot_t _slot[] = {
  {"time_12", CONFIG_TYPE_UINT8,   {.val8 = 1}},
  {"acc_th",CONFIG_TYPE_UINT8,     {.val8 = 50}},
  {"acc_en",CONFIG_TYPE_UINT8,     {.val8 = 1}},
  {"bp_en",CONFIG_TYPE_UINT8,      {.val8 = 1}},
  {"alm0_day_mask", CONFIG_TYPE_UINT8, {.val8 = 0}},
  {"alm0_hour", CONFIG_TYPE_UINT8, {.val8 = 0}}, 
  {"alm0_min", CONFIG_TYPE_UINT8,  {.val8 = 0}},
  {"alm0_snd", CONFIG_TYPE_UINT8,  {.val8 = 0}},   
  {"alm1_en", CONFIG_TYPE_UINT8,   {.val8 = 1}},  
  {"bat_65", CONFIG_TYPE_UINT16,   {.val16 = 0}},
  {"bat_90", CONFIG_TYPE_UINT16,   {.val16 = 0}},   
  {"lt_0", CONFIG_TYPE_UINT16,     {.val16 = 0}},
  {"lt_100", CONFIG_TYPE_UINT16,   {.val16 = 0}}, 
  {"mon_lt_en", CONFIG_TYPE_UINT8, {.val8 = 1}},
  {"temp_cen", CONFIG_TYPE_UINT8,  {.val8 = 1}}, 
  {"power_timeo", CONFIG_TYPE_UINT8,  {.val8 = 30}},   
};

static config_val_t _val;

static void config_factory_reset(void)
{
  uint32_t i, offset = 0;
  for(i = 0 ; i < sizeof(_slot) / sizeof(config_slot_t) ; i ++) { 
    switch(_slot[i].type) {
      case CONFIG_TYPE_UINT8:
        _val.val8 = _slot[i].default_val.val8;
        BSP_ROM_Write(offset, (uint8_t *)(&_val.val8), 1);
        offset += 1;
        break;
      case CONFIG_TYPE_UINT16:
        _val.val16 = _slot[i].default_val.val16;
        BSP_ROM_Write(offset, (uint8_t *)(&_val.val16), 2);
        offset += 2;
        break;
      case CONFIG_TYPE_UINT32:
        _val.val32 = _slot[i].default_val.val32;
        BSP_ROM_Write(offset, (uint8_t *)(&_val.val32), 4);
        offset += 4;
        break;
      default: ;
    }   
  } 
}

static config_slot_t * find_config(const char * name, uint32_t * offset)
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

static void config_dump(void)
{
  uint32_t i, offset = 0;
  IVDBG("dump all config begin ----------------------");
  for(i = 0 ; i < sizeof(_slot) / sizeof(config_slot_t) ; i ++) { 
    switch(_slot[i].type) {
      case CONFIG_TYPE_UINT8:
        BSP_ROM_Read(offset, (uint8_t *)(&_val.val8), 1);
        offset += 1;
        break;
      case CONFIG_TYPE_UINT16:
        BSP_ROM_Read(offset, (uint8_t *)(&_val.val16), 2);
        offset += 2;
        break;
      case CONFIG_TYPE_UINT32:
        BSP_ROM_Read(offset, (uint8_t *)(&_val.val32), 4);
        offset += 4;
        break;
      default: ;
    }   
    IVDBG("[0x%08x] %s : %d", offset, _slot[i].name, 
      _slot[i].type == CONFIG_TYPE_UINT8 ? _val.val8 : 
      (_slot[i].type == CONFIG_TYPE_UINT16 ? _val.val16 : 
      (_slot[i].type == CONFIG_TYPE_UINT32 ? _val.val32 : _val.val32))
    );
  } 
  IVDBG("dump all config end ----------------------");
}

void config_init(void)
{
  if(button_is_factory_reset()) { //12:10:30 PM
    IVINFO("config factory reset");
    config_factory_reset();
  }    
  config_dump();
}

const config_val_t * config_read(const char * name)
{
  uint32_t off = 0, size = 0;
  config_slot_t * p = find_config(name, &off);
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
  config_slot_t * p = find_config(name, &off);
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
