#!/bin/bash
rm -f server

# build OPC UA server which publishes msgs
gcc  -o server -I /usr/local/include/ -std=c99 server.c -l:libopen62541.so -L/usr/local/lib -lmbedcrypto  -lmbedx509 -I ~/open62541/src/pubsub/  -I ~/open62541/deps/

