# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import sys
sys.path.append('../')

import unittest
from package.rfssensor import *
from package.basecomnios import *

class TestRfsSensor(unittest.TestCase):
    def setUp(self):
        bridges = get_nios_status(hostname)
        self.bridge = None
        self.rfssensor = None
        self.rfssensor_dummy = RfsSensor('Dummy',False,0x0)
        if (bridges[0] is not None) :
            self.bridge = bridges[1]
            self.rfssensor = RfsSensor('RfsSensor',True,0x40100)

    def test_0_get_telemetries(self):
        res = self.rfssensor_dummy.get_telemetries(self.bridge)
        self.assertEqual(len(res),1)
        if(self.rfssensor is not None) :
            res = self.rfssensor.get_telemetries(self.bridge)
            print(res)
            self.assertEqual(len(res),4)

    def test_1_create_telemetry(self):
        data = self.rfssensor_dummy.get_telemetries(self.bridge)
        res = self.rfssensor_dummy.create_telemetry(data)
        print(res, res.content_encoding, res.content_type)
        if(self.rfssensor is not None) :
            data = self.rfssensor.get_telemetries(self.bridge)
            res = self.rfssensor.create_telemetry(data)
            print(res, res.content_encoding, res.content_type)

    def test_2_create_component_telemetry(self):
        data = self.rfssensor_dummy.get_telemetries(self.bridge)
        res = self.rfssensor_dummy.create_component_telemetry(data)
        print(res, res.content_encoding, res.content_type,res.custom_properties["$.sub"])
        if(self.rfssensor is not None) :
            data = self.rfssensor.get_telemetries(self.bridge)
            res = self.rfssensor.create_component_telemetry(data)
            print(res, res.content_encoding, res.content_type,res.custom_properties["$.sub"])

if __name__ == "__main__":
    unittest.main()