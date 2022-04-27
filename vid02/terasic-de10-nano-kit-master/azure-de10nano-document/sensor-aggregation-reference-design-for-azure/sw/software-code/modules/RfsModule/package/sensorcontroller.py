# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

from package.sensor import *
from azure.iot.device import Message
import json

class SensorController:
    def __init__(self,name:str,real=False, offset=0x0) -> None:
        self.name = name
        self.real = real
        self.modules = {} #Sensor or SensorController Instances

    '''
        Method get_telemetries(self, bridge)
        Arguments:
            bridge              : data bridge can be accessed
        Return Value:
            dict : Response Telemetry Message dict 
    '''
    def get_telemetries(self,bridge) -> dict:
        data = {}
        keys = list(self.modules)
        for key in keys:
            module = self.modules.get(key)
            if (isinstance(module,SensorController)) :
                value = module.get_telemetries(bridge)
            elif (isinstance(module,Sensor)) :
                value = self.modules.get(key).get_telemetry(bridge)
            if (value is not False) : data[key] = value #If fail to get, value maybe False
        return data
    
    '''
        Method create_telemetry(self, bridge)
        Arguments:
            data_dict : Data Dict to send to Azure
        Return Value:
            msg : Telemetry Message to Azure
    '''
    def create_telemetry(self, data_dict):
        msg = Message(json.dumps(data_dict))
        msg.content_encoding = "utf-8"
        msg.content_type = "application/json"
        return msg

    '''
        Method create_component_telemetry(self, bridge)
        Arguments:
            data_dict : Data Dict to send to Azure
        Return Value:
            msg : Telemetry Message to Azure with the component name
    '''
    def create_component_telemetry(self, data_dict):
        msg = self.create_telemetry(data_dict)
        msg.custom_properties["$.sub"] = self.name
        return msg