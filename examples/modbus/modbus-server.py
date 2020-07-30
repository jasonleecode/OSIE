#!/usr/bin/env python
"""
Modbus implementation for Lime2 -> MOD-IO which controls relays.

root@a20:~/osie/examples/modbus# pymodbus.console tcp --host localhost --port 502

To switch on relay 1:

    > client.write_registers address=0 values=1 unit=1

To switch OFF:

    > client.write_registers address=0 values=0 unit=1

"""
from pymodbus.server.asynchronous import StartTcpServer
from pymodbus.server.asynchronous import StartUdpServer
from pymodbus.server.asynchronous import StartSerialServer

from pymodbus.device import ModbusDeviceIdentification
from pymodbus.datastore import ModbusSequentialDataBlock
from pymodbus.datastore import ModbusSlaveContext, ModbusServerContext
from pymodbus.transaction import (ModbusRtuFramer,
                                  ModbusAsciiFramer,
                                  ModbusBinaryFramer)
from custom_message import CustomModbusRequest
from pyA20Lime2 import i2c


# name fo device within Lime2
DEFAULT_MOD_IO_DEVICE_NAME = "/dev/i2c-1"
class Lime2MODIOI2c:
    """
    Class to communication from Lime2 -> MOD-IO (over I2c)

    XXX: keep relay state internally as take into account to preserver relay state!

    """

    def __init__(self, mod_io_device_name = DEFAULT_MOD_IO_DEVICE_NAME):
        self.mod_io_device_name = mod_io_device_name

    def open(self):
        """
            Open connection to MOD-IO.
        """
        i2c.init(self.mod_io_device_name)
        i2c.open(0x58)

    def close(self):
        """
           Close connection to MOD-IO.
        """
        i2c.close()

 
    def write(self, code):
        """
            Write / send frommnad to device.
        """
        self.open()
        while 1:
            try:
                i2c.write([0x10, code])
                break
            except:
                print("Failed co sent command.")
        # be a good citizen and close
        self.close()


# --------------------------------------------------------------------------- # 
# configure the service logging
# --------------------------------------------------------------------------- # 
import logging
FORMAT = ('%(asctime)-15s %(threadName)-15s'
          ' %(levelname)-8s %(module)-15s:%(lineno)-8s %(message)s')
logging.basicConfig(format=FORMAT)
log = logging.getLogger()
log.setLevel(logging.DEBUG)


mod_io = Lime2MODIOI2c()
mod_io.write(0x00) # all relays off


class LimeModbusSlaveContext(ModbusSlaveContext):
    """
    XXX: Control Lime2 -> Mod-io ->relay {1..4}
    """

    def setValues(self, fx, address, values):
        if not self.zero_mode:
            address = address + 1
        log.debug("setValues[%d] %d:%s" % (fx, address, values))
        self.store[self.decode(fx)].setValues(address, values)

        # control relays
        value = int(values[0])
        log.debug(address)
        log.debug(value)

        # relay 1
        if int(address) == 0:
            if value == 1:
                mod_io.write(0x01)
            elif value == 0:
                mod_io.write(0x00)

        # relay 2
        if int(address) == 1:
            # relay1
            if value == 1:
                # on
                mod_io.write(0x03)
            elif value == 0:
                mod_io.write(0x00)


def run_async_server():
    store = LimeModbusSlaveContext(
        di=ModbusSequentialDataBlock(0, [0]*10),
        co=ModbusSequentialDataBlock(0, [0]*10),
        hr=ModbusSequentialDataBlock(0, [0]*10),
        ir=ModbusSequentialDataBlock(0, [0]*10), zero_mode = True)
    store.register(CustomModbusRequest.function_code, 'cm',
                   ModbusSequentialDataBlock(0, [17] * 100))
    context = ModbusServerContext(slaves=store, single=True)
    
    # ----------------------------------------------------------------------- # 
    # initialize the server information
    # ----------------------------------------------------------------------- # 
    # If you don't set this or any fields, they are defaulted to empty strings.
    # ----------------------------------------------------------------------- # 
    identity = ModbusDeviceIdentification()
    identity.VendorName = 'Nexedi'
    identity.ProductCode = 'Lime2.PLC'
    identity.VendorUrl = 'http://github.com/bashwork/pymodbus/'
    identity.ProductName = 'Pymodbus Server'
    identity.ModelName = 'Pymodbus Server'
    identity.MajorMinorRevision = '0.0.1'
    
    # TCP Server
    StartTcpServer(context, identity=identity, address=("0.0.0.0", 502),
                   custom_functions=[CustomModbusRequest])

if __name__ == "__main__":
    run_async_server()

    # switch off all
    mod_io.write(0x00)

