/* 
 * OPC-UA server which controls MOD-IO's relays' state over OPC-UA protocol
 * Current format of exposed OPC-UA node variables is:
 * <mod-io_number>.<relay_number>
 * For example:
 * i2c0.relay0
 * i2c0.relay1
 * i2c1.relay0 
 * Based on 
 *   https://www.kernel.org/doc/Documentation/i2c/dev-interface
 *   https://github.com/open62541/open62541/blob/master/examples/tutorial_server_variable.c
 *   https://stackoverflow.com/questions/9642732/parsing-command-line-arguments-in-c
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
#include <argp.h>
#include <string.h>
#include "common.h"


// The default port of OPC-UA server
const int DEFAULT_OPC_UA_PORT = 4840;
const int DEFAULT_MODE = 0;

// CLI arguments handling
const char *argp_program_version = "OSIE OPC-UA coupler 0.0.1";
const char *argp_program_bug_address = "ivan@nexedi.com";
static char doc[] = "OPC-UA server which controls MOD-IO's relays' state over OPC-UA protocol.";
static char args_doc[] = "...";
static struct argp_option options[] = {
    {"port", 'p', "4840", 0, "Port to bind to."},
    {"device", 'd', "/dev/i2c-1", 0, "Linux block device path."},
    {"slave-address-list", 's', "0x58", 0, "Comma separated list of slave I2C addresses."},
    {"mode", 'm', "0", 0, "Set different modes of operation of coupler. Default (0) is set attached \
	                  I2C's state state. Virtual (1) which does NOT set any I2C slaves' state."},
    {"username", 'u', "", 0, "Username."},
    {"password", 'w', "", 0, "Password."},
    {"key", 'k', "", 0, "x509 key."},
    {"certificate", 'c', "", 0, "X509 certificate."},
    {0} 
};

struct arguments
{
    int mode;
    int port;
    char *device;
    char *slave_address_list;
    char *username;
    char *password;
    char *key;
    char *certificate;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch (key) {
    case 'p': 
	    arguments->port = arg ? atoi (arg) : DEFAULT_OPC_UA_PORT; 
	    break;
    case 'd': 
	    arguments->device = arg; 
	    break;
    case 's': 
	    arguments->slave_address_list = arg; 
	    break;
    case 'm': 
	    arguments->mode = arg ? atoi (arg) : DEFAULT_MODE; 
	    break;
    case 'u': 
	    arguments->username = arg; 
	    break;
    case 'w': 
	    arguments->password = arg; 
	    break;   
    case 'c': 
	    arguments->certificate = arg; 
	    break; 
    case 'k': 
	    arguments->key = arg; 
	    break; 
    case ARGP_KEY_ARG: 
	    return 0;
    default: 
	    return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

// global relay state
uint8_t I2C_0_RELAYS_STATE = 0; // state of 4 relays at I2C slave 0
uint8_t I2C_1_RELAYS_STATE = 0; // state of 4 relays at I2C slave 1
// XXX: what if we have more than 2 MOd-IOs / I2C slaves, we need better structure

// the default addresses of MOD-IOs
static char *DEFAULT_I2C_0_ADDR = "0x58";

// the list of attached I2C slaves
const int DEFAULT_I2C_SLAVE_ADDR = 0x58;
// XXX: make dynamic array
int I2C_SLAVE_ADDR_LIST[] = {0, 0, 0, 0, 0};

// the block device at host machine
static char *DEFAULT_I2C_BLOCK_DEVICE_NAME = "/dev/i2c-1";
char *I2C_BLOCK_DEVICE_NAME;

// global virtual mode needed for testing on x86 platform
bool I2C_VIRTUAL_MODE = 0;

static int getI2CSlaveListLength()
{
    /*
     * Return ONLY registred I2C slaves
     */
    int i;
    int addr;
    int counter = 0;
    int length = sizeof(I2C_SLAVE_ADDR_LIST) / sizeof(int);

    for (i = 0; i < length; i++)
    {
        addr = I2C_SLAVE_ADDR_LIST[i];
        if (addr != 0)
        {
            counter++;
        }
    }
    return counter;
}

static int setRelayState(int command, int i2c_addr)
{
    /*
     *  Set relays' state over I2C
     */
    int file;
    char filename[20];
    if (I2C_VIRTUAL_MODE)
    {
        // we're in a virtual mode, likely on x86 platform or without I2C support
        // simply do nothing
        return 0;
    }

    // step 1: open device
    file = open(I2C_BLOCK_DEVICE_NAME, O_RDWR);
    if (file < 0)
    {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error opening i2c device (0x%x).\n", i2c_addr);
        exit(1);
    }

    // step 2: address the slave by its address
    if (ioctl(file, I2C_SLAVE, i2c_addr) < 0)
    {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error addressing i2c slave (0x%x).\n", i2c_addr);
        exit(1);
    }

    // step 3: write command over I2c
    __u8 reg = 0x10; /* Device register to access */
    char buf[10];
    buf[0] = reg;
    buf[1] = command; //0x00 -all off, 0x0F - all 4 on
    if (write(file, buf, 3) != 3)
    {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Error writing to i2c slave (0x%x).\n", i2c_addr);
    }
    close(file);
}

