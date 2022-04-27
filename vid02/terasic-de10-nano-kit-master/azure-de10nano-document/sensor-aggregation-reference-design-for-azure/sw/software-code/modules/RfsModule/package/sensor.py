# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

from package.utility import *
from package.basecomnios import *


class Sensor(BaseComNIOS) :

    def __init__(self, name:str, type:str='float', real=False, data_offset=None) -> None:
        self.name = name
        self.type = type
        self.real:bool = real #If it is a real device, please set it True
        self.offset = data_offset

    '''
        Method get_telemetry(self, bridge)
        Arguments:
            bridge              : data bridge can be accessed
        Return Value:
            False : Fails due to some reasons
            or 
            <Value>(float/int): Read Value
    '''
    def get_telemetry(self, bridge=None) :
        if self.real == True :
            offset = self.offset
            return self.read(bridge, offset)
        else :
            logger.debug('{} sensor is dummy and if you want values to test, please use generate_dummy_value in threshold class.'.format(self.name))
            return False
    