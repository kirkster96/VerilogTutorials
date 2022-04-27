# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import sys
sys.path.append('../')

import unittest
from package.sensor import *
from package.basecomnios import *

class TestSensor(unittest.TestCase):
    def setUp(self):
        bridges = get_nios_status(hostname)
        self.sensor_dummy = Sensor('Dummy','float',False,0x0)
        self.sensor = None
        self.bridge = None
        if (bridges[0] is not None) :
            self.bridge = bridges[1]
            self.sensor = Sensor('Light0','int',True,0x40100)
            #self.sensor = Sensor('Humidity','float',True,0x40108)

    def test_0_get_telemetry(self):
        res = self.sensor_dummy.get_telemetry()
        self.assertFalse(res)
        if (self.sensor is not None) :
            value = self.sensor.get_telemetry(self.bridge)
            #Cannot be validated
            self.assertIsNot(value,False)

if __name__ == "__main__":
    unittest.main()