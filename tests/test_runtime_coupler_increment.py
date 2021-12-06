"""
  OPC-UA test agent used for testing of PLC program here: https://lab.nexedi.com/nexedi/osie/tree/master/Beremiz/beremiz_test_opc_ua

"""
from opcua import Client
import time

TIMEOUT = 5 # seconds to wait between checks
NUMBER_OF_CHECKS = 10 # number of times we check the server
OPC_UA_ADDRESS = "opc.tcp://k2-osie:4840/"
OPC_UA_IDENTIFIER = "ns=1;s=i2c0.relay0"

# connect to a session at OPC-UA server
client = Client(OPC_UA_ADDRESS)

try:
  client.connect()

  # Client has a few methods to get proxy to UA nodes that should always be in address space such as Root or Objects
  root = client.get_root_node()
  children_list = root.get_children()
  var = client.get_node(OPC_UA_IDENTIFIER)

  for i in range (0, NUMBER_OF_CHECKS):
    i2c0_relay0_before = var.get_value()
    print("\ni2c0_relay0 (before) = ", i2c0_relay0_before)
    print("sleep ...")
    time.sleep(TIMEOUT)
    i2c0_relay0_after = var.get_value()
    print("i2c0_relay0 (after) = ", i2c0_relay0_after)
    # for the wait timeout runtime should have increased the value
    assert i2c0_relay0_after > i2c0_relay0_before

finally:
  client.disconnect()
