#include "terasic_includes.h"

#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

#define DEVICE_ADDR     0x52
//#define DEVICE_ADDR     0x92

//APDS REGISTER MAP
//#define COMMAND        0xA0
#define COMMAND_CMD     0x80
#define COMMAND_CLEAR   0x40
#define COMMAND_WORD    0x20

#define ID_REG         0x0A
#define CONTROL_REG    0x00
#define THRESHLOWLOW_REG     0x02
#define THRESHLOWHIGH_REG    0x03
#define THRESHHIGHLOW_REG    0x04
#define THRESHHIGHHIGH_REG   0x05
#define INTERRUPT_REG        0x06
#define TIMING_REG           0x01

#define DATA0LOW       0x0C
#define DATA0HIGH      0x0D
#define DATA1LOW       0x0E
#define DATA1HIGH      0x0F


//public API
void Light_Init(alt_u32 I2C_Controller_Base); // init i2c ip core
void Light_GetID(alt_u8 *data);
bool Light_PowerSwitch(bool bSwitch);
bool Light_Get_ADCData0(alt_u16 *pChannelData);
bool Light_Get_ADCData1(alt_u16 *pChannelData);
bool Get_light(alt_u16 *light0, alt_u16 *light1);
bool Light_SetThreshLow(alt_u8 *pData);
bool Light_SetThreshHigh(alt_u8 *pData);
bool Light_SetInterrupt(alt_u8 data);
void Light_GetInterrupt(alt_u8 *data);
bool Light_SetTiming(alt_u8 data);
void Light_GetTiming(alt_u8 *data);

#endif /*LIGHT_SENSOR_H_*/
