# Usage of OPC UA server with Beremiz

## Prerequisites

### Installing Beremiz

Request a Beremiz IDE instance via SlapOS

    release="https://lab.nexedi.com/nexedi/slapos/raw/beremiz/software/beremiz-ide/software.cfg"
    supply(release, "COMP-XXXX")
    partition_parameter_kw = {}
    filter_kw = {"computer_guid": "COMP-XXXX"}
    request(
        software_release = release,
        partition_reference = 'OSIE.k2-osie.Beremiz.IDE-1',
        partition_parameter_kw = partition_parameter_kw,
        filter_kw = filter_kw)

### Compile and install open62541

    ivan@k2-osie:~/open62541/build$ cmake -DBUILD_SHARED_LIBS=ON \
                                          -DCMAKE_BUILD_TYPE=RelWithDebInfo \
                                          -DUA_ENABLE_PUBSUB=ON \
                                          -DUA_ENABLE_PUBSUB_MONITORING=ON \
                                          -DUA_ENABLE_PUBSUB_ETH_UADP=ON \
                                          -DUA_NAMESPACE_ZERO=REDUCED \
                                          -DUA_ENABLE_ENCRYPTION=MBEDTLS \
                                          -DUA_ENABLE_ENCRYPTION_MBEDTLS=ON \
                                          ..
    ivan@k2-osie:~/open62541/build$ make

    # install system wide in /usr/local/lib
    ivan@k2-osie:~/open62541/build$ sudo make install

    # update system wide libraries
    ivan@k2-osie:~/open62541/build$ sudo ldconfig


### Building your OPC UA server (Cross compilation to ARM architecture from Ubuntu)

    # compile coupler application with a shared library and UA_ENABLE_AMALGAMATION=OFF
    ivan@k2-osie:~/open62541/build$ gcc -I /usr/local/include/ -std=c99 ~/osie/coupler/opc-ua-server/server.c -o server -l:libopen62541.so -L/usr/local/lib -lmbedcrypto  -lmbedx509

### If one wants to run coupler on a x86 platform then one needs to run server in virtual environment

$ ./server -m 1
