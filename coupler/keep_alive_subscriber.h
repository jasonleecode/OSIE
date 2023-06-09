#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/pubsub_udp.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/types_generated.h>
#include "ua_pubsub.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

UA_NodeId connectionIdentifier;
UA_NodeId readerGroupIdentifier;
UA_NodeId readerIdentifier;
UA_DataSetReaderConfig readerConfig;

static void fillTestDataSetMetaData(UA_DataSetMetaDataType *pMetaData);

/* callback to handle change notifications */
static void dataChangeNotificationCallback(UA_Server *server, UA_UInt32 monitoredItemId,
                               void *monitoredItemContext, const UA_NodeId *nodeId,
                               void *nodeContext, UA_UInt32 attributeId,
                               const UA_DataValue *var) {
    unsigned long int milli_seconds_now;
    unsigned int coupler_id;

    // filter out heart_beat from Data Set
    if(UA_Variant_hasScalarType(&var->value, &UA_TYPES[UA_TYPES_FLOAT])) {
        float heart_beat = *(UA_Float*) var->value.data;
        milli_seconds_now = getMilliSecondsSinceEpoch();
        // split <ID>.<heart_beats>, just converting to int is enough
        coupler_id = (int) heart_beat;
        if (coupler_id!=COUPLER_ID) {
          //UA_LOG_INFO(UA_Log_Stdout, \
          //           UA_LOGCATEGORY_USERLAND, \
          //           "HEART BEAT: %d (%ld)", coupler_id, milli_seconds_now);

	  // convert coupler_id to str
          char* coupler_id_str = convertInt2Str(coupler_id);

          // convert micro seconds to str
          char* milli_seconds_now_str = convertLongInt2Str(milli_seconds_now);

          // Add to our local linked list
          addItem(&SUBSCRIBER_DICT, coupler_id_str, milli_seconds_now_str);

	  // set GPIO so we can monitor using logical analyzer the work of
	  // keep-alive network system
	  if (CURRENT_GPIO_MODE == 1) setGPIO();

	}
    }
}

/* Add new connection to the server */
static UA_StatusCode addPubSubConnectionSubscriber(UA_Server *server, UA_String *transportProfile,
                    UA_NetworkAddressUrlDataType *networkAddressUrl) {
    if((server == NULL) || (transportProfile == NULL) ||
        (networkAddressUrl == NULL)) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    /* Configuration creation for the connection */
    UA_PubSubConnectionConfig connectionConfig;
    memset (&connectionConfig, 0, sizeof(UA_PubSubConnectionConfig));
    connectionConfig.name = UA_STRING("UDPMC Connection 1");
    connectionConfig.transportProfileUri = *transportProfile;
    connectionConfig.enabled = UA_TRUE;
    if (strlen(NETWORK_INTERFACE) > 0){
        // set preferred network interface for Pub / Sub
        networkAddressUrl->networkInterface = UA_STRING(NETWORK_INTERFACE);
    }
    UA_Variant_setScalar(&connectionConfig.address, networkAddressUrl,
                         &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
    connectionConfig.publisherId.numeric = UA_UInt32_random ();
    retval |= UA_Server_addPubSubConnection (server, &connectionConfig, &connectionIdentifier);
    if (retval != UA_STATUSCODE_GOOD) {
        return retval;
    }

    return retval;
}

static UA_StatusCode addReaderGroup(UA_Server *server) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_ReaderGroupConfig readerGroupConfig;
    memset (&readerGroupConfig, 0, sizeof(UA_ReaderGroupConfig));
    readerGroupConfig.name = UA_STRING("ReaderGroup1");
    retval |= UA_Server_addReaderGroup(server, connectionIdentifier, &readerGroupConfig,
                                       &readerGroupIdentifier);
    UA_Server_setReaderGroupOperational(server, readerGroupIdentifier);
    return retval;
}

static UA_StatusCode addDataSetReader(UA_Server *server) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    memset (&readerConfig, 0, sizeof(UA_DataSetReaderConfig));
    readerConfig.name = UA_STRING("DataSet Reader 1");
    UA_UInt16 publisherIdentifier = PUBLISHER_ID;
    readerConfig.publisherId.type = &UA_TYPES[UA_TYPES_UINT16];
    readerConfig.publisherId.data = &publisherIdentifier;
    readerConfig.writerGroupId    = WRITER_GROUP_ID;
    readerConfig.dataSetWriterId  = DATASET_WRITER_ID;

    /* Setting up Meta data configuration in DataSetReader */
    fillTestDataSetMetaData(&readerConfig.dataSetMetaData);

    retval |= UA_Server_addDataSetReader(server, readerGroupIdentifier, &readerConfig,
                                         &readerIdentifier);
    return retval;
}

