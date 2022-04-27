/*
 * Modified 
 * Copyright (C) 2021 Intel Corporation 
 * Licensed under the MIT license. See LICENSE file in the project root for
 * full license information.

 * Origin(Terasic)
 * See Copyright.txt file in the <project root/LICENSES/origin/de10nano-samples> for
 * full license information.
 * shared_mem_def.h
 *
 */

#ifndef SHARED_MEM_DEF_H_
#define SHARED_MEM_DEF_H_

// define Address Map of shared memory
// Sensor values
#define LIGHT0_SENSOR_VALUE    (0x100)
#define LIGHT1_SENSOR_VALUE    (0x104)
#define LUX_SENSOR_VALUE       (0x108)
#define HUMIDI_SENSOR_VALUE    (0x10C)
#define TEMPER_SENSOR_VALUE    (0x110)
#define ACCEL_X_SENSOR_VALUE   (0x114)
//#define ACCEL_Y_SENSOR_VALUE   (0x118)
//#define ACCEL_Z_SENSOR_VALUE   (0x11C)
#define GYROS_X_SENSOR_VALUE   (0x120)
//#define GYROS_Y_SENSOR_VALUE   (0x124)
//#define GYROS_Z_SENSOR_VALUE   (0x128)
#define MAGNE_X_SENSOR_VALUE   (0x12C)
//#define MAGNE_Y_SENSOR_VALUE   (0x130)
//#define MAGNE_Z_SENSOR_VALUE   (0x134)

// Threshold values
#define LIGHT0_SENSOR_L_THRESH     (0x200)
#define LIGHT1_SENSOR_L_THRESH     (0x208)
#define LUX_SENSOR_L_THRESH        (0x210)
#define HUMIDI_SENSOR_L_THRESH     (0x218)
#define TEMPER_SENSOR_L_THRESH     (0x220)
#define ACCEL_X_SENSOR_L_THRESH    (0x228)
#define GYROS_X_SENSOR_L_THRESH    (0x240)
#define MAGNE_X_SENSOR_L_THRESH    (0x258)

#endif /* SHARED_MEM_DEF_H_ */