static int getDigitalInputState(int i2c_addr, char **digital_input)
{
    /*
     *  get digital input state over I2C
     */
    int file;
    char filename[20];

    // step 1: open device
    file = open(I2C_BLOCK_DEVICE_NAME, O_RDWR);
    if (file < 0)
    {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error opening i2c device (0x%x).\n", i2c_addr);
        exit(1);
    }

    // step 2: address the slave by its address
    if (ioctl(file, I2C_SLAVE, i2c_addr) < 0)
    {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error addressing i2c slave (0x%x).\n", i2c_addr);
        exit(1);
    }

    // step 3: write command over I2c
    __u8 read_reg = 0x20; /* Device register to access */
    char read_buf[2];
    read_buf[0] = read_reg;
    if (write(file, read_buf, 1) != 1)
    {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Error writing to i2c slave (0x%x).\n", i2c_addr);
    }
    if (read(file, read_buf, 1) != 1)
    {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Error reading digital input from i2c slave (0x%x).\n", i2c_addr);
    }
    else
    {
        /* read_buf[0] contains the read byte */
        *digital_input = &read_buf[0];
    }
    close(file);
}

static int getAnalogInputStateAIN(int i2c_addr, int **analog_input, uint8_t read_reg)
{
    /*
     *  get digital input state over I2C
     */
    int file;
    char filename[20];

    // step 1: open device
    file = open(I2C_BLOCK_DEVICE_NAME, O_RDWR);
    if (file < 0)
    {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error opening i2c device (0x%x).\n", i2c_addr);
        exit(1);
    }

    // step 2: address the slave by its address
    if (ioctl(file, I2C_SLAVE, i2c_addr) < 0)
    {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Error addressing i2c slave (0x%x).\n", i2c_addr);
        exit(1);
    }

    // step 3: write command over I2c
    //__u8 read_reg = 0x30; /* Device register to access */
    char read_buf[10];
    read_buf[0] = read_reg;
    if (write(file, read_buf, 1) != 1)
    {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Error writing to i2c slave (0x%x).\n", i2c_addr);
    }
    if (read(file, read_buf, 2) != 2)
    {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Error reading analog input from i2c slave (0x%x).\n", i2c_addr);
    }
    else
    {
        int analog_data = 0;
	// based on https://github.com/OLIMEX/OLINUXINO/blob/master/SOFTWARE/A13/MOD-IO/main.c
	// since ADC is 10 bit we need to read and convert accordingly 2 bytes
	analog_data = read_buf[1];
	analog_data <<= 8;
	analog_data |= read_buf[0];
	*analog_input = &analog_data;
    }
    close(file);
}


void addIntegerVariableNode(UA_Server *server, char *node_id, char *node_description)
{
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

void addUIntegerVariableReadNode(UA_Server *server, char *node_id, char *node_description)
{
    UA_UInt32 myInteger = 0;
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);

    UA_VariableAttributes attr0 = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr0.value, &myInteger, &UA_TYPES[UA_TYPES_UINT32]);
    attr0.description = UA_LOCALIZEDTEXT("en-US", node_description);
    attr0.displayName = UA_LOCALIZEDTEXT("en-US", node_description);
    attr0.dataType = UA_TYPES[UA_TYPES_UINT32].typeId;
    attr0.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_NodeId myIntegerNodeId0 = UA_NODEID_STRING(1, node_id);
    UA_QualifiedName myIntegerName0 = UA_QUALIFIEDNAME(1, node_description);
    UA_Server_addVariableNode(server, myIntegerNodeId0, parentNodeId,
                              parentReferenceNodeId, myIntegerName0,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr0, NULL, NULL);
}

void addBooleanVariableReadNode(UA_Server *server, char *node_id, char *node_description)
{
    UA_Boolean myBoolean = false;
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);

    UA_VariableAttributes attr0 = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr0.value, &myBoolean, &UA_TYPES[UA_TYPES_BOOLEAN]);
    attr0.description = UA_LOCALIZEDTEXT("en-US", node_description);
    attr0.displayName = UA_LOCALIZEDTEXT("en-US", node_description);
    attr0.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    attr0.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_NodeId myIntegerNodeId0 = UA_NODEID_STRING(1, node_id);
    UA_QualifiedName myIntegerName0 = UA_QUALIFIEDNAME(1, node_description);
    UA_Server_addVariableNode(server, myIntegerNodeId0, parentNodeId,
                              parentReferenceNodeId, myIntegerName0,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr0, NULL, NULL);
}

