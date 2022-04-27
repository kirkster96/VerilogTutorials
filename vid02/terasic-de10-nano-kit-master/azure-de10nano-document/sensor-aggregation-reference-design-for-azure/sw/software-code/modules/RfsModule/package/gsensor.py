# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

from package.sensorcontroller import *
from package.utility import *

GSENSOR_I2C_BUS = 0x53

class Gsensor(SensorController):
    def __init__(self,name="gSensor", real=False, offset=0x0) -> None:
        self.name = name
        #They are dummies
        self.modules = {
            'X'        : Sensor('X', 4, real,        0x0),
            'Y'        : Sensor('Y', 4, real,        0x0),
            'Z'        : Sensor('Z', 4, real,        0x0),
        }
        self.real = real
        if self.real :
            from package.adxl345 import ADXL345
            self.adxl345 = ADXL345(GSENSOR_I2C_BUS)

    '''
        Method get_telemetries(self, bridge)
        Arguments:
            bridge              : data bridge can be accessed
        Return Value:
            dict : Response Telemetry Message dict 
            or
            False : if module is dummy
    '''
    def get_telemetries(self,bridge=None):
        data = {}
        if self.real : 
            data[self.name] = self.adxl345.get_axes()
            return data
        else : 
            logger.debug('{} sensor is dummy and if you want values to test, please use generate_dummy_value in threshold class.'.format(self.name))
            return False