#include "rh_temp.h"
#include "terasic_includes.h"
#include "I2C_core.h"

// th: temperature & humidity
bool th_Read_Measure_Data(int nRegisterNo, alt_u8 *pData8, int nReadLength);
bool th_reset(alt_u32 I2C_Controller_Base);
bool th_dump_chip_info(alt_u32 I2C_Controller_Base);

bool th_write_register(alt_u8 RegisterPorinter, alt_u16 Data16);
bool th_read_register(alt_u8 RegisterPorinter, alt_u16 *pData16);

void RH_Temp_Init(alt_u32 I2C_Controller_Base)
{
	const int ref_clk = 50*1000*1000; // 50Mhz
	const int i2c_clk = 400*1000; // 400KHz
//	MPU9250_I2C_Controller_Base = I2C_Controller_Base;
	oc_i2c_init_ex(I2C_Controller_Base, ref_clk, i2c_clk);

    // reset
	bool bPass;
    bPass = th_reset(RH_TEMP_I2C_OPENCORES_BASE);
    if (!bPass){
    	bPass = th_reset(RH_TEMP_I2C_OPENCORES_BASE); // try again
    	if (!bPass){
    		printf("Failed to reset!\r\n");
    	}
    }

    //th_dump_chip_info(RH_TEMP_I2C_OPENCORES_BASE);


}

bool RH_Temp_Read_Device_ID(alt_u16 *pID16){
	bool bPass;
	alt_u8 szData8[2];

	bPass =  OC_I2C_Read(RH_TEMP_I2C_OPENCORES_BASE, RH_Temp_DEVICE_ADDR, R_DEVICE_ID, szData8, sizeof(szData8));
	if (bPass){
		*pID16 = (szData8[0] << 8) | szData8[1];
		printf("Device ID=%04xh\r\n", *pID16);
	}
	return bPass;


}


bool RH_Temp_Sensor_Init()
{
	bool bPass;

#if 0
    alt_u16 data;
	bPass = I2C_Read16(RH_TEMP_I2C_SCL_BASE,RH_TEMP_I2C_SDA_BASE,RH_Temp_DEVICE_ADDR,R_MANUFACTURER_ID,&data);
    if(bPass){
		printf("Manufacturer ID is %x\r\n",data);// 0x5449 expected
    }else{
	    printf("I2C MultipleRead Failed!\r\n");
    }

    bPass = I2C_Read16(RH_TEMP_I2C_SCL_BASE,RH_TEMP_I2C_SDA_BASE,RH_Temp_DEVICE_ADDR,R_DEVICE_ID,&data);
    if(bPass){
	    printf("Device ID is %x\r\n",data); // 0x1000 expected
    }else{
	    printf("I2C Read16 Failed!\r\n");
    }

    bPass =  Read_Configuration();
    if(!bPass){
		printf("I2C Read_Configuration Failed!\r\n");
	}
#endif


    //Temperature and Humidity are acquired in sequence, Temperature first.
    //Temperature Measurement Resolution: 14 bits
    //Humidity Measurement Resolution: 14 bits
	bPass =  Write_Configuration(1,0,0);
//    if(bPass)
//    {
//		printf("RH_Temp Sensor Set Configuration successful!\r\n");
//	}
//    else
//    {
//    	printf("RH_Temp Sensor Set Configuration Failed!\r\n");
//    }

    return bPass;

}

bool RH_Temp_Sensor_Read(float *fTemperature, float *fHumidity){
	bool bPass;
	alt_u8 szValue8[4];
	alt_u16 Value16;
	//int i;

	bPass =  th_Read_Measure_Data(R_TEMPERATURE, szValue8, sizeof(szValue8));
	if (bPass){
		//for(i=0;i<4;i++)
		//	printf("%02XH ", szValue8[i]);

		// temperature
		Value16 = (szValue8[0] << 8) | szValue8[1];
		*fTemperature = (float)Value16/397.188 -40.0;

		// humidity
		Value16 = (szValue8[2] << 8) | szValue8[3];
		*fHumidity = (float)((float)Value16/65536)*100;
	}

	return bPass;

}