static void addVariable(UA_Server *server)
{
    /* 
     * Create all variables representing MOD-IO's relays
     */
    int length = getI2CSlaveListLength();
    if (length >= 1)
    {
        // IC2-0
        addIntegerVariableNode(server, "i2c0.relay0", "I2C0 / Relay 0");
        addIntegerVariableNode(server, "i2c0.relay1", "I2C0 / Relay 1");
        addIntegerVariableNode(server, "i2c0.relay2", "I2C0 / Relay 2");
        addIntegerVariableNode(server, "i2c0.relay3", "I2C0 / Relay 3");
        addBooleanVariableReadNode(server, "i2c0.in0", "I2C0 / Digital Input 0");
        addBooleanVariableReadNode(server, "i2c0.in1", "I2C0 / Digital Input 1");
        addBooleanVariableReadNode(server, "i2c0.in2", "I2C0 / Digital Input 2");
        addBooleanVariableReadNode(server, "i2c0.in3", "I2C0 / Digital Input 3");
        addUIntegerVariableReadNode(server, "i2c0.ain0", "I2C0 / Analog Input 0");
        addUIntegerVariableReadNode(server, "i2c0.ain1", "I2C0 / Analog Input 1");
        addUIntegerVariableReadNode(server, "i2c0.ain2", "I2C0 / Analog Input 2");
        addUIntegerVariableReadNode(server, "i2c0.ain3", "I2C0 / Analog Input 3");
    }
    if (length >= 2)
    {
        // IC2-1
        addIntegerVariableNode(server, "i2c1.relay0", "I2C1 / Relay 0");
        addIntegerVariableNode(server, "i2c1.relay1", "I2C1 / Relay 1");
        addIntegerVariableNode(server, "i2c1.relay2", "I2C1 / Relay 2");
        addIntegerVariableNode(server, "i2c1.relay3", "I2C1 / Relay 3");
        addBooleanVariableReadNode(server, "i2c1.in0", "I2C1 / Digital Input 0");
        addBooleanVariableReadNode(server, "i2c1.in1", "I2C1 / Digital Input 1");
        addBooleanVariableReadNode(server, "i2c1.in2", "I2C1 / Digital Input 2");
        addBooleanVariableReadNode(server, "i2c1.in3", "I2C1 / Digital Input 3");
        addUIntegerVariableReadNode(server, "i2c1.ain0", "I2C1 / Analog Input 0");
        addUIntegerVariableReadNode(server, "i2c1.ain1", "I2C1 / Analog Input 1");
        addUIntegerVariableReadNode(server, "i2c1.ain2", "I2C1 / Analog Input 2");
        addUIntegerVariableReadNode(server, "i2c1.ain3", "I2C1 / Analog Input 3");
    }
}

/* Connect to variables to physical relays
 *
 */
static void beforeReadTime(UA_Server *server,
                           const UA_NodeId *sessionId, void *sessionContext,
                           const UA_NodeId *nodeid, void *nodeContext,
                           const UA_NumericRange *range, const UA_DataValue *data)
{
}
static void beforeReadTimeI2C0Ain0(UA_Server *server,
                                   const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_NodeId *nodeid, void *nodeContext,
                                   const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[0];
    int *data_input = 0;
    uint8_t read_addr =0x30;
    if (!I2C_VIRTUAL_MODE) {
      getAnalogInputStateAIN(addr, &data_input, read_addr);
      if (data->value.type == &UA_TYPES[UA_TYPES_UINT32])
      {
        *(UA_UInt32 *)data->value.data = *data_input;
      }
    }
}

static void beforeReadTimeI2C0Ain1(UA_Server *server,
                                   const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_NodeId *nodeid, void *nodeContext,
                                   const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[0];
    int *data_input = 0;
    uint8_t read_addr =0x31;
    if (!I2C_VIRTUAL_MODE) {
      getAnalogInputStateAIN(addr, &data_input, read_addr);
      if (data->value.type == &UA_TYPES[UA_TYPES_UINT32])
      {
        *(UA_UInt32 *)data->value.data = *data_input;
      }
    }
}

static void beforeReadTimeI2C0Ain2(UA_Server *server,
                                   const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_NodeId *nodeid, void *nodeContext,
                                   const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[0];
    int *data_input = 0;
    uint8_t read_addr =0x32;
    if (!I2C_VIRTUAL_MODE) {
      getAnalogInputStateAIN(addr, &data_input, read_addr);
      if (data->value.type == &UA_TYPES[UA_TYPES_UINT32])
      {
        *(UA_UInt32 *)data->value.data = *data_input;
      }
    }
}

