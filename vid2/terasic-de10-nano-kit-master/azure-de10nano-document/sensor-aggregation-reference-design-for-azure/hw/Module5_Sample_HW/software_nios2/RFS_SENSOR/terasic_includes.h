/*
 * Modified 
 * Copyright (C) 2021 Intel Corporation 
 * Licensed under the MIT license. See LICENSE file in the project root for
 * full license information.

 * Origin(Terasic)
 * See Copyright.txt file in the <project root/LICENSES/origin/de10nano-samples> for
 * full license information.
 * terasic_includes.h
 *
 *  Created on: 2017��1��5��
 *      Author: Administrator
 */

#ifndef TERASIC_INCLUDES_H_
#define TERASIC_INCLUDES_H_

#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <stddef.h>
#include <unistd.h>  // usleep (unix standard?)
#include "sys/alt_flash.h"
#include "sys/alt_flash_types.h"
#include "io.h"
#include "alt_types.h"  // alt_u32
#include "altera_avalon_pio_regs.h" //IOWR_ALTERA_AVALON_PIO_DATA
#include "sys/alt_irq.h"  // interrupt
#include "sys/alt_alarm.h" // time tick function (alt_nticks(), alt_ticks_per_second())
#include "sys/alt_timestamp.h"
#include "sys/alt_stdio.h"
#include "system.h"
#include <fcntl.h>


#define DEBUG_DUMP  /*printf */

#include <stdbool.h>
enum {FALSE, TRUE};
//typedef int bool;
//#define TRUE    1
//#define FALSE   0


typedef alt_u8 	uint8_t;
typedef alt_8 	int8_t;
typedef alt_u16 uint16_t;
typedef alt_16 	int16_t;
typedef alt_u32 uint32_t;
typedef alt_32 	int32_t;

// MACRO name conversion table due to Qsys design hierarchy change
#define LIGHT_I2C_OPENCORES_BASE    NIOS_SYSTEM_LIGHT_I2C_OPENCORES_BASE
#define MPU_I2C_OPENCORES_BASE      NIOS_SYSTEM_MPU_I2C_OPENCORES_BASE
#define RH_TEMP_I2C_OPENCORES_BASE  NIOS_SYSTEM_RH_TEMP_I2C_OPENCORES_BASE
#define RH_TEMP_DRDY_N_BASE         NIOS_SYSTEM_RH_TEMP_DRDY_N_BASE

#endif /* TERASIC_INCLUDES_H_ */
