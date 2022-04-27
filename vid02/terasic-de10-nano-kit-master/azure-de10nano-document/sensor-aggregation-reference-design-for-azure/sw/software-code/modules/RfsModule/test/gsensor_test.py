# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import sys
sys.path.append('../')

import unittest
from package.gsensor import *
from package.basecomnios import *

class TestGsensor(unittest.TestCase):
    def setUp(self):
        bridges = get_nios_status(hostname)
        self.gsensor_dummy = Gsensor(real=False,offset=0x0)
        self.gsensor = None
        self.bridge = None
        if (bridges[0] is not None) :
            self.bridge = bridges[1]
            self.gsensor = Gsensor(real=True,offset=0x0)

    def test_0_get_telemetries(self):
        res = self.gsensor_dummy.get_telemetries()
        self.assertFalse(res)
        if (self.gsensor is not None) :
            values = self.gsensor.get_telemetries(self.bridge)
            #Cannot be validated
            print(values)
            self.assertIsNot(values,False)

if __name__ == "__main__":
    unittest.main()