#include "config.h"
#include "rom.h"
#include "debug.h"
#include "button.h"
#include <stddef.h>
#include <string.h>

static const uint8_t alarm_blob0[] = {0,0,0,0};
static const uint8_t alarm_blob1[] = {0x7F,12,6,1};
static const uint8_t alarm_blob2[] = {0x7F,12,13,3};
static const uint8_t alarm_blob3[] = {0x7F,12,25,9};
static const uint8_t alarm_blob4[] = {0x7F,12,16,4};

static const config_slot_t _slot[] = {
  {"time_12", CONFIG_TYPE_UINT8,   {.val8 = 1}},
  {"acc_th",CONFIG_TYPE_UINT8,     {.val8 = 50}},
  {"bp_en",CONFIG_TYPE_UINT8,      {.val8 = 1}},
  {"alm1_en", CONFIG_TYPE_UINT8,   {.val8 = 1}},  
  {"bat_65", CONFIG_TYPE_UINT16,   {.val16 = 2421}},
  {"bat_90", CONFIG_TYPE_UINT16,   {.val16 = 3667}},   
  {"lt_0", CONFIG_TYPE_UINT16,     {.val16 = 354}},
  {"lt_100", CONFIG_TYPE_UINT16,   {.val16 = 920}}, 
  {"mon_lt_en", CONFIG_TYPE_UINT8, {.val8 = 1}},
  {"temp_cen", CONFIG_TYPE_UINT8,  {.val8 = 1}}, 
  {"power_timeo", CONFIG_TYPE_UINT8,  {.val8 = 15}},
  {"tmr_snd", CONFIG_TYPE_UINT8, {.val8 = 0}},
  {"ply_vol", CONFIG_TYPE_UINT8, {.val8 = 10}},  
  {"alm00_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob1),
    .valblob.body = alarm_blob1}}, 
  {"alm01_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob2),
    .valblob.body = alarm_blob2}},
  {"alm02_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob3),
    .valblob.body = alarm_blob3}}, 
  {"alm03_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob4),
    .valblob.body = alarm_blob4}},
  {"alm04_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob0),
    .valblob.body = alarm_blob0}}, 
  {"alm05_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob0),
    .valblob.body = alarm_blob0}},
  {"alm06_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob0),
    .valblob.body = alarm_blob0}}, 
  {"alm07_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob0),
    .valblob.body = alarm_blob0}},
  {"alm08_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob0),
    .valblob.body = alarm_blob0}}, 
  {"alm09_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm_blob0),
    .valblob.body = alarm_blob0}},  
};



static void config_factory_reset(void)
{
  uint32_t i, offset = 0;
  for(i = 0 ; i < sizeof(_slot) / sizeof(config_slot_t) ; i ++) { 
    switch(_slot[i].type) {
      case CONFIG_TYPE_UINT8:
        BSP_ROM_Write(offset, (uint8_t *)(&_slot[i].default_val.val8), 1);
        offset += 1;
        break;
      case CONFIG_TYPE_UINT16:
        BSP_ROM_Write(offset, (uint8_t *)(&_slot[i].default_val.val16), 2);
        offset += 2;
        break;
      case CONFIG_TYPE_UINT32:
        BSP_ROM_Write(offset, (uint8_t *)(&_slot[i].default_val.val32), 4);
        offset += 4;
        break;
      case CONFIG_TYPE_BLOB:
        BSP_ROM_Write(offset, (uint8_t *)(&_slot[i].default_val.valblob.len),
          sizeof(_slot[i].default_val.valblob.len));
        offset += sizeof(_slot[i].default_val.valblob.len);
        BSP_ROM_Write(offset, (uint8_t *)(_slot[i].default_val.valblob.body), _slot[i].default_val.valblob.len);
        offset += _slot[i].default_val.valblob.len;
        break; 
      default: ;
    }   
  } 
}

static const config_slot_t * find_config(const char * name, uint32_t * offset)
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
      case CONFIG_TYPE_BLOB:
        *offset += _slot[i].default_val.valblob.len + sizeof(_slot[i].default_val.valblob.len);
        break; 
      default: ;
    }   
  }
  return NULL;
}

