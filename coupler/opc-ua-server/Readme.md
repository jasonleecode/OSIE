# Usage of OPC UA server with Beremiz

## Prerequisites

### Installing Beremiz

Run beremiz_install.sh file under Init folder

### Installing open62541

Run open62541_install.sh file under Init folder

### Copy the open62541.h and open62541.c

If you are developing OPC UA application for LIM2 controller with MOD-IO, then your OPC UA location will be ~/osie/coupler/opc-ua-server/

$ cp ~/open62541/build/open62541.* ~/osie/coupler/opc-ua-server/

### Building your OPC UA server (Cross compilation to ARM architecture from Ubuntu)

$ arm-linux-gnueabihf-gcc open62541.c server.c -o server

### If one wants to run coupler on a x86 platform then one needs to expose in current environment

$ export I2C_VIRTUAL_MODE=1