static UA_StatusCode addSubscribedVariables(UA_Server *server, UA_NodeId dataSetReaderId) {
    if(server == NULL)
        return UA_STATUSCODE_BADINTERNALERROR;

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_NodeId folderId;
    UA_String folderName = readerConfig.dataSetMetaData.name;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    UA_QualifiedName folderBrowseName;
    if(folderName.length > 0) {
        oAttr.displayName.locale = UA_STRING ("en-US");
        oAttr.displayName.text = folderName;
        folderBrowseName.namespaceIndex = 1;
        folderBrowseName.name = folderName;
    }
    else {
        oAttr.displayName = UA_LOCALIZEDTEXT ("en-US", "Subscribed Variables");
        folderBrowseName = UA_QUALIFIEDNAME (1, "Subscribed Variables");
    }

    UA_Server_addObjectNode (server, UA_NODEID_NULL,
                             UA_NODEID_NUMERIC (0, UA_NS0ID_OBJECTSFOLDER),
                             UA_NODEID_NUMERIC (0, UA_NS0ID_ORGANIZES),
                             folderBrowseName, UA_NODEID_NUMERIC (0,
                             UA_NS0ID_BASEOBJECTTYPE), oAttr, NULL, &folderId);

    /* Create the TargetVariables with respect to DataSetMetaData fields */
    UA_FieldTargetVariable *targetVars = (UA_FieldTargetVariable *)
            UA_calloc(readerConfig.dataSetMetaData.fieldsSize, sizeof(UA_FieldTargetVariable));
    for(size_t i = 0; i < readerConfig.dataSetMetaData.fieldsSize; i++) {
        /* Variable to subscribe data */
        UA_VariableAttributes vAttr = UA_VariableAttributes_default;
        UA_LocalizedText_copy(&readerConfig.dataSetMetaData.fields[i].description,
                              &vAttr.description);
        vAttr.displayName.locale = UA_STRING("en-US");
        vAttr.displayName.text = readerConfig.dataSetMetaData.fields[i].name;
        vAttr.dataType = readerConfig.dataSetMetaData.fields[i].dataType;

        UA_NodeId newNode;
        retval |= UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, (UA_UInt32)i + 50000),
                                           folderId,
                                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                           UA_QUALIFIEDNAME(1, (char *)readerConfig.dataSetMetaData.fields[i].name.data),
                                           UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                           vAttr, NULL, &newNode);
        /*monitor variable*/
        if (ENABLE_HEART_BEAT_CHECK) {
          UA_MonitoredItemCreateRequest monRequest = UA_MonitoredItemCreateRequest_default(newNode);
          monRequest.requestedParameters.samplingInterval = PUBLISHING_INTERVAL;
          UA_Server_createDataChangeMonitoredItem(server, UA_TIMESTAMPSTORETURN_NEITHER,
                                                  monRequest, NULL, dataChangeNotificationCallback);
        }
        /* For creating Targetvariables */
        UA_FieldTargetDataType_init(&targetVars[i].targetVariable);
        targetVars[i].targetVariable.attributeId  = UA_ATTRIBUTEID_VALUE;
        targetVars[i].targetVariable.targetNodeId = newNode;
    }

    retval = UA_Server_DataSetReader_createTargetVariables(server, dataSetReaderId,
                                                           readerConfig.dataSetMetaData.fieldsSize, targetVars);
    for(size_t i = 0; i < readerConfig.dataSetMetaData.fieldsSize; i++)
        UA_FieldTargetDataType_clear(&targetVars[i].targetVariable);

    UA_free(targetVars);
    UA_free(readerConfig.dataSetMetaData.fields);
    return retval;
}