static void beforeReadTimeI2C0Ain3(UA_Server *server,
                                   const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_NodeId *nodeid, void *nodeContext,
                                   const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[0];
    int *data_input = 0;
    uint8_t read_addr =0x33;
    if (!I2C_VIRTUAL_MODE) {
      getAnalogInputStateAIN(addr, &data_input, read_addr);
      if (data->value.type == &UA_TYPES[UA_TYPES_UINT32])
      {
        *(UA_UInt32 *)data->value.data = *data_input;
      }
    }
}
static void beforeReadTimeI2C1Ain0(UA_Server *server,
                                   const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_NodeId *nodeid, void *nodeContext,
                                   const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[1];
    int *data_input = 0;
    uint8_t read_addr =0x30;
    if (!I2C_VIRTUAL_MODE) {
      getAnalogInputStateAIN(addr, &data_input, read_addr);
      if (data->value.type == &UA_TYPES[UA_TYPES_UINT32])
      {
        *(UA_UInt32 *)data->value.data = *data_input;
      }
    }
}

static void beforeReadTimeI2C1Ain1(UA_Server *server,
                                   const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_NodeId *nodeid, void *nodeContext,
                                   const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[1];
    int *data_input = 0;    
    uint8_t read_addr =0x31;
    if (!I2C_VIRTUAL_MODE) {
      getAnalogInputStateAIN(addr, &data_input, read_addr);
      if (data->value.type == &UA_TYPES[UA_TYPES_UINT32])
      {
        *(UA_UInt32 *)data->value.data = *data_input;
      }
    }
}

static void beforeReadTimeI2C1Ain2(UA_Server *server,
                                   const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_NodeId *nodeid, void *nodeContext,
                                   const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[1];
    int *data_input = 0;
    uint8_t read_addr =0x32;
    if (!I2C_VIRTUAL_MODE) {
      getAnalogInputStateAIN(addr, &data_input, read_addr);
      if (data->value.type == &UA_TYPES[UA_TYPES_UINT32])
      {
        *(UA_UInt32 *)data->value.data = *data_input;
      }
    }
}

static void beforeReadTimeI2C1Ain3(UA_Server *server,
                                   const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_NodeId *nodeid, void *nodeContext,
                                   const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[1];
    int *data_input = 0;
    uint8_t read_addr =0x33;
    if (!I2C_VIRTUAL_MODE) {
      getAnalogInputStateAIN(addr, &data_input, read_addr);
      if (data->value.type == &UA_TYPES[UA_TYPES_UINT32])
      {
        *(UA_UInt32 *)data->value.data = *data_input;
      }
    }
}

static void beforeReadTimeI2C0In0(UA_Server *server,
                                  const UA_NodeId *sessionId, void *sessionContext,
                                  const UA_NodeId *nodeid, void *nodeContext,
                                  const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[0];
    char *data_input = 0;
    if (!I2C_VIRTUAL_MODE) {
      getDigitalInputState(addr, &data_input);
      if ((*data_input) & (1UL << 0))
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = true;
        }
      }
      else
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = false;
        }
      }
    }
}

static void beforeReadTimeI2C0In1(UA_Server *server,
                                  const UA_NodeId *sessionId, void *sessionContext,
                                  const UA_NodeId *nodeid, void *nodeContext,
                                  const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[0];
    char *data_input = 0;
    if (!I2C_VIRTUAL_MODE) {
      getDigitalInputState(addr, &data_input);
      if ((*data_input) & (1UL << 1))
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = true;
        }
      }
      else
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = false;
        }
      }
    }
}

static void beforeReadTimeI2C0In2(UA_Server *server,
                                  const UA_NodeId *sessionId, void *sessionContext,
                                  const UA_NodeId *nodeid, void *nodeContext,
                                  const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[0];
    char *data_input = 0;
    if (!I2C_VIRTUAL_MODE) {
      getDigitalInputState(addr, &data_input);
      if ((*data_input) & (1UL << 2))
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = true;
        }
      }
      else
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = false;
        }
      }
    }
}

static void beforeReadTimeI2C0In3(UA_Server *server,
                                  const UA_NodeId *sessionId, void *sessionContext,
                                  const UA_NodeId *nodeid, void *nodeContext,
                                  const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[0];
    char *data_input = 0;
    if (!I2C_VIRTUAL_MODE) {     
      getDigitalInputState(addr, &data_input);
      if ((*data_input) & (1UL << 3))
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = true;
        }
      }
      else
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = false;
        }
      }
    }
}
static void beforeReadTimeI2C1In0(UA_Server *server,
                                  const UA_NodeId *sessionId, void *sessionContext,
                                  const UA_NodeId *nodeid, void *nodeContext,
                                  const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[1];
    char *data_input = 0;
    if (!I2C_VIRTUAL_MODE) {   
      getDigitalInputState(addr, &data_input);
      if ((*data_input) & (1UL << 0))
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = true;
        }
      }
      else
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = false;
        }
      }
    }
}

