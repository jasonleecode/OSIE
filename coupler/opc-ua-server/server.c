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
#include "open62541.h"

// global relay state
uint8_t I2C_0_RELAYS_STATE = 0; // state of 4 relays at I2C slave 0
uint8_t I2C_1_RELAYS_STATE = 0; // state of 4 relays at I2C slave 1

// the default addresses of MOD-IOs
const int I2C_0_ADDR = 0x58;
const int I2C_1_ADDR = 0x59;

// the list of attached I2C slaves
const int DEFAULT_I2C_SLAVE_ADDR = 0x58;
int I2C_SLAVE_ADDR_LIST[] = {0, 0, 0, 0, 0}; // XXX: make dynamic array

// the block device at host machine
static char *DEFAULT_I2C_BLOCK_DEVICE_NAME = "/dev/i2c-1";
char *I2C_BLOCK_DEVICE_NAME;

static int setRelayState(int command, int i2c_addr) {
    /*
     *  Set relays' state over I2C
     */
    int file;
    char filename[20];

    // step 1: open device
    file = open(I2C_BLOCK_DEVICE_NAME, O_RDWR);
    if (file < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error openind i2c device.\n");
        exit(1);
    }

    // step 2: address the slave by its address 
    if (ioctl(file, I2C_SLAVE, i2c_addr) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error addressing i2c slave.\n");
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
        printf("Error writing to i2c slave.\n"); 
    }
    close(file);
}

void addIntegerVariableNode(UA_Server *server, char *node_id, char *node_description) {
    UA_Int32 myInteger = 0;
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    
    UA_VariableAttributes attr0 = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr0.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr0.description = UA_LOCALIZEDTEXT("en-US", node_description);
    attr0.displayName = UA_LOCALIZEDTEXT("en-US", node_description);
    attr0.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr0.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_NodeId myIntegerNodeId0 = UA_NODEID_STRING(1, node_id);
    UA_QualifiedName myIntegerName0 = UA_QUALIFIEDNAME(1, node_description);
    UA_Server_addVariableNode(server, myIntegerNodeId0, parentNodeId,
                              parentReferenceNodeId, myIntegerName0,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr0, NULL, NULL);
 
}

static void addVariable(UA_Server *server) {
    /* 
     * Create all variables representing MOD-IO's relays
     */
    // IC2-0
    addIntegerVariableNode(server, "i2c0.relay0", "I2C0 / Relay 0");
    addIntegerVariableNode(server, "i2c0.relay1", "I2C0 / Relay 1");
    addIntegerVariableNode(server, "i2c0.relay2", "I2C0 / Relay 2");
    addIntegerVariableNode(server, "i2c0.relay3", "I2C0 / Relay 3");
    // IC2-1
    addIntegerVariableNode(server, "i2c1.relay0", "I2C1 / Relay 0");
    addIntegerVariableNode(server, "i2c1.relay1", "I2C1 / Relay 1");
    addIntegerVariableNode(server, "i2c1.relay2", "I2C1 / Relay 2");
    addIntegerVariableNode(server, "i2c1.relay3", "I2C1 / Relay 3");
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
static void afterWriteTimeI2C0_0(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            I2C_0_RELAYS_STATE |= 1UL << 0; 
            setRelayState(I2C_0_RELAYS_STATE, I2C_0_ADDR);
	}
	else {
            I2C_0_RELAYS_STATE &= ~(1UL << 0);
            setRelayState(I2C_0_RELAYS_STATE, I2C_0_ADDR);
	}
    }
}

static void afterWriteTimeI2C0_1(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            I2C_0_RELAYS_STATE |= 1UL << 1;
            setRelayState(I2C_0_RELAYS_STATE, I2C_0_ADDR);
	}
	else {
            I2C_0_RELAYS_STATE &= ~(1UL << 1);
            setRelayState(I2C_0_RELAYS_STATE, I2C_0_ADDR);
	}
    }
}

static void afterWriteTimeI2C0_2(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            I2C_0_RELAYS_STATE |= 1UL << 2;
            setRelayState(I2C_0_RELAYS_STATE, I2C_0_ADDR);
	}
	else {
            I2C_0_RELAYS_STATE &= ~(1UL << 2);
            setRelayState(I2C_0_RELAYS_STATE, I2C_0_ADDR);
	}
    }
}

static void afterWriteTimeI2C0_3(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            I2C_0_RELAYS_STATE |= 1UL << 3;
            setRelayState(I2C_0_RELAYS_STATE, I2C_0_ADDR);
	}
	else {
            I2C_0_RELAYS_STATE &= ~(1UL << 3);
            setRelayState(I2C_0_RELAYS_STATE, I2C_0_ADDR);
	}
    }
}


// I2C1
static void afterWriteTimeI2C1_0(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            I2C_1_RELAYS_STATE |= 1UL << 0; 
            setRelayState(I2C_1_RELAYS_STATE, I2C_1_ADDR);
	}
	else {
            I2C_1_RELAYS_STATE &= ~(1UL << 0);
            setRelayState(I2C_1_RELAYS_STATE, I2C_1_ADDR);
	}
    }
}

static void afterWriteTimeI2C1_1(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            I2C_1_RELAYS_STATE |= 1UL << 1; 
            setRelayState(I2C_1_RELAYS_STATE, I2C_1_ADDR);
	}
	else {
            I2C_1_RELAYS_STATE &= ~(1UL << 1);
            setRelayState(I2C_1_RELAYS_STATE, I2C_1_ADDR);
	}
    }
}

