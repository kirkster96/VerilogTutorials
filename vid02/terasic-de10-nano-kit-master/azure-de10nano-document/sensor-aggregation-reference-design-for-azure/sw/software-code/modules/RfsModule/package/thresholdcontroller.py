# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

from package.threshold import *

class ThresholdController:

    def __init__(self,real=False, bridge=None,offset=0x0) -> None:
        self.modules = {
            # 'X', 'Y', 'Z' is only for dummy modules to generate dummy values and 'real' is always False
            'X'             : Threshold('X',            'float', False,       bridge, 0x0,            0x0,                    0,      10),
            'Y'             : Threshold('Y',            'float', False,       bridge, 0x0,            0x0,                    0,      10),
            'Z'             : Threshold('Z',            'float', False,       bridge, 0x0,            0x0,                    0,      10),
            'lux'           : Threshold('lux',          'float', real,        bridge, offset+0x8*2,   offset+0x4+0x8*2,       0,      1000),
            'humidity'      : Threshold('humidity',     'float', real,        bridge, offset+0x8*3,   offset+0x4+0x8*3,       0,      49.9), 
            'temperature'   : Threshold('temperature',  'float', real,        bridge, offset+0x8*4,   offset+0x4+0x8*4,       0,      42.3),
            'ax'            : Threshold('ax',           'float', real,        bridge, offset+0x8*5,   offset+0x4+0x8*5,       -100,   100), 
            'ay'            : Threshold('ay',           'float', real,        bridge, offset+0x8*6,   offset+0x4+0x8*6,       -100,   100), 
            'az'            : Threshold('az',           'float', real,        bridge, offset+0x8*7,   offset+0x4+0x8*7,       -100,   100), 
            'gx'            : Threshold('gx',           'float', real,        bridge, offset+0x8*8,   offset+0x4+0x8*8,       -10,    10), 
            'gy'            : Threshold('gy',           'float', real,        bridge, offset+0x8*9,   offset+0x4+0x8*9,       -10,    10), 
            'gz'            : Threshold('gz',           'float', real,        bridge, offset+0x8*10,   offset+0x4+0x8*10,     -10,    10), 
            'mx'            : Threshold('mx',           'float', real,        bridge, offset+0x8*11,  offset+0x4+0x8*11,      -100,   100), 
            'my'            : Threshold('my',           'float', real,        bridge, offset+0x8*12,  offset+0x4+0x8*12,      -100,   100), 
            'mz'            : Threshold('mz',           'float', real,        bridge, offset+0x8*13,  offset+0x4+0x8*13,      -100,   100), 
        }
        if(real) :
            logger.debug(" \n\
                    ThresholdController: \n\
                        X(Dummy) : \n\
                            Min  : {} Max  : {} \n\
                        Y(Dummy) : \n\
                            Min  : {} Max  : {} \n\
                        Z(Dummy) : \n\
                            Min  : {} Max  : {} \n\
                        lux : \n\
                            Min  : {} Max  : {} \n\
                        humidity : \n\
                            Min  : {} Max  : {} \n\
                        temperature : \n\
                            Min  : {} Max  : {} \n\
                        ax : \n\
                            Min  : {} Max  : {} \n\
                        ay : \n\
                            Min  : {} Max  : {} \n\
                        az : \n\
                            Min  : {} Max  : {} \n\
                        gx : \n\
                            Min  : {} Max  : {} \n\
                        gy : \n\
                            Min  : {} Max  : {} \n\
                        gz : \n\
                            Min  : {} Max  : {} \n\
                        mx : \n\
                            Min  : {} Max  : {} \n\
                        my : \n\
                            Min  : {} Max  : {} \n\
                        mz : \n\
                            Min  : {} Max  : {} \n"
                .format(
                    hex(self.modules.get('X').offset.get('min')),               hex(self.modules.get('X').offset.get('max')),
                    hex(self.modules.get('Y').offset.get('min')),               hex(self.modules.get('Y').offset.get('max')),
                    hex(self.modules.get('Z').offset.get('min')),               hex(self.modules.get('Z').offset.get('max')),
                    hex(self.modules.get('lux').offset.get('min')),             hex(self.modules.get('lux').offset.get('max')),
                    hex(self.modules.get('humidity').offset.get('min')),        hex(self.modules.get('humidity').offset.get('max')),
                    hex(self.modules.get('temperature').offset.get('min')),     hex(self.modules.get('temperature').offset.get('max')),
                    hex(self.modules.get('ax').offset.get('min')),              hex(self.modules.get('ax').offset.get('max')),
                    hex(self.modules.get('ay').offset.get('min')),              hex(self.modules.get('ay').offset.get('max')),
                    hex(self.modules.get('az').offset.get('min')),              hex(self.modules.get('az').offset.get('max')),
                    hex(self.modules.get('gx').offset.get('min')),              hex(self.modules.get('gx').offset.get('max')),
                    hex(self.modules.get('gy').offset.get('min')),              hex(self.modules.get('gy').offset.get('max')),
                    hex(self.modules.get('gz').offset.get('min')),              hex(self.modules.get('gz').offset.get('max')),
                    hex(self.modules.get('mx').offset.get('min')),              hex(self.modules.get('mx').offset.get('max')),
                    hex(self.modules.get('my').offset.get('min')),              hex(self.modules.get('my').offset.get('max')),
                    hex(self.modules.get('mz').offset.get('min')),              hex(self.modules.get('mz').offset.get('max')),
                )
            )

    '''
        Method get_module_value(self, bridge, module_key, min_max_key=None)
        Arguments:
            bridge              : data bridge can be accessed
            module_key          : module name keyword to search in modules
            min_max_key         : min/max keyword to search in a module
        Return Value:
            False : Fails due to some reasons
            or 
            <Value>(float/int): Read Value(real is True) or Set Value(real is False) in the specific module
    '''
    def get_module_value(self, bridge, module_key, min_max_key):
        module = self.modules.get(module_key)
        if (module is not None) :
            return module.get_value(bridge, min_max_key)
        else : 
            return False

    '''
        Method generate_module_dummy_value(self, bridge, module_key)
        Arguments:
            module_key          : module name keyword to search in modules
        Return Value:
            False : Fails due to some reasons
            or 
            <Value>(float/int): Random value between minimum value and maximum
    '''
    def generate_module_dummy_value(self, module_key):
        module = self.modules.get(module_key)
        if (module is not None) :
            return module.generate_dummy_value()
        return False

    '''
        Method update_component_property(self, bridge, patch)
        Arguments:
            bridge              : data bridge can be accessed
            patch : 
                Component Type  : { 'thresholdProperty': {'__t': 'c', 'lux': {'Min': 10, 'Max': 11}}, '$version': 4}
        Return Value:
            dict : reported patch's contents
    '''
    def update_component_property(self, bridge, patch:dict) -> dict:
        ignore_keys = ["__t", "$version"]
        component_prefix = list(patch.keys())[0] # Expected Value is component name(Default:thresholdProperty)
        values = patch[component_prefix]
        print("Values received are :-")
        print(values)

        version = patch["$version"]

        res_dict = {}
        for prop_name, prop_value in values.items():
            if prop_name in ignore_keys:
                continue
            else:
                module =  self.modules.get(prop_name)
                if not (module is None) :
                    temp_dict = module.update_threshold_from_azure(bridge, version, prop_value)
                    res_dict.update(temp_dict)

        properties_dict = dict()
        if component_prefix:
            properties_dict[component_prefix] = res_dict
            properties_dict[component_prefix]['__t'] = 'c'
        else:
            properties_dict = res_dict
        return properties_dict