static void beforeReadTimeI2C1In1(UA_Server *server,
                                  const UA_NodeId *sessionId, void *sessionContext,
                                  const UA_NodeId *nodeid, void *nodeContext,
                                  const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[1];
    char *data_input = 0;
    if (!I2C_VIRTUAL_MODE) {
      getDigitalInputState(addr, &data_input);
      if ((*data_input) & (1UL << 1))
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = true;
        }
      }
      else
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = false;
        }
      }
    }
}

static void beforeReadTimeI2C1In2(UA_Server *server,
                                  const UA_NodeId *sessionId, void *sessionContext,
                                  const UA_NodeId *nodeid, void *nodeContext,
                                  const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[1];
    char *data_input = 0;
    if (!I2C_VIRTUAL_MODE) {
      getDigitalInputState(addr, &data_input);
      if ((*data_input) & (1UL << 2))
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = true;
        }
      }
      else
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = false;
        }
      }
    }
}

static void beforeReadTimeI2C1In3(UA_Server *server,
                                  const UA_NodeId *sessionId, void *sessionContext,
                                  const UA_NodeId *nodeid, void *nodeContext,
                                  const UA_NumericRange *range, const UA_DataValue *data)
{
    int addr = I2C_SLAVE_ADDR_LIST[1];
    char *data_input = 0;
    if (!I2C_VIRTUAL_MODE) {
      getDigitalInputState(addr, &data_input);
      if ((*data_input) & (1UL << 3))
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = true;
        }
      }
      else
      {
        if (data->value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            *(UA_Boolean *)data->value.data = false;
        }
      }
    }
}

static void afterWriteTime(UA_Server *server,
                           const UA_NodeId *sessionId, void *sessionContext,
                           const UA_NodeId *nodeId, void *nodeContext,
                           const UA_NumericRange *range, const UA_DataValue *data)
{
    // nothing to do as not yet needed.
}

// XXX: having afterWriteTime{0..3} is not needed and maybe with introspection of context we can
// write only one callback function
static void afterWriteTimeI2C0_0(UA_Server *server,
                                 const UA_NodeId *sessionId, void *sessionContext,
                                 const UA_NodeId *nodeId, void *nodeContext,
                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
        int addr = I2C_SLAVE_ADDR_LIST[0];
        if (hrValue > 0)
        {
            I2C_0_RELAYS_STATE |= 1UL << 0;
            setRelayState(I2C_0_RELAYS_STATE, addr);
        }
        else
        {
            I2C_0_RELAYS_STATE &= ~(1UL << 0);
            setRelayState(I2C_0_RELAYS_STATE, addr);
        }
    }
}

static void afterWriteTimeI2C0_1(UA_Server *server,
                                 const UA_NodeId *sessionId, void *sessionContext,
                                 const UA_NodeId *nodeId, void *nodeContext,
                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
        int addr = I2C_SLAVE_ADDR_LIST[0];
        if (hrValue > 0)
        {
            I2C_0_RELAYS_STATE |= 1UL << 1;
            setRelayState(I2C_0_RELAYS_STATE, addr);
        }
        else
        {
            I2C_0_RELAYS_STATE &= ~(1UL << 1);
            setRelayState(I2C_0_RELAYS_STATE, addr);
        }
    }
}

static void afterWriteTimeI2C0_2(UA_Server *server,
                                 const UA_NodeId *sessionId, void *sessionContext,
                                 const UA_NodeId *nodeId, void *nodeContext,
                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
        int addr = I2C_SLAVE_ADDR_LIST[0];
        if (hrValue > 0)
        {
            I2C_0_RELAYS_STATE |= 1UL << 2;
            setRelayState(I2C_0_RELAYS_STATE, addr);
        }
        else
        {
            I2C_0_RELAYS_STATE &= ~(1UL << 2);
            setRelayState(I2C_0_RELAYS_STATE, addr);
        }
    }
}

static void afterWriteTimeI2C0_3(UA_Server *server,
                                 const UA_NodeId *sessionId, void *sessionContext,
                                 const UA_NodeId *nodeId, void *nodeContext,
                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
        int addr = I2C_SLAVE_ADDR_LIST[0];
        if (hrValue > 0)
        {
            I2C_0_RELAYS_STATE |= 1UL << 3;
            setRelayState(I2C_0_RELAYS_STATE, addr);
        }
        else
        {
            I2C_0_RELAYS_STATE &= ~(1UL << 3);
            setRelayState(I2C_0_RELAYS_STATE, addr);
        }
    }
}

