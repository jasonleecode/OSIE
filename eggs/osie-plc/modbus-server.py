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
import bitarray
from bitarray.util import ba2int

# name fo device within Lime2
DEFAULT_MOD_IO_DEVICE_NAME = "/dev/i2c-1"

class Lime2MODIOI2c:
    """
    Class to communication from Lime2 -> MOD-IO (over I2c)

    Keep relay state internally as take into account to preserver relay state!
    """

    # state of 4 relays of mod-io, by default OFF
    # added 8 as their format is 8 bit and maybe they extend mod-io
    # only last 4 bits are used
    relay_state_list = [0, 0, 0, 0, 0, 0, 0, 0]

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
            Write / send to device.
        """
        self.open()
        while 1:
            try:
                i2c.write([0x10, code])
                break
            except:
                print("Failed to sent command: %s" %code)
        # be a good citizen and close
        self.close()

    def setRelayState(self, relay_number, relay_state):
        """
            Set relay state.
            relay_number: 0 - 3
            relay_state: 0 (off), 1 (on)
        """
        if relay_state not in (0, 1):
            raise ValueError("Incorect relay state!")
        if relay_number not in (0, 1, 2, 3):
            raise ValueError("Incorrect relay number!")

        # relay1 is the most right element in list / byte command representation
        self.relay_state_list[-(1 + relay_number)] = relay_state

        # generate proper relay state command of all 4 relays
        # which doesn't override any other relays' state
        # which itself is kept in this app's RAM
        ba = bitarray.bitarray(self.relay_state_list)
        self.write(ba2int(ba)) 

    def setRelayStateAllOff(self):
        """
            Switch off all relays.
        """
        self.relay_state_list = [0, 0, 0, 0, 0, 0, 0, 0]
        ba = bitarray.bitarray(self.relay_state_list)
        self.write(ba2int(ba)) 


    def setRelayStateAllOn(self):
        """
            Switch on all relays.
        """
        self.relay_state_list = [0, 0, 0, 0, 1, 1, 1, 1]
        ba = bitarray.bitarray(self.relay_state_list)
        self.write(ba2int(ba)) 


# --------------------------------------------------------------------------- # 
# configure the service logging
# --------------------------------------------------------------------------- # 
import logging
FORMAT = ('%(asctime)-15s %(threadName)-15s'
          ' %(levelname)-8s %(module)-15s:%(lineno)-8s %(message)s')
logging.basicConfig(format=FORMAT)
log = logging.getLogger()
log.setLevel(logging.DEBUG)


# main class to communicate with  MOD-IO
mod_io = Lime2MODIOI2c()

class LimeModbusSlaveContext(ModbusSlaveContext):
    """
    Control Lime2 -> Mod-io ->relay {1..4}
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

        # switch relays at MOD-IO
        mod_io.setRelayState(address, value)

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

def main():
    """
    Main OSIE-PLC entry point.
    """
    # switch OFF all relays
    mod_io.setRelayStateAllOff()

    # run modbus "server"
    run_async_server()

    # switch off all
    mod_io.setRelayStateAllOff()

if __name__ == "__main__":
    main()

