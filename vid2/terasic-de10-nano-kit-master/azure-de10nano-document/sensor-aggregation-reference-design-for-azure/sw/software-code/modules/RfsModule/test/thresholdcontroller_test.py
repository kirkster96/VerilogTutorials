# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import sys
sys.path.append('../')

import unittest
from package.thresholdcontroller import *
from package.basecomnios import *

class TestThresholdController(unittest.TestCase):
    def setUp(self):
        bridges = get_nios_status(hostname)
        self.bridge = None
        self.threshold_controller = None
        self.threshold_controller_dummy = ThresholdController(False,self.bridge,0x0)
        if (bridges[0] is not None) :
            self.bridge = bridges[1]
            self.threshold_controller = ThresholdController(True,self.bridge,0x40200)

    def test_0_get_module_value(self):
        def loop_module(module:ThresholdController) :
            for module_key in ('Test', 'X', 'Y', 'Z', 'lux', 'humidity', 'temperature', 'ax', 'ay', 'az', 'gx', 'gy', 'gz', 'mx', 'my', 'mz') :
                for min_max_key in ('Test', 'min', 'max') :
                    res = module.get_module_value(self.bridge,module_key,min_max_key)
                    if (module_key == 'Test') or (min_max_key == 'Test') :
                        self.assertFalse(res)
                    else :
                        print('{}-{} value is {}'.format(module_key,min_max_key,res))
                        self.assertIsNot(res,False)
        loop_module(self.threshold_controller_dummy)
        if(self.threshold_controller is not None) :
            loop_module(self.threshold_controller)

    def test_1_generate_module_dummy_value(self):
        for module_key in ('Test', 'X', 'Y', 'Z', 'lux', 'humidity', 'temperature', 'ax', 'ay', 'az', 'gx', 'gy', 'gz', 'mx', 'my', 'mz') :
            res = self.threshold_controller_dummy.generate_module_dummy_value(module_key)
            res2 = self.threshold_controller_dummy.generate_module_dummy_value(module_key)
            if (module_key == 'Test') :
                self.assertFalse(res)
                self.assertFalse(res2)
            else :
                print('{} dummy value is {}(1st), {}(2nd)'.format(module_key,res,res2))
                self.assertIsNot(res,False)
                self.assertIsNot(res2,False)
                self.assertNotEqual(res,res2)

    def test_2_update_component_property(self) :
        patch = { 
            'thresholdProperty': {
                '__t': 'c',
                'lux'     : { 'min': 0, 'max': 1000 },
                'humidity': { 'min': 0, 'max': 100},
                'temperature': { 'min': 0, 'max': 100},
                'ax': { 'min': -10, 'max': 10},
                'ay': { 'min': -10, 'max': 10},
                'az': { 'min': -10, 'max': 10},
                'gx': { 'min': -1, 'max': 1},
                'gy': { 'min': -1, 'max': 1},
                'gz': { 'min': -1, 'max': 1},
                'mx': { 'min': -10, 'max': 10},
                'my': { 'min': -10, 'max': 10},
                'mz': { 'min': -10, 'max': 10}
            },
            "$version": 2
        }
        expected = {
            'thresholdProperty': {
                'lux' : {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': 0, 'max': 1000}}, 
                'humidity': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': 0, 'max': 100}}, 
                'temperature': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': 0, 'max': 100}}, 
                'ax': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -10, 'max': 10}}, 
                'ay': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -10, 'max': 10}}, 
                'az': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -10, 'max': 10}}, 
                'gx': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -1, 'max': 1}}, 
                'gy': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -1, 'max': 1}}, 
                'gz': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -1, 'max': 1}}, 
                'mx': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -10, 'max': 10}}, 
                'my': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -10, 'max': 10}}, 
                'mz': {'ac': 200, 'ad': 'Successfully executed patch', 'av': 2, 'value': {'min': -10, 'max': 10}},
                '__t': 'c'
            }
        }
        res = self.threshold_controller_dummy.update_component_property(self.bridge,patch)
        self.assertEqual(res,expected)
        if(self.threshold_controller is not None) :
            res2 = self.threshold_controller.update_component_property(self.bridge,patch)
            self.assertEqual(res2,expected)
 
    
if __name__ == "__main__":
    unittest.main()