static void fillTestDataSetMetaData(UA_DataSetMetaDataType *pMetaData) {
    if(pMetaData == NULL) {
        return;
    }

    UA_DataSetMetaDataType_init (pMetaData);
    pMetaData->name = UA_STRING ("DataSet 1 (subscribed)");
    pMetaData->fieldsSize = 1;
    pMetaData->fields = (UA_FieldMetaData*)UA_Array_new (pMetaData->fieldsSize,
                         &UA_TYPES[UA_TYPES_FIELDMETADATA]);

    /* heartbeat */
    UA_FieldMetaData_init (&pMetaData->fields[0]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_FLOAT].typeId,
                    &pMetaData->fields[0].dataType);
    pMetaData->fields[0].builtInType = UA_NS0ID_FLOAT;
    pMetaData->fields[0].name =  UA_STRING ("Heartbeat (subscribed)");
    pMetaData->fields[0].valueRank = -1; /* scalar */

}


void callbackCheckHeartBeat() {
  /*
   * Check if for liveness of related couplers. Called upon a certain interval.
   * If a related coupler is down got to safe mode.
   */
  int i, coupler_id, last_seen_timestamp_int, timestamp_delta;
  bool is_down;
  unsigned long int milli_seconds = getMilliSecondsSinceEpoch();
  size_t n = sizeof(HEART_BEAT_ID_LIST)/sizeof(HEART_BEAT_ID_LIST[0]);
  for (int i = 0; i < n; i++) {
    coupler_id = HEART_BEAT_ID_LIST[i];
    if (coupler_id > 0) {
      // convert to str as this is the hash key
      char* coupler_id_str = convertInt2Str(coupler_id);
      char *last_seen_timestamp = getItem(SUBSCRIBER_DICT, coupler_id_str);
      //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Check ID=%s, last_seen=%s", coupler_id_str, last_seen_timestamp);
      if (last_seen_timestamp != NULL){
        // we do have timestamp for this coupler ID
        last_seen_timestamp_int = atoi(last_seen_timestamp);
        timestamp_delta = milli_seconds - last_seen_timestamp_int;
        is_down = (timestamp_delta > HEART_BEAT_TIMEOUT_INTERVAL);
        if (is_down) {
          // count for stats the switch to SAFE mode
          if (CURRENT_STATE != STATE_DOWN) {
            CURRENT_STATE = STATE_DOWN;
            SAFE_MODE_STATE_COUNTER += 1;
            UA_LOG_INFO(UA_Log_Stdout, \
                        UA_LOGCATEGORY_USERLAND, \
                        "DOWN: %s (delta=%d)", coupler_id_str, timestamp_delta);
            // go to safe mode as a dependant coupler is DOWN.
            gotoSafeMode();
          }
        }
        else {
          // all good, we received a keep alive in time
          if (CURRENT_STATE == STATE_NO_INITIAL_HEART_BEAT) {
            // initial keep alive received, printout
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "INITIAL HEART BEAT received: %s", coupler_id_str);
          }
          else if (CURRENT_STATE == STATE_DOWN) {
            // initial keep alive received, printout
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                        "UP (recovered %d times): %s", SAFE_MODE_STATE_COUNTER, coupler_id_str);
	    // go to normal operational mode
	    gotoNormalMode();
          }
          CURRENT_STATE = STATE_UP;
        }
      }
      else {
        // still no hear beat from this coupler ...
        if (CURRENT_STATE != STATE_NO_INITIAL_HEART_BEAT){
          CURRENT_STATE = STATE_NO_INITIAL_HEART_BEAT;
          UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "NO INITIAL HEART BEAT: %s", coupler_id_str);
        }
      }
    }
  }
}


static int enableSubscribeToHeartBeat(UA_Server *server, UA_ServerConfig *config){
    // enable subscribe to keep-alive messages
    UA_String transportProfile = UA_STRING(DEFAULT_TRANSPORT_PROFILE);
    UA_NetworkAddressUrlDataType networkAddressUrl = {UA_STRING_NULL , UA_STRING(NETWORK_ADDRESS_URL_DATA_TYPE)};
    addPubSubConnectionSubscriber(server, &transportProfile, &networkAddressUrl);

    /* Add ReaderGroup to the created PubSubConnection */
    addReaderGroup(server);

    /* Add DataSetReader to the created ReaderGroup */
    addDataSetReader(server);

    /* Add SubscribedVariables to the created DataSetReader */
    addSubscribedVariables(server, readerIdentifier);

   // add a callback which will check related coupler's heart beats
   UA_UInt64 callbackId = 2;
   UA_Server_addRepeatedCallback(server, callbackCheckHeartBeat, NULL, HEART_BEAT_INTERVAL, &callbackId);
}
