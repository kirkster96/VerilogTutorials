# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import sys
sys.path.append('../')

import unittest
from package.threshold import *
from package.basecomnios import *

class TestThreshold(unittest.TestCase):
    def setUp(self):
        bridges = get_nios_status(hostname)
        self.min = 4
        self.max = 10
        self.bridge = None
        self.threshold = None
        self.threshold_dummy = Threshold('Dummy','float',False,self.bridge,0x0,0x0,0,1000)
        if (bridges[0] is not None) :
            self.bridge = bridges[1]
            #Example threshold
            #self.threshold = Threshold('channel0','int',True,self.bridge,0x40200,0x40204,0,1000)
            self.threshold = Threshold('lux','float', True,self.bridge, 0x40210,0x40214,0,1000)

    def test_0_set_threshold(self):
        res = self.threshold_dummy.set_threshold(self.bridge,self.min,self.max)
        self.assertTrue(res)
        if (self.threshold is not None) :
            res = self.threshold.set_threshold(self.bridge,self.min,self.max)
            self.assertTrue(res)

    def test_1_get_value(self):
        res = self.threshold_dummy.get_value(self.bridge, 'Test')
        self.assertFalse(res)
        res = self.threshold_dummy.get_value(self.bridge, 'min')
        self.assertEqual(res,0)
        res = self.threshold_dummy.get_value(self.bridge, 'max')
        self.assertEqual(res,1000)
        
        if (self.threshold is not None) :
            res = self.threshold.set_threshold(self.bridge,self.min,self.max)
            self.assertTrue(res)
            res = self.threshold.get_value(self.bridge, 'Test')
            self.assertFalse(res)
            res = self.threshold.get_value(self.bridge, 'min')
            self.assertEqual(res,self.min)
            res = self.threshold.get_value(self.bridge, 'max')
            self.assertEqual(res,self.max)

    def test_2_generate_dummy_value(self) :
        value  = self.threshold_dummy.generate_dummy_value()
        value2 = self.threshold_dummy.generate_dummy_value()
        print('1st value is {}, and 2nd value is {}'.format(value,value2))
        res = (0<=value<=1000) and (0<=value2<=1000)
        self.assertTrue(res)
    
    def test_3_update_threshold_from_azure(self) :
        prop = {'max':self.max, 'min':self.min}
        msg  = self.threshold_dummy.update_threshold_from_azure(self.bridge,2,prop)
        expect_result ={"Dummy": {"ac": 200, "av": 2, "ad": "Successfully executed patch", "value": {"min": 4, "max": 10}}}
        self.assertEqual(msg,expect_result)
        if (self.threshold is not None) :
            msg = self.threshold.update_threshold_from_azure(self.bridge,2,prop)
            #expect_result ={"channel0": {"ac": 200, "av": 2, "ad": "Successfully executed patch", "value": {"min": 4, "max": 10}}}
            expect_result ={"lux": {"ac": 200, "av": 2, "ad": "Successfully executed patch", "value": {"min": 4, "max": 10}}}
            self.assertEqual(msg,expect_result)

if __name__ == "__main__":
    unittest.main()