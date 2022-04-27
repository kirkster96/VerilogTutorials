/*
 * Modified 
 * Copyright (C) 2021 Intel Corporation 
 * Licensed under the MIT license. See LICENSE file in the project root for
 * full license information.

 * Origin(Terasic)
 * See Copyright.txt file in the <project root/LICENSES/origin/de10nano-samples> for
 * full license information.

 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <unistd.h>
#include <fcntl.h>

#include "terasic_includes.h"
#include "light_sensor.h"
#include "rh_temp.h"
#include "shared_mem_def.h"
#include "math.h"

void getMotion9(float *ax, float *ay, float *az, float *gx, float *gy, float *gz, float *mx, float *my, float *mz);
void MPU9250_Init(alt_u32 I2C_Controller_Base);
bool MPU9250_initialize();


static struct {
	int light[2][2];
	float lux[2];
	float fTemperature[2];
	float fHumidity[2];
	float a[3][2];
	float g[3][2];
	float m[3][2];
} thresh;

bool convert_light_lux(int light0, int light1, float *lux){
	float condition = (float)light1/(float)light0;
	if 			(0 < condition    && condition <= 0.50) *lux = (0.0304 * light0) - (0.062 * light0 * powf(condition,1.4));
	else if (0.50 < condition && condition <= 0.61) *lux = (0.0224 * light0) - (0.031 * light1);
	else if (0.61 < condition && condition <= 0.80) *lux = (0.0128 * light0) - (0.0153 * light1);
	else if (0.80 < condition && condition <= 1.30) *lux = (0.00146 * light1) - (0.00112 * light1);
  else *lux = 0;
	return TRUE;
}

void  set_thresh( void ){

  unsigned offst;
  union { 
  	unsigned u;
  	float f;
  } uf ;

  // Light threshold
  offst = LIGHT0_SENSOR_L_THRESH>>2;
	for (int i=0; i<2 ; i++) { thresh.light[0][i] = IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++); }
  offst = LIGHT1_SENSOR_L_THRESH>>2;
	for (int i=0; i<2 ; i++) { thresh.light[1][i] = IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++); }
	offst = LUX_SENSOR_L_THRESH >>2;
	for (int i=0; i<2 ; i++) {  uf.u = IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++);   thresh.lux[i] = uf.f;  }

  // Temp. HM. threshold
  offst = HUMIDI_SENSOR_L_THRESH>>2;
	for (int i=0; i<2 ; i++) {  uf.u = IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++);   thresh.fHumidity[i] = uf.f;  }
  offst = TEMPER_SENSOR_L_THRESH>>2;
	for (int i=0; i<2 ; i++) {  uf.u = IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++);   thresh.fTemperature[i] = uf.f;  }

  // 9-axis threshold
  offst = ACCEL_X_SENSOR_L_THRESH>>2;
  float *fp=&thresh.a[0][0];
	for (int i=0; i<6 ; i++) {  uf.u = IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++);   *fp++ = uf.f;  }

  offst = GYROS_X_SENSOR_L_THRESH>>2;
  fp=&thresh.g[0][0];
	for (int i=0; i<6 ; i++) {  uf.u = IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++);   *fp++ = uf.f;  }

  offst = MAGNE_X_SENSOR_L_THRESH>>2;
  fp=&thresh.m[0][0];
	for (int i=0; i<6 ; i++) {  uf.u = IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++);   *fp++ = uf.f;  }

}


//#define is_OutOfRange(a,THptr) ( ((a) < (*(THptr))) || ((a)>(*((THptr)+1))) )
#define is_OutOfRange(a,THptr) ( ((a) < ((THptr)[0])) || ((a)>(((THptr)[1]))) )
#define SET_OUT_OF_RANGE_LED(v)  IOWR(NIOS_SYSTEM_LED_PIO_BASE, 0, (v) )

bool check_light_threshold(int light0, int light1,float lux){
	bool rv=false;
	if ( 
			 //is_OutOfRange(light0, &thresh.light[0][0] ) || 
	     //is_OutOfRange(light1, &thresh.light[1][0] ) ||
	     is_OutOfRange(lux, &thresh.lux[0]         ) ) {
		rv = true;
	}
	return rv;
}
bool check_temp_hm_threshold(float temp, float hm){
	bool rv=false;
	if ( is_OutOfRange(temp, &thresh.fTemperature[0] ) || 
	     is_OutOfRange(hm,   &thresh.fHumidity[0]    ) ) {
		rv = true;
	}
	return rv;
}

bool check_temp_ax9_threshold(float a[3], float g[3], float m[3]){
	bool rv=false;
	int err=0;
	for ( int i=0;i<3; i++){ if ( is_OutOfRange(a[i], &thresh.a[i][0]))  err++;  }
	for ( int i=0;i<3; i++){ if ( is_OutOfRange(g[i], &thresh.g[i][0]))  err++;  }
	for ( int i=0;i<3; i++){ if ( is_OutOfRange(m[i], &thresh.m[i][0]))  err++;  }

	if (err) {
		rv=true;
	}
	return rv;
}

#define DEBUG

static bool g_clear_OOR_flag=true;

void Sensor_Report(bool print_flag){
  	bool bPass, bPass2;

  	////////////////////////////////
  	// report light sensor
  	alt_u16 light0 = 0, light1 = 0;
		float lux = 0;
  	bPass = Get_light(&light0, &light1);
	  if(bPass) {
			bPass2 = convert_light_lux(light0, light1, &lux);
		}
		else bPass2 = FALSE;
			
  	if (print_flag) {
			if(bPass2) {
	  		printf("light0 = %d, light1 = %d, lux = %.3f\r\n", light0, light1,lux);
			}
			else if (bPass) {
	  		printf("light0 = %d, light1 = %d,but getting lux value failed\r\n", light0, light1);
			}
	  	else{
	  		printf("get light0 and light1 values failed!\r\n");
	  	}
  	}

  	////////////////////////////////
  	// report HDC1000 temperature & humidity sensor
  	float fTemperature, fHumidity;
  	bPass = RH_Temp_Sensor_Read(&fTemperature, &fHumidity);
  	if (print_flag) {
	  	if (bPass){
			printf("Temperature: %.3f*C\r\n", fTemperature);
	    	printf("Humidity: %.3f%%\r\n",fHumidity);
	  	}else{
	  		printf("Failed to ready Temperature/Humidity sensor!\r\n");
	  	}
  	}


  	////////////////////////////////
  	// report mpu9250 9-axis sensor
  	float a[3];
  	float g[3];
  	float m[3];
  	getMotion9(
  	  &a[0], &a[1], &a[2], 
  	  &g[0], &g[1], &g[2], 
  	  &m[0], &m[1], &m[2] 
  	);


  	////////////////////////////////
    // Check out of range, and set LED
	static int out_of_range=0;
	if ( g_clear_OOR_flag ) out_of_range=0;
	if ( check_light_threshold(light0, light1,lux) ) 
		out_of_range |= 1<<0;
	if ( check_temp_hm_threshold(fTemperature, fHumidity) ) 
		out_of_range |= 1<<1;
	if ( check_temp_ax9_threshold( a, g, m) ) 
		out_of_range |= 1<<2;
	SET_OUT_OF_RANGE_LED(out_of_range);


  	////////////////////////////////
    // Print out sensor values
  	if (print_flag) {
	  	printf("9-axis info:\r\n");
	  	printf("ax = %.3f, ay = %.3f, az = %.3f\r\n", a[0], a[1], a[2]);
	  	printf("gx = %.3f, gy = %.3f, gz = %.3f\r\n", g[0], g[1], g[2]);
	  	printf("mx = %.3f, my = %.3f, mz = %.3f\r\n", m[0], m[1], m[2]);
  	}
  	if (print_flag) {
#ifdef DEBUG
  	   ////////////////////////////////
       // Print out threshold values
	    int *pi=&thresh.light[0][0];
	    printf(" TH:light "); for (int i=0; i<4; i++) { printf("%d ", *pi++);} printf("\n");
	    float *pf;
	    pf=&thresh.lux[0];
	    printf(" TH:Lux "); for (int i=0; i<2; i++) { printf("%.3f ", *pf++);} printf("\n");
	    pf=&thresh.fTemperature[0];
	    printf(" TH:Temp "); for (int i=0; i<2; i++) { printf("%.3f ", *pf++);} printf("\n");
	    pf=&thresh.fHumidity[0];
	    printf(" TH:HM "); for (int i=0; i<2; i++) { printf("%.3f ", *pf++);} printf("\n");
	    pf=&thresh.a[0][0];
	    printf(" TH:a* "); for (int i=0; i<6; i++) { printf("%.3f ", *pf++);} printf("\n");
	    pf=&thresh.g[0][0];
	    printf(" TH:g* "); for (int i=0; i<6; i++) { printf("%.3f ", *pf++);} printf("\n");
	    pf=&thresh.m[0][0];
	    printf(" TH:m* "); for (int i=0; i<6; i++) { printf("%.3f ", *pf++);} printf("\n");
#endif
	  	printf("\r\n");
  	}

  	////////////////////////////////
    // Set sensor values in shared memory
  	unsigned offst;

  	// Light0,1
  	offst = LIGHT0_SENSOR_VALUE>>2;
  	IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst, light0);
  	offst = LIGHT1_SENSOR_VALUE>>2;
  	IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst, light1);
  	unsigned *p;
		offst = LUX_SENSOR_VALUE>>2;
  	p =(unsigned *)&lux;    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst, *p);

  	// Temp. and Humidity
  	offst = HUMIDI_SENSOR_VALUE>>2;
  	p =(unsigned *)&fHumidity;    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst, *p);
  	offst = TEMPER_SENSOR_VALUE>>2;
  	p =(unsigned *)&fTemperature; IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst, *p);

  	// 9-Axes
  	offst = ACCEL_X_SENSOR_VALUE>>2;
  	p =(unsigned *)&a[0];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);
  	p =(unsigned *)&a[1];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);
  	p =(unsigned *)&a[2];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);
  	offst = GYROS_X_SENSOR_VALUE>>2;
  	p =(unsigned *)&g[0];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);
  	p =(unsigned *)&g[1];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);
  	p =(unsigned *)&g[2];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);
  	offst = MAGNE_X_SENSOR_VALUE>>2;
  	p =(unsigned *)&m[0];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);
  	p =(unsigned *)&m[1];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);
  	p =(unsigned *)&m[2];    IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, offst++, *p);

}

#define READ_COMMUNICATION_REGISTER IORD(NIOS_SYSTEM_SHARED_MEMORY_BASE, 1)
#define SEND_ACK_COMMUNICATION_REGISTER IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, 1, 0)
#define SEND_STATUS_REGISTER(x) IOWR(NIOS_SYSTEM_SHARED_MEMORY_BASE, 0, (x))

int main()
{
//	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	printf("Welcome to DE10_NANO RFS_SENSOR DEMO!\r\n");

  	bool bPass = FALSE;

  	////////////////////////////////////
  	// init light sensor i2c and power on
  	Light_Init(LIGHT_I2C_OPENCORES_BASE);
  	bPass = Light_PowerSwitch(TRUE);
  	if(bPass)
  		printf("light sensor power up successful!\r\n");
  	else
  		printf("light sensor power up failed!\r\n");

  	////////////////////////////////////
  	// init HDC1000: temperature and humidity sensor
  	RH_Temp_Init(RH_TEMP_I2C_OPENCORES_BASE);
  	bPass = RH_Temp_Sensor_Init();
  	if(bPass)
  		printf("Init HDC1000 successful!\r\n");
  	else
  		printf("Init HDC1000 failed!\r\n");



  	////////////////////////////////////
  	//init MPU9250 9-axis sensor
  	MPU9250_Init(MPU_I2C_OPENCORES_BASE);
  	MPU9250_initialize();
  	printf("\r\n");

    int loop_cnt=0;
  	while(1){ // report every second

  		SEND_STATUS_REGISTER(loop_cnt|0x80000000);
  		//Mode select
  		unsigned com_reg = READ_COMMUNICATION_REGISTER;
		if ( (com_reg&0xffff0000)== 0xaaaa0000 ) {
			SEND_ACK_COMMUNICATION_REGISTER; // Send ACK
			switch (com_reg&0xffff) {
			case 1:
				g_clear_OOR_flag = false; break;
			case 2:
				g_clear_OOR_flag = true; break;
			default :
				break;
			}
		}

  		set_thresh();  // Read threshold values from shared memory and set the thresh struct

  		// print out values once every 16 loops
  		bool print_flag = ((loop_cnt) & 0xF) == 0;
  		Sensor_Report(print_flag);

  		usleep(1000*50);
  		loop_cnt++;

  	}



  return 0;
}
