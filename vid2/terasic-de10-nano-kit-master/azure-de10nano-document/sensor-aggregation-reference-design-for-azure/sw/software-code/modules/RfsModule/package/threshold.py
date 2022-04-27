# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

from package.utility import *
from package.basecomnios import *
import random

class Threshold(BaseComNIOS) :

    def __init__(self, name:str, type:str='float', real=False, bridge=None, min_offset=None, max_offset=None, min_init=None, max_init=None) -> None:
        self.name = name
        self.type = type
        self.real:bool = real #If it is a real device, please set it True
        self.value = {'max':max_init, 'min':min_init}
        self.offset = {'max':max_offset, 'min':min_offset}
        if(self.real) : 
            self.set_threshold(bridge,min_init,max_init)

    '''
        Method get_value(self, bridge, key=None)
        Arguments:
            bridge : data bridge can be accessed
            key    : keyword to search in a dictionary(max or min)
        Return Value:
            False : Fails due to some reasons
            or 
            <Value>(float/int): Read Value(real is True ) or Set Value(real is False)
    '''
    def get_value(self, bridge, key=None) :
        if key in ('max', 'min') :
            if self.real == True :
                offset = self.offset.get(key)
                return self.read(bridge, offset)
            else :
                logger.debug('This Return Value of {}\'s {} will be just stored value in the application.'.format(self.name, key))
                return self.value.get(key)
        else :
                logger.debug('Error::: key({}) is undefined'.format(key))
                return False

    '''
        Method generate_dummy_value(self)
        Return Value:
            <Value>(float/int): Random value between minimum value and maximum
    '''
    def generate_dummy_value(self) : 
        return random.uniform(self.value.get('min'), self.value.get('max'))

    '''
        Method set_threshold(self, bridge, min_data=None, max_data=None):
        Arguments:
            bridge   : data bridge can be accessed
            min_data : Requested Value to set as minimum threshold
            max_data : Requested Value to set as maximum threshold
        Return Value:
            res(True or False) : Success or Fail
    '''
    def set_threshold(self, bridge, min_data=None, max_data=None) -> bool:
            res = True
            if(min_data is not None) : res &= self._set_threshold(bridge, 'min', min_data)
            if(max_data is not None) : res &= self._set_threshold(bridge, 'max', max_data)
            logger.debug('New {}\'s Threshold is {} - {}'
                .format(self.name, self.get_value(bridge, 'min'),self.get_value(bridge, 'max'))
            )
            return res

    '''
        Method _set_threshold(self, bridge, key, data=None):
        Arguments:
            bridge : data bridge can be accessed
            key    : keyword for a dictionary(min/max)
            data   : Requested Value
        Return Value:
            True : Success
            or
            False : Fail
        Note:
            This method is a helper for set_threshold.
    '''
    def _set_threshold(self, bridge, key, data=None) -> bool:
        if key in ('max', 'min') :
            try :
                if self.type == 'float' : __data = float(data)
                elif self.type == 'int' : __data = int(data)
                self.value[key] = float(__data)
                if self.real == True :
                    offset = self.offset.get(key)
                    return self.write(bridge,offset,data)
                else :
                    return True
            except Exception as e:
                logger.debug(e)
                return False


    '''
        Method update_threshold_from_azure(self, bridge, version, new_prop):
        Arguments:
            bridge   : data bridge can be accessed
            version  : Requested Version
            new_prop : new data dict
                Expected : {"min": 5, "max": 11}
        Return Value:
            dict : response message contents to Azure
                Expected : {self.name: {"ac": 200, "av": $version, {"min": 5, "max": 11}, ad: "comment"}}         
        Note:
            Reference :https://docs.microsoft.com/en-us/azure/iot-edge/iot-edge-runtime?view=iotedge-2020-11
    '''
    def update_threshold_from_azure(self, bridge, version, new_prop:dict) -> dict:
        max_data = new_prop.get('max')
        min_data = new_prop.get('min')
        res = self.set_threshold(bridge,min_data,max_data)
        msg_dict  = {}
        if  res : 
            ac_msg = 200
            ad_msg = 'Successfully executed patch'
        else :
            ac_msg = 400
            ad_msg = 'Fail to patch'
        
        msg_dict[self.name] = {
            'ac' : ac_msg,
            'ad' : ad_msg,
            'av' : version,
            'value' : {'min': self.get_value(bridge, 'min'), 'max': self.get_value(bridge, 'max')}
        }
        return msg_dict
