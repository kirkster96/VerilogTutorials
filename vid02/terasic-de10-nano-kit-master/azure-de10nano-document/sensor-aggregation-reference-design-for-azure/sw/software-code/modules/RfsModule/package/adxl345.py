# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

# This source code is customized to fit DE10-Nano.
# The axe data are almost same as them of C Sample Source Code bundled in Terasic RFS Design

from smbus2 import SMBus

bus = SMBus(0)

# ADXL345 constant parameteers
EARTH_GRAVITY_MS2   = 9.80665
#g_per_digi
SCALE_MULTIPLIER    = 0.004

DATA_FORMAT         = 0x31
BW_RATE             = 0x2C
POWER_CTL           = 0x2D

BW_RATE_1600HZ      = 0x0F
BW_RATE_800HZ       = 0x0E
BW_RATE_400HZ       = 0x0D
BW_RATE_200HZ       = 0x0C
BW_RATE_100HZ       = 0x0B
BW_RATE_50HZ        = 0x0A
BW_RATE_25HZ        = 0x09

RANGE_2G            = 0x00
RANGE_4G            = 0x01
RANGE_8G            = 0x02
RANGE_16G           = 0x03

MEASURE             = 0x08
AXES_DATA           = 0x32

class ADXL345:

    address = None

    def __init__(self, address = 0x53):        
        self.address = address
        self.set_bandwidthrate(BW_RATE_50HZ)
        self.set_range(RANGE_2G)
        self.enable_measurement()

    def enable_measurement(self):
        bus.write_byte_data(self.address, POWER_CTL, MEASURE)

    def set_bandwidthrate(self, rate_flag):
        bus.write_byte_data(self.address, BW_RATE, rate_flag)

    # set the measurement range for 10-bit readings
    def set_range(self, range_flag):
        value = (range_flag | 0x08)
        bus.write_byte_data(self.address, DATA_FORMAT, value)
    
    def get_axes(self):
        bytes = bus.read_i2c_block_data(self.address, AXES_DATA, 6)
        
        x = bytes[0] | (bytes[1] << 8)
        #cast unit to int
        if(x & (1 << 16 - 1)):
            x = x - (1<<16)

        y = bytes[2] | (bytes[3] << 8)
        #cast unit to int
        if(y & (1 << 16 - 1)):
            y = y - (1<<16)

        z = bytes[4] | (bytes[5] << 8)
        #cast unit to int
        if(z & (1 << 16 - 1)):
            z = z - (1<<16)

        x = x * SCALE_MULTIPLIER 
        y = y * SCALE_MULTIPLIER
        z = z * SCALE_MULTIPLIER

        x = round(x, 4)
        y = round(y, 4)
        z = round(z, 4)

        return {"x": x, "y": y, "z": z}