// I2C1
static void afterWriteTimeI2C1_0(UA_Server *server,
                                 const UA_NodeId *sessionId, void *sessionContext,
                                 const UA_NodeId *nodeId, void *nodeContext,
                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
        int addr = I2C_SLAVE_ADDR_LIST[1];
        if (hrValue > 0)
        {
            I2C_1_RELAYS_STATE |= 1UL << 0;
            setRelayState(I2C_1_RELAYS_STATE, addr);
        }
        else
        {
            I2C_1_RELAYS_STATE &= ~(1UL << 0);
            setRelayState(I2C_1_RELAYS_STATE, addr);
        }
    }
}

static void afterWriteTimeI2C1_1(UA_Server *server,
                                 const UA_NodeId *sessionId, void *sessionContext,
                                 const UA_NodeId *nodeId, void *nodeContext,
                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
        int addr = I2C_SLAVE_ADDR_LIST[1];
        if (hrValue > 0)
        {
            I2C_1_RELAYS_STATE |= 1UL << 1;
            setRelayState(I2C_1_RELAYS_STATE, addr);
        }
        else
        {
            I2C_1_RELAYS_STATE &= ~(1UL << 1);
            setRelayState(I2C_1_RELAYS_STATE, addr);
        }
    }
}

static void afterWriteTimeI2C1_2(UA_Server *server,
                                 const UA_NodeId *sessionId, void *sessionContext,
                                 const UA_NodeId *nodeId, void *nodeContext,
                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
        int addr = I2C_SLAVE_ADDR_LIST[1];
        if (hrValue > 0)
        {
            I2C_1_RELAYS_STATE |= 1UL << 2;
            setRelayState(I2C_1_RELAYS_STATE, addr);
        }
        else
        {
            I2C_1_RELAYS_STATE &= ~(1UL << 2);
            setRelayState(I2C_1_RELAYS_STATE, addr);
        }
    }
}

static void afterWriteTimeI2C1_3(UA_Server *server,
                                 const UA_NodeId *sessionId, void *sessionContext,
                                 const UA_NodeId *nodeId, void *nodeContext,
                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    if (data->value.type == &UA_TYPES[UA_TYPES_INT32])
    {
        UA_Int32 hrValue = *(UA_Int32 *)data->value.data;
        int addr = I2C_SLAVE_ADDR_LIST[1];
        if (hrValue > 0)
        {
            I2C_1_RELAYS_STATE |= 1UL << 3;
            setRelayState(I2C_1_RELAYS_STATE, addr);
        }
        else
        {
            I2C_1_RELAYS_STATE &= ~(1UL << 3);
            setRelayState(I2C_1_RELAYS_STATE, addr);
        }
    }
}

