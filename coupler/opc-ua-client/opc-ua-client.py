"""
Simple OPC UA client.
"""
from opcua import Client

url = "opc.tcp://localhost:4840"
client = Client(url)
client.connect()
print("Client connected.")

while 1:
    Temp = Temp.get_node("ns=2;i=3") # XXX: get from OPC UA server
    temperature = Temp.get_value()
    print(temperature)
