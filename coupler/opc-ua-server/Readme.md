# Usage of OPC UA server with Beremiz

## Prerequisites

### Installing open62541
$ git clone https://github.com/open62541/open62541.git
$ cd open62541/
$ git submodule update --init --recursive
$ mkdir build && cd build
$ cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DUA_NAMESPACE_ZERO=FULL -DUA_ENABLE_AMALGAMATION=ON ..
$ make
$ sudo make install

### Copy the open62541.h and open62541.c

If you are developing OPC UA application for LIM2 controller with MOD-IO, then your OPC UA location will be ~/osie/coupler/opc-ua-server/

$ cp ~/open62541/build/open62541.* ~/osie/coupler/opc-ua-server/

### Building your OPC UA server (Cross compilation to ARM architecture from Ubuntu)

$ arm-linux-gnueabihf-gcc open62541.c server.c -o server