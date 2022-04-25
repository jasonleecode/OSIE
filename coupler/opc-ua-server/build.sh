#!/bin/bash
rm -f server_publish server_subscribe 

# build OPC UA server which publishes msgs
gcc  -o server_publish -I /usr/local/include/ -std=c99 server.c -l:libopen62541.so -L/usr/local/lib -lmbedcrypto  -lmbedx509 -I ~/open62541/src/pubsub/  -I ~/open62541/deps/


# build OPC Ua server which subscribes to msgs
gcc -o server_subscribe -I /usr/local/include/ -std=c99 server_subscribe.c -l:libopen62541.so -L/usr/local/lib -I ~/open62541/src/pubsub/  -I ~/open62541/deps/

