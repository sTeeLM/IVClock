#ifndef __IVCLOCK_RTC_H__
#define __IVCLOCK_RTC_H__

#include <stdint.h>
#include "cext.h"

enum BSP_RTC_Data_Type {
  RTC_TYPE_TIME    = 0, // 时间
  RTC_TYPE_DATE    = 1, // 日期
  RTC_TYPE_ALARM0  = 2, // 闹钟0
  RTC_TYPE_ALARM1  = 3, // 闹钟1
  RTC_TYPE_TEMP    = 4, // 温度
  RTC_TYPE_CTL     = 5,
};

enum BSP_RTC_Alarm_Mode
{
  RTC_ALARM0_MOD_PER_SEC                 = 0,
  RTC_ALARM0_MOD_MATCH_SEC               = 1,  
  RTC_ALARM0_MOD_MATCH_MIN_SEC           = 2, 
  RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC      = 3, 
  RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC = 4,   
  RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC  = 5,
  RTC_ALARM0_MOD_CNT                     = 6, 
  RTC_ALARM1_MOD_PER_MIN                 = 7,
  RTC_ALARM1_MOD_MATCH_MIN               = 8,  
  RTC_ALARM1_MOD_MATCH_HOUR_MIN          = 9, 
  RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN     = 10,   
  RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN      = 11,     
};

enum BSP_RTC_Square_Rate
{
  RTC_SQUARE_RATE_1HZ    = 0,
  RTC_SQUARE_RATE_1024HZ = 1,
  RTC_SQUARE_RATE_4096HZ = 2,
  RTC_SQUARE_RATE_8192HZ = 3,    
};

enum BSP_RTC_Alarm_Index {
  RTC_ALARM0 = 0,
  RTC_ALARM1
};


int8_t BSP_RTC_Init(void);
bool BSP_RTC_Is_Lt_Timer(void);
void BSP_RTC_Set_Lt_Timer(bool enable);

void BSP_RTC_Enter_Powersave(void);
void BSP_RTC_Leave_Powersave(void);

void BSP_RTC_Read_Data(enum BSP_RTC_Data_Type type);
void BSP_RTC_Write_Data(enum BSP_RTC_Data_Type type);

// 在BSP_RTC_Read_Data(RTC_TYPE_TIME)之后调用
uint8_t BSP_RTC_Time_Get_Hour(void);
void BSP_RTC_Time_Set_Hour(uint8_t hour);
void BSP_RTC_Time_Set_Hour_12(bool enable);
bool BSP_RTC_Time_Get_Hour_12(void);
uint8_t BSP_RTC_Time_Get_Min(void);
void BSP_RTC_Time_Set_Min(uint8_t min);
uint8_t BSP_RTC_Time_Get_Sec(void);
void BSP_RTC_Time_Set_Sec(uint8_t sec);

// 在BSP_RTC_Read_Data(RTC_TYPE_DATE)之后调用
uint8_t BSP_RTC_Date_Get_Year(void);
void BSP_RTC_Date_Set_Year(uint8_t year);
uint8_t BSP_RTC_Date_Get_Month(void);
void BSP_RTC_Date_Set_Month(uint8_t month);
uint8_t BSP_RTC_Date_Get_Date(void);
bool BSP_RTC_Date_Set_Date(uint8_t date);
uint8_t BSP_RTC_Date_Get_Day(void);
void BSP_RTC_Date_Set_Day(uint8_t day);

// 在BSP_RTC_Read_Data(RTC_TYPE_ALARM0)或者RTC_TYPE_ALARM1之后调用
bool BSP_RTC_Alarm_Get_Hour_12();
void BSP_RTC_Alarm_Set_Hour_12(bool enable);
uint8_t BSP_RTC_Alarm_Get_Day(void);
void BSP_RTC_Alarm_Set_Day(uint8_t day);
uint8_t BSP_RTC_Alarm_Get_Date(void);
void BSP_RTC_Alarm_Set_Date(uint8_t date);
uint8_t BSP_RTC_Alarm_Get_Hour(void);
void BSP_RTC_Alarm_Set_Hour(uint8_t hour);
uint8_t BSP_RTC_Alarm_Get_Min();
void BSP_RTC_Alarm_Set_Min( uint8_t min);
uint8_t BSP_RTC_Alarm_Get_Sec();
void BSP_RTC_Alarm_Set_Sec( uint8_t sec);
enum BSP_RTC_Alarm_Mode BSP_RTC_Alarm_Get_Mode(void);
void BSP_RTC_Alarm_Set_Mode(enum BSP_RTC_Alarm_Mode mode);
enum BSP_RTC_Alarm_Mode BSP_RTC_Alarm_Get_Mode(void);
const char * BSP_RTC_Alarm_Get_Mode_Str(void);

// 在rtc_read_data(RTC_TYPE_TEMP)之后调用
bool BSP_RTC_Get_Temperature(uint8_t * integer, uint8_t * flt);