static void addValueCallbackToCurrentTimeVariable(UA_Server *server)
{
    int length = getI2CSlaveListLength();
    // I2C0
    // relay 0
    UA_NodeId currentNodeId0 = UA_NODEID_STRING(1, "i2c0.relay0");
    UA_ValueCallback callback0;
    callback0.onRead = beforeReadTime;
    callback0.onWrite = afterWriteTimeI2C0_0;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId0, callback0);

    // relay 1
    UA_NodeId currentNodeId1 = UA_NODEID_STRING(1, "i2c0.relay1");
    UA_ValueCallback callback1;
    callback1.onRead = beforeReadTime;
    callback1.onWrite = afterWriteTimeI2C0_1;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId1, callback1);

    // relay 2
    UA_NodeId currentNodeId2 = UA_NODEID_STRING(1, "i2c0.relay2");
    UA_ValueCallback callback2;
    callback2.onRead = beforeReadTime;
    callback2.onWrite = afterWriteTimeI2C0_2;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId2, callback2);

    // relay 3
    UA_NodeId currentNodeId3 = UA_NODEID_STRING(1, "i2c0.relay3");
    UA_ValueCallback callback3;
    callback3.onRead = beforeReadTime;
    callback3.onWrite = afterWriteTimeI2C0_3;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId3, callback3);

    // Digital input 0
    UA_NodeId currentNodeId4 = UA_NODEID_STRING(1, "i2c0.in0");
    UA_ValueCallback callback4;
    callback4.onRead = beforeReadTimeI2C0In0;
    callback4.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId4, callback4);

    // Digital input 1
    UA_NodeId currentNodeId5 = UA_NODEID_STRING(1, "i2c0.in1");
    UA_ValueCallback callback5;
    callback5.onRead = beforeReadTimeI2C0In1;
    callback5.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId5, callback5);

    // Digital input 2
    UA_NodeId currentNodeId6 = UA_NODEID_STRING(1, "i2c0.in2");
    UA_ValueCallback callback6;
    callback6.onRead = beforeReadTimeI2C0In2;
    callback6.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId6, callback6);

    // Digital input 3
    UA_NodeId currentNodeId7 = UA_NODEID_STRING(1, "i2c0.in3");
    UA_ValueCallback callback7;
    callback7.onRead = beforeReadTimeI2C0In3;
    callback7.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId7, callback7);

    // Analog input 0
    UA_NodeId currentNodeId8 = UA_NODEID_STRING(1, "i2c0.ain0");
    UA_ValueCallback callback8;
    callback8.onRead = beforeReadTimeI2C0Ain0;
    callback8.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId8, callback8);

    // Analog input 1
    UA_NodeId currentNodeId9 = UA_NODEID_STRING(1, "i2c0.ain1");
    UA_ValueCallback callback9;
    callback9.onRead = beforeReadTimeI2C0Ain1;
    callback9.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId9, callback9);

    // Analog input 2
    UA_NodeId currentNodeId10 = UA_NODEID_STRING(1, "i2c0.ain2");
    UA_ValueCallback callback10;
    callback10.onRead = beforeReadTimeI2C0Ain2;
    callback10.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId10, callback10);

    // Analog input 3
    UA_NodeId currentNodeId11 = UA_NODEID_STRING(1, "i2c0.ain3");
    UA_ValueCallback callback11;
    callback11.onRead = beforeReadTimeI2C0Ain3;
    callback11.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId11, callback11);

    if (length > 1)
    {
        // I2C1
        // relay 0
        UA_NodeId currentNodeId12 = UA_NODEID_STRING(1, "i2c1.relay0");
        UA_ValueCallback callback12;
        callback12.onRead = beforeReadTime;
        callback12.onWrite = afterWriteTimeI2C1_0;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId12, callback12);

        // relay 1
        UA_NodeId currentNodeId13 = UA_NODEID_STRING(1, "i2c1.relay1");
        UA_ValueCallback callback13;
        callback13.onRead = beforeReadTime;
        callback13.onWrite = afterWriteTimeI2C1_1;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId13, callback13);

        // relay 2
        UA_NodeId currentNodeId14 = UA_NODEID_STRING(1, "i2c1.relay2");
        UA_ValueCallback callback14;
        callback14.onRead = beforeReadTime;
        callback14.onWrite = afterWriteTimeI2C1_2;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId14, callback14);

        // relay 2
        UA_NodeId currentNodeId15 = UA_NODEID_STRING(1, "i2c1.relay3");
        UA_ValueCallback callback15;
        callback15.onRead = beforeReadTime;
        callback15.onWrite = afterWriteTimeI2C1_3;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId15, callback15);

        // Digital input 0
        UA_NodeId currentNodeId16 = UA_NODEID_STRING(1, "i2c1.in0");
        UA_ValueCallback callback16;
        callback16.onRead = beforeReadTimeI2C1In0;
        callback16.onWrite = afterWriteTime;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId16, callback16);

        // Digital input 1
        UA_NodeId currentNodeId17 = UA_NODEID_STRING(1, "i2c1.in1");
        UA_ValueCallback callback17;
        callback17.onRead = beforeReadTimeI2C1In1;
        callback17.onWrite = afterWriteTime;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId17, callback17);

        // Digital input 2
        UA_NodeId currentNodeId18 = UA_NODEID_STRING(1, "i2c1.in2");
        UA_ValueCallback callback18;
        callback18.onRead = beforeReadTimeI2C1In2;
        callback18.onWrite = afterWriteTime;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId18, callback18);

        // Digital input 3
        UA_NodeId currentNodeId19 = UA_NODEID_STRING(1, "i2c1.in3");
        UA_ValueCallback callback19;
        callback19.onRead = beforeReadTimeI2C1In3;
        callback19.onWrite = afterWriteTime;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId19, callback19);

        // Analog input 0
        UA_NodeId currentNodeId20 = UA_NODEID_STRING(1, "i2c1.ain0");
        UA_ValueCallback callback20;
        callback20.onRead = beforeReadTimeI2C1Ain0;
        callback20.onWrite = afterWriteTime;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId20, callback20);

        // Analog input 1
        UA_NodeId currentNodeId21 = UA_NODEID_STRING(1, "i2c1.ain1");
        UA_ValueCallback callback21;
        callback21.onRead = beforeReadTimeI2C1Ain1;
        callback21.onWrite = afterWriteTime;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId21, callback21);

        // Analog input 2
        UA_NodeId currentNodeId22 = UA_NODEID_STRING(1, "i2c1.ain2");
        UA_ValueCallback callback22;
        callback22.onRead = beforeReadTimeI2C1Ain2;
        callback22.onWrite = afterWriteTime;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId22, callback22);

        // Analog input 3
        UA_NodeId currentNodeId23 = UA_NODEID_STRING(1, "i2c1.ain3");
        UA_ValueCallback callback23;
        callback23.onRead = beforeReadTimeI2C1Ain3;
        callback23.onWrite = afterWriteTime;
        UA_Server_setVariableNode_valueCallback(server, currentNodeId23, callback23);
    }
}

