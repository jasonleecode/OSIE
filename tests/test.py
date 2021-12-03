"""
  OPC-UA test agent used for testing of PLC program here: https://lab.nexedi.com/nexedi/osie/tree/master/Beremiz/beremiz_test_opc_ua
  XXX:
  - add asserts
"""
from opcua import Client
import time

client = Client("opc.tcp://k2-osie:4840/")

try:
  client.connect()

  # Client has a few methods to get proxy to UA nodes that should always be in address space such as Root or Objects
  root = client.get_root_node()
  children_list = root.get_children()
  for i in range (0,10):
    var = client.get_node("ns=1;s=i2c0.relay0")
    i2c0_relay0 = var.get_value()
    # XXX: do asserts here
    print  i2c0_relay0
    print "sleep"
    time.sleep(5)
  
finally:
  client.disconnect()
