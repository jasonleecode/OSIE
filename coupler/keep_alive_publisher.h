/*
Keep alive implementation for couplers based on OPC UA's pub/sub mechanism
*/

#include "keep_alive.h"

UA_NodeId connectionIdent, publishedDataSetIdent, writerGroupIdent;

static void addPubSubConnection(UA_Server *server, UA_String *transportProfile,
                    UA_NetworkAddressUrlDataType *networkAddressUrl){
    UA_PubSubConnectionConfig connectionConfig;
    memset(&connectionConfig, 0, sizeof(connectionConfig));
    connectionConfig.name = UA_STRING("UADP Connection 1");
    connectionConfig.transportProfileUri = *transportProfile;
    connectionConfig.enabled = UA_TRUE;
    if (strlen(NETWORK_INTERFACE) > 0){
        // set preferred network interface for Pub / Sub
        networkAddressUrl->networkInterface = UA_STRING(NETWORK_INTERFACE);
    }
    UA_Variant_setScalar(&connectionConfig.address, networkAddressUrl,
                         &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
    /* Changed to static publisherId from random generation to identify
     * the publisher on Subscriber side */
    connectionConfig.publisherId.numeric = PUBLISHER_ID;
    UA_Server_addPubSubConnection(server, &connectionConfig, &connectionIdent);
}

static void addPublishedDataSet(UA_Server *server) {
    /* The PublishedDataSetConfig contains all necessary public
    * information for the creation of a new PublishedDataSet */
    UA_PublishedDataSetConfig publishedDataSetConfig;
    memset(&publishedDataSetConfig, 0, sizeof(UA_PublishedDataSetConfig));
    publishedDataSetConfig.publishedDataSetType = UA_PUBSUB_DATASET_PUBLISHEDITEMS;
    publishedDataSetConfig.name = UA_STRING("Demo PDS");
    /* Create new PublishedDataSet based on the PublishedDataSetConfig. */
    UA_Server_addPublishedDataSet(server, &publishedDataSetConfig, &publishedDataSetIdent);
}

static void addWriterGroup(UA_Server *server) {
    /* Now we create a new WriterGroupConfig and add the group to the existing
     * PubSubConnection. */
    UA_WriterGroupConfig writerGroupConfig;
    memset(&writerGroupConfig, 0, sizeof(UA_WriterGroupConfig));
    writerGroupConfig.name = UA_STRING("Demo WriterGroup");
    writerGroupConfig.publishingInterval = PUBLISHING_INTERVAL;
    writerGroupConfig.keepAliveTime = PUBLISHING_INTERVAL;
    writerGroupConfig.enabled = UA_FALSE;
    writerGroupConfig.writerGroupId = WRITER_GROUP_ID;
    writerGroupConfig.encodingMimeType = UA_PUBSUB_ENCODING_UADP;
    writerGroupConfig.messageSettings.encoding             = UA_EXTENSIONOBJECT_DECODED;
    writerGroupConfig.messageSettings.content.decoded.type = &UA_TYPES[UA_TYPES_UADPWRITERGROUPMESSAGEDATATYPE];
    /* The configuration flags for the messages are encapsulated inside the
     * message- and transport settings extension objects. These extension
     * objects are defined by the standard. e.g.
     * UadpWriterGroupMessageDataType */
    UA_UadpWriterGroupMessageDataType *writerGroupMessage  = UA_UadpWriterGroupMessageDataType_new();
    /* Change message settings of writerGroup to send PublisherId,
     * WriterGroupId in GroupHeader and DataSetWriterId in PayloadHeader
     * of NetworkMessage */
    writerGroupMessage->networkMessageContentMask          = (UA_UadpNetworkMessageContentMask)(UA_UADPNETWORKMESSAGECONTENTMASK_PUBLISHERID |
                                                              (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_GROUPHEADER |
                                                              (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_WRITERGROUPID |
                                                              (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_PAYLOADHEADER);
    writerGroupConfig.messageSettings.content.decoded.data = writerGroupMessage;
    UA_Server_addWriterGroup(server, connectionIdent, &writerGroupConfig, &writerGroupIdent);
    UA_Server_setWriterGroupOperational(server, writerGroupIdent);
    UA_UadpWriterGroupMessageDataType_delete(writerGroupMessage);
}

static void addDataSetWriter(UA_Server *server) {
    /* We need now a DataSetWriter within the WriterGroup. This means we must
     * create a new DataSetWriterConfig and add call the addWriterGroup function. */
    UA_NodeId dataSetWriterIdent;
    UA_DataSetWriterConfig dataSetWriterConfig;
    memset(&dataSetWriterConfig, 0, sizeof(UA_DataSetWriterConfig));
    dataSetWriterConfig.name = UA_STRING("Demo DataSetWriter");
    dataSetWriterConfig.dataSetWriterId = DATASET_WRITER_ID;
    dataSetWriterConfig.keyFrameCount = 10;
    UA_Server_addDataSetWriter(server, writerGroupIdent, publishedDataSetIdent,
                               &dataSetWriterConfig, &dataSetWriterIdent);
}

typedef struct PublishedVariable {
    char *name;
    char *description;
    void * UA_RESTRICT pdefaultValue;
    int type;
} PublishedVariable;

static void addPubSubVariable(UA_Server *server, PublishedVariable varDetails) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Variant_setScalar(&attr.value, varDetails.pdefaultValue, &UA_TYPES[varDetails.type]);
    attr.description = UA_LOCALIZEDTEXT("en-US", varDetails.description);
    attr.displayName = UA_LOCALIZEDTEXT("en-US", varDetails.description);
    attr.dataType = UA_TYPES[varDetails.type].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_Server_addVariableNode(server, UA_NODEID_STRING(1, varDetails.name),
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                      UA_QUALIFIEDNAME(1, varDetails.description),
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                      attr, NULL, NULL);
}

static void addPubSubDataSetField(UA_Server *server, PublishedVariable varDetails) {
    UA_NodeId dataSetFieldIdent;
    UA_DataSetFieldConfig dataSetFieldConfig;
    memset(&dataSetFieldConfig, 0, sizeof(UA_DataSetFieldConfig));
    dataSetFieldConfig.dataSetFieldType = UA_PUBSUB_DATASETFIELD_VARIABLE;
    dataSetFieldConfig.field.variable.fieldNameAlias = UA_STRING(varDetails.description);
    dataSetFieldConfig.field.variable.promotedField = UA_FALSE;
    dataSetFieldConfig.field.variable.publishParameters.publishedVariable =
    UA_NODEID_STRING(1, varDetails.name);
    dataSetFieldConfig.field.variable.publishParameters.attributeId = UA_ATTRIBUTEID_VALUE;
    UA_Server_addDataSetField(server, publishedDataSetIdent,
                              &dataSetFieldConfig, &dataSetFieldIdent);
}


void callbackTicHeartBeat()
{
    /* Increase periodically heart beats of the server */
    HEART_BEATS += 1;
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "\theart_beat %d", HEART_BEATS);

    // set OPC UA's heat_beat node value
    UA_NodeId myFloatNodeId = UA_NODEID_STRING(1, "heart_beat");
    
    // heart_beat format is <ID_of_coupler>.<heart_beats>
    int len = snprintf(NULL, 0, "%d", HEART_BEATS);
    char *result1 = malloc(len + 1);
    snprintf(result1, len + 1, "%d", HEART_BEATS);

    int len1 = snprintf(NULL, 0, "%d", COUPLER_ID);
    char *result2 = malloc(len1 + 1);
    snprintf(result2, len1 + 1, "%d", COUPLER_ID);
    
    // concat
    strcat(result2, ".");
    strcat(result2, result1);
   
    char * end_ptr;
    float final_result = strtof(result2, &end_ptr );

    UA_Float myFloat = final_result;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myFloat, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_writeValue(server, myFloatNodeId, myVar);
}


static void enablePublishHeartBeat(UA_Server *server, UA_ServerConfig *config){
    int i;
    // add a callback which will increment heart beat tics
    UA_UInt64 callbackId = 1;
    UA_Server_addRepeatedCallback(server, callbackTicHeartBeat, NULL, HEART_BEAT_INTERVAL, &callbackId);

    UA_UInt32  defaultUInt32 = 0;
    UA_UInt32  couplerID = COUPLER_ID;
    UA_Float   defaultFloat = 0.0;
    const PublishedVariable publishedVariableArray[] = {
        // representing time in millis since start of process
        {
            .name = "heart_beat",
            .description = "Heartbeat",
            .pdefaultValue = &defaultFloat,
            .type = UA_TYPES_FLOAT
        },
        // representing the ID of the coupler
        //{
        //    .name = "id",
        //    .description = "ID",
        //    .pdefaultValue = &couplerID,
        //    .type = UA_TYPES_UINT32
        //}
    };

    UA_String transportProfile = UA_STRING(DEFAULT_TRANSPORT_PROFILE);
    UA_NetworkAddressUrlDataType networkAddressUrl =
        {UA_STRING_NULL , UA_STRING(NETWORK_ADDRESS_URL_DATA_TYPE)};
    addPubSubConnection(server, &transportProfile, &networkAddressUrl);
    addPublishedDataSet(server);
    for(i = 0; i < countof(publishedVariableArray); i++) {
        addPubSubVariable(server, publishedVariableArray[i]);
        addPubSubDataSetField(server, publishedVariableArray[i]);
    }
    addWriterGroup(server);
    addDataSetWriter(server);
}
