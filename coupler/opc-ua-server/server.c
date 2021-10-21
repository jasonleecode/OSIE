/* 
 * Example controling MOD-IO's relays' state over OPC-UA protocol
 * Based on 
 *   https://www.kernel.org/doc/Documentation/i2c/dev-interface
 *   https://github.com/open62541/open62541/blob/master/examples/tutorial_server_variable.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

// global relay state
uint8_t myrelay = 0;

static int setRelayState(int command) {
    /*
     *  Set relays' state over I2C
     */
    int file;
    int adapter_nr = 1; /* probably dynamically determined */
    char filename[20];

    // step 1: open device
    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
    file = open(filename, O_RDWR);
    if (file < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error openind i2c device.");
        exit(1);
    }

    // step 2: address the slave by its address 
    int addr = 0x58; /* The I2C address */
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error addressing i2c slave.");
        exit(1);
    }

    // step 3: write command over I2c
    __u8 reg = 0x10; /* Device register to access */
    __s32 res;
    char buf[10];
    buf[0] = reg;
    buf[1] = command; //0x00 -all off, 0x0F - all 4 on
    buf[2] = 0x65; // seems irrelevant the value
    if (write(file, buf, 3) != 3) {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Error writing to i2c slave."); 
    }
    close(file);
}

static void addVariable(UA_Server *server) {
    /* 
     * Create all variables representing MOD-IO's relays
     */
    UA_Int32 myInteger = 0;
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    
    // relay 0
    UA_VariableAttributes attr0 = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr0.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr0.description = UA_LOCALIZEDTEXT("en-US","Relay 0");
    attr0.displayName = UA_LOCALIZEDTEXT("en-US", "Relay 0");
    attr0.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr0.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId myIntegerNodeId0 = UA_NODEID_STRING(1, "relay0");
    UA_QualifiedName myIntegerName0 = UA_QUALIFIEDNAME(1, "Relay 0");
    UA_Server_addVariableNode(server, myIntegerNodeId0, parentNodeId,
                              parentReferenceNodeId, myIntegerName0,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr0, NULL, NULL);
    // relay 1
    UA_VariableAttributes attr1 = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr1.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr1.description = UA_LOCALIZEDTEXT("en-US","Relay 1");
    attr1.displayName = UA_LOCALIZEDTEXT("en-US", "Relay 1");
    attr1.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr1.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId myIntegerNodeId1 = UA_NODEID_STRING(1, "relay1");
    UA_QualifiedName myIntegerName1 = UA_QUALIFIEDNAME(1, "Relay 1");
    UA_Server_addVariableNode(server, myIntegerNodeId1, parentNodeId,
                              parentReferenceNodeId, myIntegerName1,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr1, NULL, NULL);

    // relay 2
    UA_VariableAttributes attr2 = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr2.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr2.description = UA_LOCALIZEDTEXT("en-US","Relay 2");
    attr2.displayName = UA_LOCALIZEDTEXT("en-US", "Relay 2");
    attr2.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr2.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId myIntegerNodeId2 = UA_NODEID_STRING(1, "relay2");
    UA_QualifiedName myIntegerName2 = UA_QUALIFIEDNAME(1, "Relay 2");
    UA_Server_addVariableNode(server, myIntegerNodeId2, parentNodeId,
                              parentReferenceNodeId, myIntegerName2,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr2, NULL, NULL);

    // relay 3
    UA_VariableAttributes attr3 = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr3.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr3.description = UA_LOCALIZEDTEXT("en-US","Relay 3");
    attr3.displayName = UA_LOCALIZEDTEXT("en-US", "Relay 3");
    attr3.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr3.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId myIntegerNodeId3 = UA_NODEID_STRING(1, "relay3");
    UA_QualifiedName myIntegerName3 = UA_QUALIFIEDNAME(1, "Relay 3");
    UA_Server_addVariableNode(server, myIntegerNodeId3, parentNodeId,
                              parentReferenceNodeId, myIntegerName3,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr3, NULL, NULL);

}

/* Connect to variables to physical relays
 *
 */
static void beforeReadTime(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeid, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    // nothing to do as not yet needed.
}

