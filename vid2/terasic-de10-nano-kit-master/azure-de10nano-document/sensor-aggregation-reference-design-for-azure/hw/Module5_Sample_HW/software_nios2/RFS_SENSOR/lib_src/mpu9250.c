/*
 * mpu9250.c
 *
 *  Created on: 2016Äê11ÔÂ25ÈÕ
 *      Author: Administrator
 */

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "mpu9250.h"
#include "I2C_core.h"
#include "terasic_includes.h"

#define G_SI 9.80665

void MPU9250_Init(alt_u32 I2C_Controller_Base)
{
	const int ref_clk = 50*1000*1000; // 50Mhz
	const int i2c_clk = 400*1000; // 400KHz
//	MPU9250_I2C_Controller_Base = I2C_Controller_Base;
	oc_i2c_init_ex(I2C_Controller_Base, ref_clk, i2c_clk);
}


/*-----------------------------------------------------------------------------------------------
                                    REGISTER READ & WRITE
usage: use these methods to read and write MPU9250 registers over SPI
-----------------------------------------------------------------------------------------------*/

unsigned int WriteReg( uint8_t WriteAddr, uint8_t WriteData )
{
#if 1
	if (WriteAddr & READ_FLAG)
		return ReadReg(WriteAddr, WriteData);
	OC_I2C_Write(MPU_I2C_OPENCORES_BASE, MPU_DEVICE_ADDR, WriteAddr, &WriteData, sizeof(WriteData));
	return 0x00;
#else
    unsigned int temp_val;

    unsigned char tx[2] = {WriteAddr, WriteData};
	unsigned char rx[2] = {0};

	SPIdev::transfer("/dev/spidev0.1", tx, rx, 2);

    return rx[1];
#endif
}

//-----------------------------------------------------------------------------------------------

unsigned int  ReadReg( uint8_t WriteAddr, uint8_t WriteData )
{
#if 1
	unsigned char ReadData;
	OC_I2C_Read(MPU_I2C_OPENCORES_BASE, MPU_DEVICE_ADDR, WriteAddr, &ReadData, sizeof(ReadData));
	return ReadData;
#else
    return WriteReg(WriteAddr | READ_FLAG, WriteData);
#endif
}

//-----------------------------------------------------------------------------------------------
void ReadRegs( uint8_t ReadAddr, uint8_t *ReadBuf, unsigned int Bytes )
{
#if 1
	OC_I2C_Read(MPU_I2C_OPENCORES_BASE, MPU_DEVICE_ADDR, ReadAddr, ReadBuf, Bytes);
#else
    unsigned int  i = 0;

    unsigned char tx[255] = {0};
	unsigned char rx[255] = {0};

	tx[0] = ReadAddr | READ_FLAG;

	SPIdev::transfer("/dev/spidev0.1", tx, rx, Bytes + 1);

    for(i=0; i<Bytes; i++)
    	ReadBuf[i] = rx[i + 1];

    usleep(50);
#endif
}

/*-----------------------------------------------------------------------------------------------
                                ACCELEROMETER SCALE
usage: call this function at startup, after initialization, to set the right range for the
accelerometers. Suitable ranges are:
BITS_FS_2G
BITS_FS_4G
BITS_FS_8G
BITS_FS_16G
returns the range set (2,4,8 or 16)
-----------------------------------------------------------------------------------------------*/

unsigned int set_acc_scale(int scale)
{
	unsigned int temp_scale;
//	I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_ACCEL_CONFIG, scale);
    WriteReg(MPUREG_ACCEL_CONFIG, scale);

    switch (scale){
        case BITS_FS_2G:
            acc_divider=16384;
        break;
        case BITS_FS_4G:
            acc_divider=8192;
        break;
        case BITS_FS_8G:
            acc_divider=4096;
        break;
        case BITS_FS_16G:
            acc_divider=2048;
        break;
    }
//    I2C_Read(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_ACCEL_CONFIG, &temp_scale);
	temp_scale=WriteReg(MPUREG_ACCEL_CONFIG|READ_FLAG, 0x00);

    switch (temp_scale){
        case BITS_FS_2G:
            temp_scale=2;
        break;
        case BITS_FS_4G:
            temp_scale=4;
        break;
        case BITS_FS_8G:
            temp_scale=8;
        break;
        case BITS_FS_16G:
            temp_scale=16;
        break;
    }
    return temp_scale;
}

/*-----------------------------------------------------------------------------------------------
                                GYROSCOPE SCALE
usage: call this function at startup, after initialization, to set the right range for the
gyroscopes. Suitable ranges are:
BITS_FS_250DPS
BITS_FS_500DPS
BITS_FS_1000DPS
BITS_FS_2000DPS
returns the range set (250,500,1000 or 2000)
-----------------------------------------------------------------------------------------------*/

