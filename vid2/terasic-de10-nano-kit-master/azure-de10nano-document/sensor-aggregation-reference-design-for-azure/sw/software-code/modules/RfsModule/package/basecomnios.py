# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import asyncio
import struct
from package.utility import *
import logging
import os
import mmap
import socket

'''
    hostname is global variable to check what's device for this application
'''
hostname = os.getenv('IOTEDGE_GATEWAYHOSTNAME',socket.gethostname()) 

'''
    BaseComNIOS class is base communication class with the NIOS II processor in FPGA.
    In this design, the way to access data(sensor/threshold) is same(Of course, offset is different) and that's why this class is created.
    It will be extended to Sensor or Threshold class.
'''

class BaseComNIOS :

    def __init__(self, type:str='float', real=False) -> None:
        self.real:bool = real #If it is a real device, please set it True
        self.type = type #float or int

    '''
        Method read(self, bridge, offset)
        Arguments:
            bridge : data bridge can be accessed
            offset : Address offset to read value
        Return Value:
            False : Fails due to some reasons
            or 
            <Value>(float/int): Read Value
    '''
    def read(self, bridge, offset) :
        try : 
            if self.real == False :
                return False
            if offset == None : 
                raise Exception('Error: Offset value is None!')
            bridge.seek(offset)
            if self.type == 'int' :
                _data = bridge.read(4)
                return int.from_bytes(_data, byteorder='little')
            elif self.type == 'float' :
                _data = bridge.read(4)
                return struct.unpack("<f", _data)[0]
        except Exception as e: 
            logger.debug(e)
            return False

    '''
        Method write(self, bridge, offset, data=None)
        Arguments:
            bridge : data bridge can be accessed.
            offset : Address offset to write value
            data   : Requested value to write
        Return Value:
            False : Fails due to some reasons
            or 
            True  : Complete
    '''
    def write(self, bridge, offset, data=None) -> bool:
        try : 
            if self.real == False :
                return False
            if offset == None : 
                raise Exception('Error: Offset value is None!')
            bridge.seek(offset)
            if self.type == 'int' :
                _bytedata = data.to_bytes(4, byteorder='little')
                bridge.write(_bytedata)
                return True
            elif self.type == 'float' :
                _bytedata = struct.pack('<f', data)
                bridge.write(_bytedata)
                return True
            else :
                raise Exception('Error: {} is illegal and type should be int or float.'.format(type(data)))
        except Exception as e :
            logger.debug(e)
            return False


'''
    Below functions will be used in main.py or test codes.
    It is because they are initialization/tracking methods to check HW design working correctly.
'''

# Base Address
LW_BRIDGE_BASE_ADDR = 0xff200000
LW_BRIDGE_SIZE = 0x100000
REG_BYTE_SIZE = 4 # the number of byte to read/write (default=REG_BYTE_SIZE=4=size(int)=size(float))

'''
    Method get_nios_status(hostname)
    Arguments:
        hostname : hostname string.
    Return Value:
        mmap.mmap, mmap.mmap : control bridge for nios watchdog, and data bridge for data communications
        or 
        None, None : if it fails
'''
def get_nios_status(hostname:str) :
    control_bridge = None
    data_bridge    = None
    if (hostname in 'de10nano'):
        try :
            f = os.open('/dev/mem', os.O_RDWR|os.O_SYNC)
            control_bridge = mmap.mmap(f, LW_BRIDGE_SIZE, mmap.MAP_SHARED, mmap.PROT_WRITE, offset=LW_BRIDGE_BASE_ADDR)
            control_bridge.seek(0x0)
            _data = control_bridge.read(REG_BYTE_SIZE)
            _data = int.from_bytes(_data, byteorder='little')
            if (_data >> 31)  == 1 : 
                data_bridge = mmap.mmap(f, LW_BRIDGE_SIZE, mmap.MAP_SHARED, mmap.PROT_WRITE, offset=LW_BRIDGE_BASE_ADDR)
            else :
                logger.debug('Error::: NIOS maybe not working.')
                control_bridge = None
        except Exception as e:
            logger.debug(getattr(e, 'message', repr(e)))
            control_bridge = None
            data_bridge = None
    else :
            logger.debug('Warning::: The hostname({}) is not supported.'.format(hostname))
    
    return control_bridge, data_bridge


'''
    Method watchdog_nios(bridge, interval)
    Arguments:
        bridge : control bridge to access NIOS
        interval : interval time to the next loop
    Note:
        It is watchdog function to monitor whether NIOS in FPGA works
'''
async def watchdog_nios(bridge, interval) :
    loop = 0
    while True:
        temp = read_loop_value(bridge)
        if (loop == temp) or (temp is None): 
            logging.debug('Error::: NIOS maybe stuck!!!Loop Count{}.'.format(loop))
            break
        else :
            loop = temp
        await asyncio.sleep(interval)

'''
    Method read_loop_value(bridge)
    Arguments:
        bridge : control bridge to access NIOS
    Return Value:
        integer : the number of loop stored in NIOS
        or
        None : it fails
    Note:
        read_loop_value is helper for watchdog_nios method
'''
def read_loop_value(bridge) : 
    try :
        bridge.seek(0x0)
        _data = bridge.read(REG_BYTE_SIZE)
        _data = int.from_bytes(_data, byteorder='little')
        if (_data >> 31) : 
            logging.debug('Error::: NIOS maybe not working.')
        return (_data & 0x7fffffff)
    except Exception as e:
        logger.debug(getattr(e, 'message', repr(e)))
        return None