void safeShutdownI2CSlaveList()
{
    /*
     * Perform a safe shutdown of all known I2C slaves
     */
    int i;
    int length;
    int addr;
    length = sizeof(I2C_SLAVE_ADDR_LIST) / sizeof(int);

    for (i = 0; i < length; i++)
    {
        addr = I2C_SLAVE_ADDR_LIST[i];
        if (addr != 0)
        {
            // properly initialized from CLI
            setRelayState(0x00, addr);
        }
    }
}

static volatile UA_Boolean running = true;

static void stopHandler(int sign)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

int main(int argc, char **argv)
{
    int i;
    int length;
    long result;
    char *eptr;

    // handle command line arguments
    struct arguments arguments;
    arguments.port = DEFAULT_OPC_UA_PORT;
    arguments.mode = DEFAULT_MODE;
    arguments.device = DEFAULT_I2C_BLOCK_DEVICE_NAME;
    arguments.slave_address_list = DEFAULT_I2C_0_ADDR;
    arguments.username = "";
    arguments.password = "";
    arguments.key = "";
    arguments.certificate = "";
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    printf("Mode=%d\n", arguments.mode);
    printf("Listening port=%d\n", arguments.port);
    printf("Block device=%s\n", arguments.device);
    printf("Slave address list=%s\n", arguments.slave_address_list);
    printf("key=%s\n", arguments.key);
    printf("certificate=%s\n", arguments.certificate);

    // transfer to global variables (CLI input)
    I2C_VIRTUAL_MODE = arguments.mode;
    I2C_BLOCK_DEVICE_NAME = arguments.device;

    // convert arguments.slave_address_list -> I2C_SLAVE_ADDR_LIST
    i = 0;
    char *token = strtok(arguments.slave_address_list, ",");
    while (token != NULL)
    {
        // from CLI we get a hexidecimal string as a char (0x58 for example), convert to decimal
        result = strtol(token, &eptr, 16);
        I2C_SLAVE_ADDR_LIST[i++] = result;
        token = strtok(NULL, ",");
    }

    // always start attached slaves from a know safe shutdown state
    safeShutdownI2CSlaveList();

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setMinimal(UA_Server_getConfig(server), arguments.port, NULL);
    UA_ServerConfig *config = UA_Server_getConfig(server);
    config->verifyRequestTimestamp = UA_RULEHANDLING_ACCEPT;

    // add variables representing physical relarys / inputs, etc
    addVariable(server);
    addValueCallbackToCurrentTimeVariable(server);

    /* Disable anonymous logins, enable two user/password logins */
    if (strlen(arguments.username) > 0 && strlen(arguments.password) > 0){
      char *username = arguments.username;
      char *password = arguments.password;
      UA_UsernamePasswordLogin logins[1] = {
          {UA_STRING(arguments.username), UA_STRING(arguments.password)},
      };

      config->accessControl.clear(&config->accessControl);
      UA_StatusCode retval1 = UA_AccessControl_default(config, false, NULL,
               &config->securityPolicies[config->securityPoliciesSize-1].policyUri, 1, logins);
    }

    /* Enable x509 */
#ifdef UA_ENABLE_ENCRYPTION
    if (strlen(arguments.key) > 0 && strlen(arguments.certificate) > 0){
      char *key_filename = arguments.key;
      char *certificate_filename = arguments.certificate;
      /* Load certificate and private key */
      UA_ByteString certificate = loadFile(certificate_filename);
      UA_ByteString privateKey  = loadFile(key_filename);
   
      /* Load the trustlist - not used thus 0 */
      size_t trustListSize = 0;
      UA_STACKARRAY(UA_ByteString, trustList, trustListSize);
      
      /* Loading of a issuer list, not used in this application */
      size_t issuerListSize = 0;
      UA_ByteString *issuerList = NULL;
      
      /* Loading of a revocation list currently unsupported */
      UA_ByteString *revocationList = NULL;
      size_t revocationListSize = 0;
      UA_StatusCode retval =
        UA_ServerConfig_setDefaultWithSecurityPolicies(config, 4841, // XXX: why not use 4840 ?
                                                       &certificate, &privateKey,
                                                       trustList, trustListSize,
                                                       issuerList, issuerListSize,
                                                       revocationList, revocationListSize);
      //The place to fill the hole is very important
      config->applicationDescription.applicationUri = UA_STRING_ALLOC("urn:open62541.server.application");
    }
#endif
    
    // run server
    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);

    // always leave attached slaves to a known safe shutdown state
    safeShutdownI2CSlaveList();

    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
