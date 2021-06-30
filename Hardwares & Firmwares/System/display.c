#include "display.h"

void display_init(void)
{

}

void display_format_clock(struct clock_struct * clk, enum clock_display_mode display_mode)
{
//    if(display_mode == CLOCK_DISPLAY_MODE_HHMMSS) {
//      hour = clk.hour;
//      is_pm = 0;
//      if(clk_is12 && clk.hour > 12) {
//        hour -= 12;
//        is_pm = 1;
//      } else if(clk_is12 && clk.hour == 12) { // ??12??PM??
//        is_pm = 1;
//      }
//      
//      if(hour / 10 != 0) {
//        led_data[5] = led_code[hour / 10 + 4] | (is_pm ? 0 : 0x80);
//      } else {
//        led_data[5] = 0x7F | (is_pm ? 0 : 0x80);
//      }
//      led_data[4] = led_code[hour % 10 + 4];
//      led_data[3] = led_code[clk.min / 10 + 4];
//      led_data[2] = led_code[clk.min % 10 + 4];
//      led_data[1] = led_code[clk.sec / 10 + 4]; 
//      led_data[0] = led_code[clk.sec % 10 + 4]|0x80;
//    } else if(display_mode == CLOCK_DISPLAY_MODE_YYMMDD) {
//      led_data[5] = led_code[clk.year / 10 + 4] |0x80;
//      led_data[4] = led_code[clk.year % 10 + 4];
//      led_data[3] = led_code[(clk.mon + 1) / 10 + 4] |0x80;
//      led_data[2] = led_code[(clk.mon + 1) % 10 + 4];
//      led_data[1] = led_code[(clk.date + 1) / 10 + 4] |0x80; 
//      led_data[0] = led_code[(clk.date + 1) % 10 + 4]|0x80;
//    } else {
//      led_data[5] = led_code[0x18]|0x80; // D
//      led_data[4] = led_code[0x15]|0x80; // A
//      led_data[3] = led_code[0x2D]|0x80; // Y
//      led_data[2] = led_code[0x01]|0x80; // -
//      led_data[1] = led_code[(clk.day + 1) / 10 + 4] |0x80; 
//      led_data[0] = led_code[(clk.day + 1) % 10 + 4] |0x80;
//    }
}

void display_format_timer(struct timer_struct * tmr, enum timer_display_mode display_mode)
{
//  if(tmr_display) {
//    ms10 = (uint8_t)(((float)(tmr[0].ms39) * 3.9) / 10);
//    if(tmr_disp_mmssms) {    
//      led_data[5] = led_code[(tmr[0].min / 10) + 4]; 
//      led_data[4] = led_code[(tmr[0].min % 10) + 4];      
//      led_data[3] = led_code[(tmr[0].sec / 10) + 4]; 
//      led_data[2] = led_code[(tmr[0].sec % 10) + 4];    
//      led_data[1] = led_code[(ms10 / 10) + 4]; 
//      led_data[0] = led_code[(ms10 % 10) + 4]; 
//    } else {
//      led_data[5] = led_code[(tmr[0].hour / 10) + 4]; 
//      led_data[4] = led_code[(tmr[0].hour % 10) + 4];      
//      led_data[3] = led_code[(tmr[0].min / 10) + 4]; 
//      led_data[2] = led_code[(tmr[0].min % 10) + 4];    
//      led_data[1] = led_code[(tmr[0].sec / 10) + 4]; 
//      led_data[0] = led_code[(tmr[0].sec % 10) + 4]; 
//    }
//    led_data[0] |= 0x80;
//    led_data[5] |= 0x80;  
//  }
}

bool display_is_powersave(void)
{
	return 0;
}