unsigned int set_gyro_scale(int scale)
{
    unsigned int temp_scale;
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_GYRO_CONFIG, scale);
    WriteReg(MPUREG_GYRO_CONFIG, scale);
    switch (scale){
        case BITS_FS_250DPS:
            gyro_divider=131;
        break;
        case BITS_FS_500DPS:
            gyro_divider=65.5;
        break;
        case BITS_FS_1000DPS:
            gyro_divider=32.8;
        break;
        case BITS_FS_2000DPS:
            gyro_divider=16.4;
        break;
    }
//    I2C_Read(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_GYRO_CONFIG, &temp_scale);
    temp_scale=WriteReg(MPUREG_GYRO_CONFIG|READ_FLAG, 0x00);
    switch (temp_scale){
        case BITS_FS_250DPS:
            temp_scale=250;
        break;
        case BITS_FS_500DPS:
            temp_scale=500;
        break;
        case BITS_FS_1000DPS:
            temp_scale=1000;
        break;
        case BITS_FS_2000DPS:
            temp_scale=2000;
        break;
    }
    return temp_scale;
}

/*-----------------------------------------------------------------------------------------------
                                    INITIALIZATION
usage: call this function at startup, giving the sample rate divider (raging from 0 to 255) and
low pass filter value; suitable values are:
BITS_DLPF_CFG_256HZ_NOLPF2
BITS_DLPF_CFG_188HZ
BITS_DLPF_CFG_98HZ
BITS_DLPF_CFG_42HZ
BITS_DLPF_CFG_20HZ
BITS_DLPF_CFG_10HZ
BITS_DLPF_CFG_5HZ
BITS_DLPF_CFG_2100HZ_NOLPF
returns 1 if an error occurred
-----------------------------------------------------------------------------------------------*/

#define MPU_InitRegNum 16

bool MPU9250_initialize()
{
	alt_u8 i = 0;
	int low_pass_filter = 0x01;
	alt_u8 MPU_Init_Data[MPU_InitRegNum][2] =
	{
	        //{0x80, MPUREG_PWR_MGMT_1},     // Reset Device - Disabled because it seems to corrupt initialisation of AK8963
	        {0x01, MPUREG_PWR_MGMT_1},     // Clock Source
	        {0x00, MPUREG_PWR_MGMT_2},     // Enable Acc & Gyro
	        {low_pass_filter, MPUREG_CONFIG},         // Use DLPF set Gyroscope bandwidth 184Hz, temperature bandwidth 188Hz
	        {0x18, MPUREG_GYRO_CONFIG},    // +-2000dps
	        {0x08, MPUREG_ACCEL_CONFIG},   // +-4G
	        {0x09, MPUREG_ACCEL_CONFIG_2}, // Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz
	        {0x30, MPUREG_INT_PIN_CFG},    //
	        //{0x40, MPUREG_I2C_MST_CTRL},   // I2C Speed 348 kHz
	        //{0x20, MPUREG_USER_CTRL},      // Enable AUX
	        {0x20, MPUREG_USER_CTRL},       // I2C Master mode
	        {0x0D, MPUREG_I2C_MST_CTRL}, //  I2C configuration multi-master  IIC 400KHz

	        {AK8963_I2C_ADDR, MPUREG_I2C_SLV0_ADDR},  //Set the I2C slave addres of AK8963 and set for write.
	        //{0x09, MPUREG_I2C_SLV4_CTRL},
	        //{0x81, MPUREG_I2C_MST_DELAY_CTRL}, //Enable I2C delay

	        {AK8963_CNTL2, MPUREG_I2C_SLV0_REG}, //I2C slave 0 register address from where to begin data transfer
	        {0x01, MPUREG_I2C_SLV0_DO}, // Reset AK8963
	        {0x81, MPUREG_I2C_SLV0_CTRL},  //Enable I2C and set 1 byte

	        {AK8963_CNTL1, MPUREG_I2C_SLV0_REG}, //I2C slave 0 register address from where to begin data transfer
	        {0x12, MPUREG_I2C_SLV0_DO}, // Register value to continuous measurement in 16bit
	        {0x81, MPUREG_I2C_SLV0_CTRL}  //Enable I2C and set 1 byte
	};

	for(i = 0; i < MPU_InitRegNum; i++)
	{
//		I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPU_Init_Data[i][1], MPU_Init_Data[i][0]);
		WriteReg(MPU_Init_Data[i][1], MPU_Init_Data[i][0]);
	    usleep(100000);  //I2C must slow down the write speed, otherwise it won't work
	}

	set_acc_scale(BITS_FS_16G);
	set_gyro_scale(BITS_FS_2000DPS);

	calib_mag();

	return 0;
}

