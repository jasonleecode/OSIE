#!/bin/bash
# build OPC UA server which publishes msgs
gcc -I /usr/local/include/ -std=c99 server.c -o server_publish -l:libopen62541.so -L/usr/local/lib -lmbedcrypto  -lmbedx509

# build OPC Ua server which subscribes to msgs
gcc -o server_subscribe -I /usr/local/include/ -std=c99 server_subscribe.c -l:libopen62541.so -L/usr/local/lib -I ~/open62541/src/pubsub/  -I ~/open62541/deps/