bool Read_Configuration(void)
{
	bool bPass;
	alt_u16 data;

	bPass = th_read_register(R_CONFIGURATION, &data);


	if(bPass){

		printf("Read Configuration Data = 0x%04X \r\n",data);

		if(((data>>12)&0x0001) == 0) printf("Acquisition mode:Temperature or Humidity is acquired.\r\n ");
		else printf("Acquisition mode:Temperature and Humidity are acquired in sequence,Temperature first.\r\n ");

		if(((data>>10)&0x0001) == 0) printf("Temperature Measurement Resolution: 14 bit.\r\n ");
		else  printf("Temperature Measurement Resolution: 11 bit.\r\n ");

		if(((data>> 8)&0x0003) == 0) printf("Humidity Measurement Resolution: 14 bit.\r\n ");
		else if(((data>> 8)&0x0003) == 1) printf("Humidity Measurement Resolution: 11 bit.\r\n ");
		else  printf("Humidity Measurement Resolution: 8 bit.\r\n ");

	}else{
		printf("Failed to read configure data.\r\n");
	}

	return bPass;
}



bool Write_Configuration(alt_u8 acquisition_mode , alt_u8 temp_resolution,alt_u8 humidity_resolution)
{
	bool bPass;
	alt_u16 Data16;


	Data16 = ((acquisition_mode&0x0001) <<12) + ((temp_resolution&0x0001)<<10) + ((humidity_resolution&0x0003)<<8);

	//printf("Write Configure Register: %04xh\r\n", Data16);

	bPass = th_write_register(R_CONFIGURATION, Data16);

	if (bPass)
		usleep(20*1000); // wait ready

	return bPass;
}


bool th_Read_Measure_Data(int nRegisterNo, alt_u8 *pData8, int nReadLength)
{
	bool bPass;
	const bool bSTOP = TRUE; // need stop so the ready pin can assert when measure is done

	// write address
	bPass = OC_I2C_Write_Addr(RH_TEMP_I2C_OPENCORES_BASE,RH_Temp_DEVICE_ADDR,nRegisterNo, bSTOP);

	// check hardware pin DRDYn to make sure measure is done
	if (bPass){ // check ready hardware pin - DRDYn
		//
		bool bTimeout = false;
		alt_u32 TimeStart;

		bPass = false;
		TimeStart = alt_nticks();
		bTimeout = false;
		while(!bPass && !bTimeout){
			if ((IORD(RH_TEMP_DRDY_N_BASE, 0x00) & 0x01) == 0x00)
				bPass = TRUE;
			else if ((alt_nticks() - TimeStart) > alt_ticks_per_second()/10) // 0.1 second for timeout
				bTimeout = TRUE;
		}
		if (!bPass)
			printf("Warning: DRDYn pin not ready (high)!\r\n");
		bPass = TRUE;
	}

	// read measured data
	if (bPass){
		bPass =  OC_I2C_Read_Continue(RH_TEMP_I2C_OPENCORES_BASE,RH_Temp_DEVICE_ADDR, pData8, nReadLength);

	}



	return bPass;
}


bool th_reset(alt_u32 I2C_Controller_Base){
	bool bPass;

	bPass = th_write_register(R_CONFIGURATION, 0x8000);
	return bPass;
}


bool th_dump_chip_info(alt_u32 I2C_Controller_Base){
	alt_u16 Data16;
	bool bPass;

	// R_DEVICE_ID
	bPass = th_read_register(R_DEVICE_ID, &Data16);
	if (bPass){
		printf("DEVICE ID=%04xh(0x1000 expected)\r\n", Data16);
	}else{
		printf("Failed to read DEVICE ID\r\n");
	}

	// R_DEVICE_ID
	bPass = th_read_register(R_MANUFACTURER_ID, &Data16);
	if (bPass){
		printf("MANUFACTURER ID=%04xh(0x5449 expected)\r\n", Data16);
	}else{
		printf("Failed to read MANUFACTURER ID\r\n");
	}

	// configure Data
	bPass =  th_read_register(R_CONFIGURATION, &Data16);
	if (bPass){
		printf("Configure Data=%04xh\r\n", Data16);
	}else{
		printf("Failed to read configure data\r\n");
	}

	return bPass;
}


bool th_write_register(alt_u8 RegisterPorinter, alt_u16 Data16){
	bool bPass;
	alt_u8 szData8[2];

	szData8[0] = (Data16 >> 8) & 0xFF;
	szData8[1] = Data16 & 0xFF;
	bPass = OC_I2C_Write(RH_TEMP_I2C_OPENCORES_BASE, RH_Temp_DEVICE_ADDR, RegisterPorinter, szData8, sizeof(szData8));

	return bPass;

}


bool th_read_register(alt_u8 RegisterPorinter, alt_u16 *pData16){
	bool bPass;
	alt_u8 szData8[2];

	bPass = OC_I2C_Read(RH_TEMP_I2C_OPENCORES_BASE, RH_Temp_DEVICE_ADDR, RegisterPorinter, szData8, sizeof(szData8));
	if (bPass){
		*pData16 = (szData8[0] << 8) | szData8[1];
	}

	return bPass;


}