/*-----------------------------------------------------------------------------------------------
                                READ ACCELEROMETER CALIBRATION
usage: call this function to read accelerometer data. Axis represents selected axis:
0 -> X axis
1 -> Y axis
2 -> Z axis
returns Factory Trim value
-----------------------------------------------------------------------------------------------*/

void calib_acc()
{
	alt_u8 response[4];
    int temp_scale;
    //READ CURRENT ACC SCALE

//    I2C_Read(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_ACCEL_CONFIG, &temp_scale);
    temp_scale=WriteReg(MPUREG_ACCEL_CONFIG|READ_FLAG, 0x00);
    set_acc_scale(BITS_FS_8G);
    //ENABLE SELF TEST need modify
    //temp_scale=WriteReg(MPUREG_ACCEL_CONFIG, 0x80>>axis);

//    I2C_MultipleRead(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_SELF_TEST_X, response, 4);
    ReadRegs(MPUREG_SELF_TEST_X,response,4);
    calib_data[0]=((response[0]&11100000)>>3)|((response[3]&00110000)>>4);
    calib_data[1]=((response[1]&11100000)>>3)|((response[3]&00001100)>>2);
    calib_data[2]=((response[2]&11100000)>>3)|((response[3]&00000011));

    set_acc_scale(temp_scale);
}

//-----------------------------------------------------------------------------------------------

void calib_mag()
{
	alt_u8 response[3];
    float data;
    int i;

    WriteReg(MPUREG_I2C_SLV0_ADDR,AK8963_I2C_ADDR|READ_FLAG); //Set the I2C slave addres of AK8963 and set for read.
    WriteReg(MPUREG_I2C_SLV0_REG, AK8963_ASAX); //I2C slave 0 register address from where to begin data transfer
    WriteReg(MPUREG_I2C_SLV0_CTRL, 0x83); //Read 3 bytes from the magnetometer
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_ADDR, AK8963_I2C_ADDR|READ_FLAG);
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_REG, AK8963_ASAX);
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_CTRL, 0x83);


    //WriteReg(MPUREG_I2C_SLV0_CTRL, 0x81);    //Enable I2C and set bytes
    usleep(10000);
    //response[0]=WriteReg(MPUREG_EXT_SENS_DATA_01|READ_FLAG, 0x00);    //Read I2C
//    I2C_MultipleRead(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_EXT_SENS_DATA_00, response, 3);
    ReadRegs(MPUREG_EXT_SENS_DATA_00,response,3);

    //response=WriteReg(MPUREG_I2C_SLV0_DO, 0x00);    //Read I2C
    for(i=0; i<3; i++) {
        data=response[i];
        magnetometer_ASA[i]=((data-128)/256+1)*Magnetometer_Sensitivity_Scale_Factor;
    }
}

/*-----------------------------------------------------------------------------------------------
                                READ ACCELEROMETER
usage: call this function to read accelerometer data. Axis represents selected axis:
0 -> X axis
1 -> Y axis
2 -> Z axis
-----------------------------------------------------------------------------------------------*/

void read_acc()
{
	alt_u8 response[6];
	alt_16 bit_data;
    float data;
    int i;

//    I2C_MultipleRead(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_ACCEL_XOUT_H, response, 6);
    ReadRegs(MPUREG_ACCEL_XOUT_H,response,6);
    for(i=0; i<3; i++) {
        bit_data = ((alt_16)response[i*2] << 8) | response[i*2+1];
        data = (float)bit_data;
        accelerometer_data[i] = G_SI * data / acc_divider;
    }

}

/*-----------------------------------------------------------------------------------------------
                                READ GYROSCOPE
usage: call this function to read gyroscope data. Axis represents selected axis:
0 -> X axis
1 -> Y axis
2 -> Z axis
-----------------------------------------------------------------------------------------------*/

void read_gyro()
{
	alt_u8 response[6];
	alt_16 bit_data;
    float data;
    int i;

//    I2C_MultipleRead(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_GYRO_XOUT_H, response, 6);
    ReadRegs(MPUREG_GYRO_XOUT_H,response,6);
    for(i=0; i<3; i++) {
        bit_data = ((alt_16)response[i*2] << 8) | response[i*2+1];
        data = (float)bit_data;
        gyroscope_data[i] = (M_PI / 180) * data / gyro_divider;
    }

}

/*-----------------------------------------------------------------------------------------------
                                READ TEMPERATURE
usage: call this function to read temperature data.
returns the value in Â°C
-----------------------------------------------------------------------------------------------*/

void read_temp()
{
	alt_u8 response[2];
	alt_16 bit_data;
    float data;

//    I2C_MultipleRead(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_TEMP_OUT_H, response, 2);
    ReadRegs(MPUREG_TEMP_OUT_H,response,2);

    bit_data=((alt_16)response[0]<<8)|response[1];
    data=(float)bit_data;
    temperature=(data/340)+36.53;
}