static void config_dump(void)
{
  uint32_t i, j, offset = 0;
  config_val_t val;
  uint8_t buffer[8];
  IVDBG("dump all config begin ----------------------");
  for(i = 0 ; i < sizeof(_slot) / sizeof(config_slot_t) ; i ++) { 
    switch(_slot[i].type) {
      case CONFIG_TYPE_UINT8:
        BSP_ROM_Read(offset, (uint8_t *)(&val.val8), 1);
        offset += 1;
        break;
      case CONFIG_TYPE_UINT16:
        BSP_ROM_Read(offset, (uint8_t *)(&val.val16), 2);
        offset += 2;
        break;
      case CONFIG_TYPE_UINT32:
        BSP_ROM_Read(offset, (uint8_t *)(&val.val32), 4);
        offset += 4;
        break;
      case CONFIG_TYPE_BLOB:
        BSP_ROM_Read(offset, (uint8_t *)(&val.valblob.len), sizeof(val.valblob.len));
        offset += sizeof(val.valblob.len);
        break;      
      default: ;
    }
    if(_slot[i].type != CONFIG_TYPE_BLOB) {
      IVDBG("[0x%08x] %s : %d", offset, _slot[i].name, 
        _slot[i].type == CONFIG_TYPE_UINT8 ? val.val8 : 
        (_slot[i].type == CONFIG_TYPE_UINT16 ? val.val16 : 
        (_slot[i].type == CONFIG_TYPE_UINT32 ? val.val32 : 0))
      );
    } else {
      IVDBG("[0x%08x] %s (blob len = %d):", offset, _slot[i].name, val.valblob.len);
      for(j = 0 ; j < val.valblob.len / 8 ; j ++) {
        BSP_ROM_Read(offset, buffer, sizeof(buffer));
        offset += 8;
        IVDBG(" %02x %02x %02x %02x %02x %02x %02x %02x",
          buffer[0],buffer[1],buffer[2],buffer[3],
          buffer[4],buffer[5],buffer[6],buffer[7]
        );
      }
      BSP_ROM_Read(offset, buffer, val.valblob.len % 8);
      offset += val.valblob.len % 8;
      IVDBG_RH;
      for(j = 0 ; j < val.valblob.len % 8 ; j ++) {
        IVDBG_R(" %02x", buffer[j]);
      }
      IVDBG_RT;
    }
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

bool config_read(const char * name, config_val_t * val)
{
  uint32_t off = 0;
  bool ret = FALSE;
  const config_slot_t * p = find_config(name, &off);
  if(p) {
    ret = TRUE;
    switch (p->type) {
      case CONFIG_TYPE_UINT8:
        BSP_ROM_Read(off, (uint8_t *)(&val->val8), 1);
        break;
      case CONFIG_TYPE_UINT16:
        BSP_ROM_Read(off, (uint8_t *)(&val->val16), 2);
        break;
      case CONFIG_TYPE_UINT32:
        BSP_ROM_Read(off, (uint8_t *)(&val->val32), 4);
        break;
      case CONFIG_TYPE_BLOB:
        BSP_ROM_Read(off, (uint8_t *)(&val->valblob.len), sizeof(val->valblob.len));
        BSP_ROM_Read(off + sizeof(val->valblob.len), (uint8_t *)(val->valblob.body), val->valblob.len);      
      default: ;
    }
  }
  return ret;
}

uint32_t config_read_int(const char * name)
{
  uint32_t off = 0, size = 0, ret = 0;
  config_val_t val;
  const config_slot_t * p = find_config(name, &off);
  if(p) {
    switch (p->type) {
      case CONFIG_TYPE_UINT8:
        BSP_ROM_Read(off, (uint8_t *)(&val.val8), 1);
        ret = val.val8;
        break;
      case CONFIG_TYPE_UINT16:
        BSP_ROM_Read(off, (uint8_t *)(&val.val16), 2);
        ret = val.val16;
        break;
      case CONFIG_TYPE_UINT32:
        BSP_ROM_Read(off, (uint8_t *)(&val.val32), 4);
        ret = val.val32;
        break;
      default: ;
    }
  } else {
    IVERR("config_read: %s not exist!", name);
  }
  return ret;
}

void config_write(const char * name, const config_val_t * val)
{
  uint32_t off = 0, size = 0;
  const config_slot_t * p = find_config(name, &off);
  if(p) {
    switch (p->type) {
      case CONFIG_TYPE_UINT8:
        BSP_ROM_Write(off, (uint8_t *)(&val->val8), 1);
        break;
      case CONFIG_TYPE_UINT16:
        BSP_ROM_Write(off, (uint8_t *)(&val->val16), 2);
        break;
      case CONFIG_TYPE_UINT32:
        BSP_ROM_Write(off, (uint8_t *)(&val->val32), 4);
        break;
      case CONFIG_TYPE_BLOB:
        BSP_ROM_Write(off, (uint8_t *)(&val->valblob.len), sizeof(val->valblob.len));
        BSP_ROM_Write(off + sizeof(val->valblob.len), (uint8_t *)(val->valblob.body), val->valblob.len);
      default: ;
    }
  } else {
    IVERR("config_write: %s not exist!", name);
  }
}
