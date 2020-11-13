"""
Simple OPC UA server with one paremeter (temp).
"""
from opcua import Server
from random import randint
import time

server = Server()
url = "opc.tcp://0.0.0.0:4840"
server.set_endpoint(url)

name = "OPCUA_SIMULATION_SERVER"
addspace = server.register_namespace(name)
node = server.get_objects_node()

Param = node.add_object(addspace, "Parameters")
Temp = Param.add_variable(addspace, "Temperature", 0)
Temp.set_writable()

server.start()

print ("started at %s" %url)

while 1:
    Temp.set_value(randint(10,50))
    time.sleep(10)