//-----------------------------------------------------------------------------------------------

void read_mag()
{
	alt_u8 response[7];
	alt_16 bit_data;
    float data;
    int i;

    WriteReg(MPUREG_I2C_SLV0_ADDR,AK8963_I2C_ADDR|READ_FLAG); //Set the I2C slave addres of AK8963 and set for read.
    WriteReg(MPUREG_I2C_SLV0_REG, AK8963_HXL); //I2C slave 0 register address from where to begin data transfer
    WriteReg(MPUREG_I2C_SLV0_CTRL, 0x87); //Read 6 bytes from the magnetometer
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_ADDR, AK8963_I2C_ADDR|READ_FLAG);
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_REG, AK8963_HXL);
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_CTRL, 0x87);

    usleep(10000);
//    I2C_MultipleRead(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_EXT_SENS_DATA_00, response, 7);
    ReadRegs(MPUREG_EXT_SENS_DATA_00,response,7);
    //must start your read from AK8963A register 0x03 and read seven bytes so that upon read of ST2 register 0x09 the AK8963A will unlatch the data registers for the next measurement.
    for(i=0; i<3; i++) {
        bit_data=((alt_16)response[i*2+1]<<8)|response[i*2];
        data=(float)bit_data;
        magnetometer_data[i]=data*magnetometer_ASA[i];
    }
}

//-----------------------------------------------------------------------------------------------

void read_all()
{
	alt_u8 response[21];
	alt_16 bit_data;
    float data;
    int i;

    //Send I2C command at first
    WriteReg(MPUREG_I2C_SLV0_ADDR,AK8963_I2C_ADDR|READ_FLAG); //Set the I2C slave addres of AK8963 and set for read.
    WriteReg(MPUREG_I2C_SLV0_REG, AK8963_HXL); //I2C slave 0 register address from where to begin data transfer
    WriteReg(MPUREG_I2C_SLV0_CTRL, 0x87); //Read 7 bytes from the magnetometer
    //must start your read from AK8963A register 0x03 and read seven bytes so that upon read of ST2 register 0x09 the AK8963A will unlatch the data registers for the next measurement.
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_ADDR, AK8963_I2C_ADDR|READ_FLAG);
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_REG, AK8963_HXL);
//    I2C_Write(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_I2C_SLV0_CTRL, 0x87);


    //wait(0.001);
//    I2C_MultipleRead(MPU_I2C_SCL_BASE, MPU_I2C_SDA_BASE, MPU_DEVICE_ADDR, MPUREG_ACCEL_XOUT_H, response, 21);
    ReadRegs(MPUREG_ACCEL_XOUT_H,response,21);
    //Get accelerometer value
    for(i=0; i<3; i++) {
        bit_data = ((alt_16)response[i*2] << 8)|response[i*2+1];
        data = (float)bit_data;
        accelerometer_data[i] = G_SI * data / acc_divider;
    }
    //Get temperature
    bit_data = ((alt_16)response[i*2] << 8) | response[i*2+1];
    data = (float)bit_data;
    temperature = ((data - 21) / 333.87) + 21;
    //Get gyroscope value
    for(i=4; i<7; i++) {
        bit_data = ((alt_16)response[i*2] << 8) | response[i*2+1];
        data = (float)bit_data;
        gyroscope_data[i-4] = (M_PI / 180) * data / gyro_divider;
    }
    //Get Magnetometer value
    for(i=7; i<10; i++) {
        bit_data = ((alt_16)response[i*2+1] << 8) | response[i*2];
        data = (float)bit_data;
        magnetometer_data[i-7] = data * magnetometer_ASA[i-7];
    }
}

/*-----------------------------------------------------------------------------------------------
                                         GET VALUES
usage: call this functions to read and get values
returns accel, gyro and mag values
-----------------------------------------------------------------------------------------------*/

void getMotion9(float *ax, float *ay, float *az, float *gx, float *gy, float *gz, float *mx, float *my, float *mz)
{
    read_all();
    *ax = accelerometer_data[0];
    *ay = accelerometer_data[1];
    *az = accelerometer_data[2];
    *gx = gyroscope_data[0];
    *gy = gyroscope_data[1];
    *gz = gyroscope_data[2];
    *mx = magnetometer_data[0];
    *my = magnetometer_data[1];
    *mz = magnetometer_data[2];
}

//-----------------------------------------------------------------------------------------------

void getMotion6(float *ax, float *ay, float *az, float *gx, float *gy, float *gz)
{
    read_acc();
    read_gyro();
    *ax = accelerometer_data[0];
    *ay = accelerometer_data[1];
    *az = accelerometer_data[2];
    *gx = gyroscope_data[0];
    *gy = gyroscope_data[1];
    *gz = gyroscope_data[2];
}
