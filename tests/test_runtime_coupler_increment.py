"""
  OPC-UA test agent used for testing of PLC program here: https://lab.nexedi.com/nexedi/osie/tree/master/Beremiz/beremiz_test_opc_ua

"""
from opcua import Client
import time
import argparse
import configparser

def main():
  # handle CLI arguments
  parser = argparse.ArgumentParser()
  parser.add_argument('--iterations', \
                      type = int, \
                      default = 10, \
                      help='number of iterations to check')
  parser.add_argument('--timeout', \
                      type = int, \
                      default = 5, \
                      help='seconds to wait between checks')
  parser.add_argument('--opc-ua-server', \
                      type = str, \
                      default = 'opc.tcp://0.0.0.0:4840/', \
                      help='Address of OPC-UA server')
  parser.add_argument('--opc-ua-node-identifier', \
                      type = str, \
                      default = 'ns=1;s=i2c0.relay0', \
                      help='Address of OPC-UA server')

  args = parser.parse_args()
  NUMBER_OF_CHECKS = args.iterations
  TIMEOUT  = args.timeout
  OPC_UA_ADDRESS = args.opc_ua_server
  OPC_UA_IDENTIFIER = args.opc_ua_node_identifier

  # connect to a session at OPC-UA server
  client = Client(OPC_UA_ADDRESS)

  # for now this is the only test thus we start it without a wrapper
  test_count = 1
  test_failures = 0
  expected_failures = 0
  try:
    client.connect()

    # Client has a few methods to get proxy to UA nodes that should always be in address space such as Root or Objects
    root = client.get_root_node()
    children_list = root.get_children()
    var = client.get_node(OPC_UA_IDENTIFIER)

    for i in range (0, NUMBER_OF_CHECKS):
      i2c0_relay0_before = var.get_value()
      print("\ni2c0_relay0 (before) = ", i2c0_relay0_before)
      print("Sleep for %s seconds ..." %TIMEOUT)
      time.sleep(TIMEOUT)
      i2c0_relay0_after = var.get_value()
      print("i2c0_relay0 (after) = ", i2c0_relay0_after)
      # for the wait timeout runtime should have increased the value
      if (i2c0_relay0_after <= i2c0_relay0_before):
        # counter should have been increased, mark failure
        test_falures += 1
  finally:
    client.disconnect()

  # print out to sdtout results
  print("Integration Tests %d Tests, %d Failures, %d Expected failures" \
          %(test_count, test_failures, expected_failures))

if __name__ == "__main__":
  main()
