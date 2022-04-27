# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information

from package.sensor import *
from package.sensorcontroller import *

class RfsSensor(SensorController):
    def __init__(self,name='rfsSensors', real=False, offset=0x0) -> None:
        self.name = name
        self.modules = {
            'lux'           : Sensor('lux',             'float', real,        offset+0x4*2),
            'humidity'      : Sensor('humidity',        'float', real,        offset+0x4*3),
            'temperature'   : Sensor('temperature',     'float', real,        offset+0x4*4),
            'mpu9250' : SensorController('mpu9250',real, offset+0x4*4)
        }
        self.modules['mpu9250'].modules = {
            'ax'            : Sensor('ax',          'float', real,        offset+0x4*5),
            'ay'            : Sensor('ay',          'float', real,        offset+0x4*6),
            'az'            : Sensor('az',          'float', real,        offset+0x4*7), 
            'gx'            : Sensor('gx',          'float', real,        offset+0x4*8), 
            'gy'            : Sensor('gy',          'float', real,        offset+0x4*9), 
            'gz'            : Sensor('gz',          'float', real,        offset+0x4*10), 
            'mx'            : Sensor('mx',          'float', real,        offset+0x4*11), 
            'my'            : Sensor('my',          'float', real,        offset+0x4*12), 
            'mz'            : Sensor('mz',          'float', real,        offset+0x4*13), 
        }
        
        if(real) :
            logger.debug(" \n\
                    RfsSensors: \n\
                        Lux : \n\
                            Data : {} \n\
                        Humidity : \n\
                            Data : {} \n\
                        Temperature : \n\
                            Data : {} \n\
                        AX : \n\
                            Data : {} \n\
                        AY : \n\
                            Data : {} \n\
                        AZ : \n\
                            Data : {} \n\
                        GX : \n\
                            Data : {} \n\
                        GY : \n\
                            Data : {} \n\
                        GZ : \n\
                            Data : {} \n\
                        MX : \n\
                            Data : {} \n\
                        MY : \n\
                            Data : {} \n\
                        MZ : \n\
                            Data : {} \n"
                .format(
                    hex(self.modules.get('lux').offset),
                    hex(self.modules.get('humidity').offset),
                    hex(self.modules.get('temperature').offset),
                    hex(self.modules.get('mpu9250').modules.get('ax').offset),
                    hex(self.modules.get('mpu9250').modules.get('ay').offset),
                    hex(self.modules.get('mpu9250').modules.get('az').offset),
                    hex(self.modules.get('mpu9250').modules.get('gx').offset),
                    hex(self.modules.get('mpu9250').modules.get('gy').offset),
                    hex(self.modules.get('mpu9250').modules.get('gz').offset),
                    hex(self.modules.get('mpu9250').modules.get('mx').offset),
                    hex(self.modules.get('mpu9250').modules.get('my').offset),
                    hex(self.modules.get('mpu9250').modules.get('mz').offset)
                )
            )