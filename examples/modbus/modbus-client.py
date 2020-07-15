from pymodbus.client.sync import ModbusTcpClient

client = ModbusTcpClient('localhost')
client.write_coil(1, True)
client.write_coil(2, False)

result = client.read_coils(1,1)
print(result.bits[0])

result = client.read_coils(2,1)
print(result.bits[0])


client.close()