static void afterWriteTimeI2C1_2(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            I2C_1_RELAYS_STATE |= 1UL << 2; 
            setRelayState(I2C_1_RELAYS_STATE, I2C_1_ADDR);
	}
	else {
            I2C_1_RELAYS_STATE &= ~(1UL << 2);
            setRelayState(I2C_1_RELAYS_STATE, I2C_1_ADDR);
	}
    }
}

static void afterWriteTimeI2C1_3(UA_Server *server,
               const UA_NodeId *sessionId, void *sessionContext,
               const UA_NodeId *nodeId, void *nodeContext,
               const UA_NumericRange *range, const UA_DataValue *data) {
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32]) {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
	if (hrValue > 0){
            I2C_1_RELAYS_STATE |= 1UL << 3; 
            setRelayState(I2C_1_RELAYS_STATE, I2C_1_ADDR);
	}
	else {
            I2C_1_RELAYS_STATE &= ~(1UL << 3);
            setRelayState(I2C_1_RELAYS_STATE, I2C_1_ADDR);
	}
    }
}

static void addValueCallbackToCurrentTimeVariable(UA_Server *server) {
    // I2C0
    // relay 0
    UA_NodeId currentNodeId0 = UA_NODEID_STRING(1, "i2c0.relay0");
    UA_ValueCallback callback0 ;
    callback0.onRead = beforeReadTime;
    callback0.onWrite = afterWriteTimeI2C0_0;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId0, callback0);
    
    // relay 1
    UA_NodeId currentNodeId1 = UA_NODEID_STRING(1, "i2c0.relay1");
    UA_ValueCallback callback1 ;
    callback1.onRead = beforeReadTime;
    callback1.onWrite = afterWriteTimeI2C0_1;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId1, callback1);
    
    // relay 2
    UA_NodeId currentNodeId2 = UA_NODEID_STRING(1, "i2c0.relay2");
    UA_ValueCallback callback2 ;
    callback2.onRead = beforeReadTime;
    callback2.onWrite = afterWriteTimeI2C0_2;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId2, callback2);
    
    // relay 3
    UA_NodeId currentNodeId3 = UA_NODEID_STRING(1, "i2c0.relay3");
    UA_ValueCallback callback3 ;
    callback3.onRead = beforeReadTime;
    callback3.onWrite = afterWriteTimeI2C0_3;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId3, callback3);

    // I2C1
    // relay 0
    UA_NodeId currentNodeId4 = UA_NODEID_STRING(1, "i2c1.relay0");
    UA_ValueCallback callback4;
    callback4.onRead = beforeReadTime;
    callback4.onWrite = afterWriteTimeI2C1_0;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId4, callback4);
    
    // relay 1
    UA_NodeId currentNodeId5 = UA_NODEID_STRING(1, "i2c1.relay1");
    UA_ValueCallback callback5;
    callback5.onRead = beforeReadTime;
    callback5.onWrite = afterWriteTimeI2C1_1;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId5, callback5);
    
    // relay 2
    UA_NodeId currentNodeId6 = UA_NODEID_STRING(1, "i2c1.relay2");
    UA_ValueCallback callback6;
    callback6.onRead = beforeReadTime;
    callback6.onWrite = afterWriteTimeI2C1_2;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId6, callback6);
    
    // relay 2
    UA_NodeId currentNodeId7 = UA_NODEID_STRING(1, "i2c1.relay3");
    UA_ValueCallback callback7;
    callback7.onRead = beforeReadTime;
    callback7.onWrite = afterWriteTimeI2C1_3;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId7, callback7);
    
}

void safeShutdownI2CSlaveList() {
    /*
     * Perform a safe shutdown of all known I2C slaves
     */
    int i;
    int length;
    int addr;
    length = sizeof(I2C_SLAVE_ADDR_LIST) / sizeof(int);

    for(i = 0; i < length; i++)
    {
        addr = I2C_SLAVE_ADDR_LIST[i];
	if (addr!= 0) {
	    // properly initialized from CLI
	    setRelayState(0x00, addr);
	}
    }
}

static volatile UA_Boolean running = true;

static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

int main(int argc, char **argv) {
    int i;
    int length;
    long result;
    char *eptr;

    // handle comand line arguments
    if (argc >= 2) {
	// override default i2c block device from first cmd line argument
        I2C_BLOCK_DEVICE_NAME = argv[1];
	
	// if nothing specified on CLI set a default one
	if (argc<= 2){
	    I2C_SLAVE_ADDR_LIST[0] = I2C_0_ADDR;
	    printf("I2C_slave=0x%x\n", I2C_0_ADDR);
	}
	else {
	    // override list of I2C slaves addresses from CLI
	    for(i=2; i < argc; i++) 
	    {
	        // from CLI we get a hexidecimal string as a char (0x58 for example), convert to decimal
	        result = strtol(argv[i], &eptr, 16);
	        printf("I2C_slave=0x%lx\n", result);
	        I2C_SLAVE_ADDR_LIST[i -2] = result;
	    }
	}
	
    }
    else {
        I2C_BLOCK_DEVICE_NAME = DEFAULT_I2C_BLOCK_DEVICE_NAME;
    }

    printf("Block device=%s\n", I2C_BLOCK_DEVICE_NAME);
    
    // always start attached slaves from a know safe shutdown state
    safeShutdownI2CSlaveList();

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    UA_ServerConfig* config = UA_Server_getConfig(server);
    config->verifyRequestTimestamp = UA_RULEHANDLING_ACCEPT;

    addVariable(server);
    addValueCallbackToCurrentTimeVariable(server);

    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);

    // always leave attached slaves to a known safe shutdown state
    safeShutdownI2CSlaveList();
    
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
