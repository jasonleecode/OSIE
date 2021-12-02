from opcua import Client

client = Client("opc.tcp://k2-osie:4840/")

try:
  client.connect()

  # Client has a few methods to get proxy to UA nodes that should always be in address space such as Root or Objects
  root = client.get_root_node()
  children_list = root.get_children()

  var = client.get_node("ns=1;s=i2c0.relay0")
  i2c0_relay0 = var.get_value()
  print  i2c0_relay0
finally:
  client.disconnect()