// 在rtc_read_data(RTC_TYPE_CTL)之后调用
void BSP_RTC_Enable_Alarm_Int(enum BSP_RTC_Alarm_Index index, bool enable);
bool BSP_RTC_Test_Alarm_Int(enum BSP_RTC_Alarm_Index index);
bool BSP_RTC_Test_Alarm_Int_Flag(enum BSP_RTC_Alarm_Index index);
void BSP_RTC_Clr_Alarm_Int_Flag(enum BSP_RTC_Alarm_Index index);

// EOSC:
// Enable Oscillator (EOSC). When set to logic 0, the oscillator is started. 
// When set to logic 1, the oscilla- tor is stopped when the DS3231 switches to VBAT. 
// This bool is clear (logic 0) when power is first applied. 
// When the DS3231 is powered by VCC, the oscillator is always on regardless of the status of the EOSC bool. 
// When EOSC is disabled, all register data is static.
bool BSP_RTC_Test_Eosc(void); 
void BSP_RTC_Set_Eosc(bool val);

// BBSQW:
// Battery-Backed Square-Wave Enable (BBSQW). 
// When set to logic 1 and the DS3231 is being powered by the VBAT pin, 
// this bool enables the square- wave or interrupt output when VCC is absent. 
// When BBSQW is logic 0, the INT/SQW pin goes high imped- ance when VCC falls below the power-fail trip point. 
// This bool is disabled (logic 0) when power is first applied.
bool BSP_RTC_Test_Bbsqw(void);
void BSP_RTC_Set_Bbsqw(bool val);

// CONV:
// Convert Temperature (CONV). 
// Setting this bool to 1 forces the temperature sensor to convert the temperature 
// into digital code and execute the TCXO algorithm to update the capacitance array to the oscillator. 
// This can only happen when a conversion is not already in progress.
// The user should check the status bool BSY before forcing the controller to start a new TCXO execution.
// A user-initiated temperature conversion does not affect the internal 64-second update cycle.
// A user-initiated temperature conversion does not affect the BSY bool for approximately 2ms. 
// The CONV bool remains at a 1 from the time it is written until the conver- sion is finished, 
// at which time both CONV and BSY go to 0. 
// The CONV bool should be used when monitoring the status of a user-initiated conversion.
bool BSP_RTC_Test_Conv(void);
void BSP_RTC_Set_Conv(bool val);

// RS:
// Rate Select (RS2 and RS1). These bits control the frequency of the square-wave output when
// the square wave has been enabled. 
// The following table shows the square-wave frequencies that can be select- ed with the RS bits. 
// These bits are both set to logic 1 (8.192kHz) when power is first applied.
enum BSP_RTC_Square_Rate BSP_RTC_Get_Square_Rate(void);
void BSP_RTC_Set_Square_Rate(enum BSP_RTC_Square_Rate rt);
const char * BSP_RTC_Get_Square_Rate_Str(void);

// INTCN:
// Interrupt Control (INTCN). 
// This bool controls the INT/SQW signal. 
// When the INTCN bool is set to logic 0, a square wave is output on the INT/SQW pin. 
// When the INTCN bool is set to logic 1, then a match between the timekeeping registers 
// and either of the alarm registers activates the INT/SQW output (if the alarm is also enabled). 
// The corresponding alarm flag is always set regardless of the state of the INTCN bool. 
// The INTCN bool is set to logic 1 when power is first applied.
bool BSP_RTC_Test_Intcn(void);
void BSP_RTC_Set_Intcn(bool val);

// OSF:
// Oscillator Stop Flag (OSF). A logic 1 in this bool indicates that the oscillator 
// either is stopped or was stopped for some period and may be used to judge the 
// validity of the timekeeping data. This bool is set to logic 1 any time that the oscillator stops.
// The following are exam- ples of conditions that can cause the OSF bool to be set:
// 1) The first time power is applied.
// 2) The voltages present on both VCC and VBAT are insufficient to support oscillation.
// 3) The EOSC bool is turned off in battery-backed mode.
// 4) External influences on the crystal (i.e., noise, leakage, etc.).
// This bool remains at logic 1 until written to logic 0.
bool BSP_RTC_Test_Osf(void);
void BSP_RTC_Set_Osf(bool val);

// EN32kHz:
// Enable 32kHz Output (EN32kHz). 
// This bool controls the status of the 32kHz pin. 
// When set to logic 1, the 32kHz pin is enabled and outputs a 32.768kHz squarewave signal. 
// When set to logic 0, the 32kHz pin goes to a high-impedance state. 
// The initial power-up state of this bool is logic 1, and a 32.768kHz square-wave signal 
// appears at the 32kHz pin after a power source is applied to the DS3231 (if the oscillator is running).
bool BSP_RTC_Test_En32khz(void);
void BSP_RTC_Set_En32khz(bool val);

// BSY:
// Busy (BSY). This bool indicates the device is busy executing TCXO functions.
// It goes to logic 1 when the conversion signal to the temperature 
// sensor is asserted and then is cleared when the device is in the 1-minute idle state.
bool BSP_RTC_Test_Bsy(void);

void BSP_RTC_Dump(void);
#endif