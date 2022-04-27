#include "terasic_includes.h"

#ifndef RH_TEMP_H_
#define RH_TEMP_H_

//#include "terasic_includes.h"
//#include "I2C.h"

#define R_TEMPERATURE      0x00
#define R_HUMIDITY         0x01
#define R_CONFIGURATION    0x02
#define R_SERILA_ID 	   0xFB
#define R_MANUFACTURER_ID  0xFE
#define R_DEVICE_ID        0xFF

#define RH_Temp_DEVICE_ADDR  0x80

void RH_Temp_Init(alt_u32 I2C_Controller_Base);
bool RH_Temp_Read_Device_ID(alt_u16 *pID16);
bool RH_Temp_Sensor_Init();

bool RH_Temp_Sensor_Read(float *fTemperature, float *fHumidity);
bool sensor_getdata();
bool Read_Configuration();
bool Write_Configuration(alt_u8 acquisition_mode , alt_u8 temp_resolution,alt_u8 humidity_resolution);


#endif /*RH_TEMP_H_*/