// XXX: having afterWriteTime{0..3} is not needed and maybe with introspection of context we can
// write only one callback function
static void afterWriteTime0(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,  "The variable was updated");
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            myrelay |= 1UL << 0; // relay0
	    setRelayState(myrelay);
	}
	else {
            myrelay &= ~(1UL << 0); // relay0
            setRelayState(myrelay);
	}
    }
}

static void afterWriteTime1(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,  "The variable was updated");
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            myrelay |= 1UL << 1;
	    setRelayState(myrelay);
	}
	else {
            myrelay &= ~(1UL << 1);
            setRelayState(myrelay);
	}
    }
}

static void afterWriteTime2(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,  "The variable was updated");
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            myrelay |= 1UL << 2;
	    setRelayState(myrelay);
	}
	else {
            myrelay &= ~(1UL << 2);
            setRelayState(myrelay);
	}
    }
}

static void afterWriteTime3(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,  "The variable was updated");
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            myrelay |= 1UL << 3;
	    setRelayState(myrelay);
	}
	else {
            myrelay &= ~(1UL << 3);
            setRelayState(myrelay);
	}
    }
}

static void addValueCallbackToCurrentTimeVariable(UA_Server *server) {
    // relay 0
    UA_NodeId currentNodeId0 = UA_NODEID_STRING(1, "relay0");
    UA_ValueCallback callback0 ;
    callback0.onRead = beforeReadTime;
    callback0.onWrite = afterWriteTime0;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId0, callback0);
    
    // relay 1
    UA_NodeId currentNodeId1 = UA_NODEID_STRING(1, "relay1");
    UA_ValueCallback callback1 ;
    callback1.onRead = beforeReadTime;
    callback1.onWrite = afterWriteTime1;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId1, callback1);
    
    // relay 2
    UA_NodeId currentNodeId2 = UA_NODEID_STRING(1, "relay2");
    UA_ValueCallback callback2 ;
    callback2.onRead = beforeReadTime;
    callback2.onWrite = afterWriteTime2;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId2, callback2);
    
    // relay 3
    UA_NodeId currentNodeId3 = UA_NODEID_STRING(1, "relay3");
    UA_ValueCallback callback3 ;
    callback3.onRead = beforeReadTime;
    callback3.onWrite = afterWriteTime3;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId3, callback3);
}


/**
 * Now we change the value with the write service. This uses the same service
 * implementation that can also be reached over the network by an OPC UA client.
 */

static void writeVariable(UA_Server *server) {
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "relay0");

    /* Write a different integer value */
    UA_Int32 myInteger = 0;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, myIntegerNodeId, myVar);

    /* Set the status code of the value to an error code. The function
     * UA_Server_write provides access to the raw service. The above
     * UA_Server_writeValue is syntactic sugar for writing a specific node
     * attribute with the write service. */
    UA_WriteValue wv;
    UA_WriteValue_init(&wv);
    wv.nodeId = myIntegerNodeId;
    wv.attributeId = UA_ATTRIBUTEID_VALUE;
    wv.value.status = UA_STATUSCODE_BADNOTCONNECTED;
    wv.value.hasStatus = true;
    UA_Server_write(server, &wv);

    /* Reset the variable to a good statuscode with a value */
    wv.value.hasStatus = false;
    wv.value.value = myVar;
    wv.value.hasValue = true;
    UA_Server_write(server, &wv);
}


static volatile UA_Boolean running = true;

static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

int main(void) {
    // set all relays to OFF at startup
    setRelayState(0x00);

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    UA_ServerConfig* config = UA_Server_getConfig(server);
    config->verifyRequestTimestamp = UA_RULEHANDLING_ACCEPT;
//#ifdef UA_ENABLE_WEBSOCKET_SERVER
//    UA_ServerConfig_addNetworkLayerWS(UA_Server_getConfig(server), 7681, 0, 0, NULL, NULL);
//#endif

    addVariable(server);
    writeVariable(server);
    addValueCallbackToCurrentTimeVariable(server);

    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);

    // set all relays to OFF at startup
    setRelayState(0x00);

    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
