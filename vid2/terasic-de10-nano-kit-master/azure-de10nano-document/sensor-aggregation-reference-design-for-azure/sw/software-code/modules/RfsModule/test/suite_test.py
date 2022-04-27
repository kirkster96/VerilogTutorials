# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import unittest
import gsensor_test as g_test
import rfssensor_test as rfs_test
import sensor_test as s_test
import threshold_test as th_test
import thresholdcontroller_test as thc_test

g_suite   = unittest.TestLoader().loadTestsFromTestCase(g_test.TestGsensor)
rfs_suite = unittest.TestLoader().loadTestsFromTestCase(rfs_test.TestRfsSensor)
s_suite   = unittest.TestLoader().loadTestsFromTestCase(s_test.TestSensor)
th_suite  = unittest.TestLoader().loadTestsFromTestCase(th_test.TestThreshold)
thc_suite = unittest.TestLoader().loadTestsFromTestCase(thc_test.TestThresholdController)

suite = unittest.TestSuite([g_suite,rfs_suite,s_suite,th_suite,thc_suite])

runner = unittest.TextTestRunner()
runner.run(suite)