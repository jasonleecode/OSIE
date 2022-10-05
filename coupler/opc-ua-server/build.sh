#!/bin/bash
OPEN62541_SR=/usr/local/lib
rm -f server

# build OPC UA server which publishes msgs
gcc  -o server -I /usr/local/include/ -std=c99 server.c -l:libopen62541.so -L$OPEN62541_SR/lib -lmbedcrypto  -lmbedx509 -I ~/open62541/src/pubsub/  -I ~/open62541/deps/

