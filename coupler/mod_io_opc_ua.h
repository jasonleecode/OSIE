/*
 * OPC-UA code representation of MOD-IOs connected to a Lime2
 */

#include <open62541/server.h>

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
        // used only for debuging with logical analyzer
        if (CURRENT_GPIO_MODE == 2) setGPIO();

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
