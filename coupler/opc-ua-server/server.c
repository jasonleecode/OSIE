/* 
 * OPC-UA server which controls MOD-IO's relays' state over OPC-UA protocol
 * Current format of exposed OPC-UA node variables is:
 * <mod-io_number>.<relay_number>
 * For example:
 *   - i2c0.relay0..3
 *   - i2c0.in0..3
 *   - i2c0.ain0..3
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
#include <argp.h>
#include <string.h>
#include "common.h"
#include "mod_io_i2c.h"
#include "mod_io_opc_ua.h"
#include "keep_alive.h"
#include <time.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/accesscontrol_default.h>
#include <open62541/plugin/pubsub_ethernet.h>
#include <open62541/plugin/pubsub_udp.h>

// global Id of coupler
static int COUPLER_ID = 0;

// global HEART BEATs of coupler
static int HEART_BEATS = 0;

// the heart beat interval
static int HEART_BEAT_INTERVAL = 250;

// global server
UA_Server *server;

// The default port of OPC-UA server
const int DEFAULT_OPC_UA_PORT = 4840;
const int DEFAULT_MODE = 0;

// CLI arguments handling
const char *argp_program_version = "OSIE OPC-UA coupler 0.0.1";
const char *argp_program_bug_address = "ivan@nexedi.com";
static char doc[] = "OPC-UA server which controls MOD-IO's relays' state over OPC-UA protocol.";
static char args_doc[] = "...";
static struct argp_option options[] = {
  {"port",        'p', "4840", 0, "Port to bind to."},
  {"device",      'd', "/dev/i2c-1", 0, "Linux block device path."},
  {"slave-address-list", 's', "0x58", 0, "Comma separated list of slave I2C addresses."},
  {"mode",        'm', "0", 0, "Set different modes of operation of coupler. Default (0) is set attached \
                  I2C's state state. Virtual (1) which does NOT set any I2C slaves' state."},
  {"username",    'u', "", 0, "Username."},
  {"password",    'w', "", 0, "Password."},
  {"key",         'k', "", 0, "x509 key."},
  {"certificate", 'c', "", 0, "X509 certificate."},
  {"id",          'i', "0", 0, "ID of coupler."},
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
    int id;
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
    case 'i':
      arguments->id = arg ? atoi (arg) : 0;
      break;
    case ARGP_KEY_ARG:
      return 0;
    default: 
      return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

static volatile UA_Boolean running = true;

static void stopHandler(int sign)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

void callbackTicHeartBeat()
{
    /* Increase periodically heart beats of the server */ 
    HEART_BEATS += 1;
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "heart_beat %d", HEART_BEATS);

    // set OPC UA's heat_beat node value
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "heart_beat");
    UA_Int32 myInteger = HEART_BEATS;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, myIntegerNodeId, myVar);
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
    arguments.id = 0;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    printf("Mode=%d\n", arguments.mode);
    printf("Listening port=%d\n", arguments.port);
    printf("Block device=%s\n", arguments.device);
    printf("Slave address list=%s\n", arguments.slave_address_list);
    printf("Key=%s\n", arguments.key);
    printf("Certificate=%s\n", arguments.certificate);
    printf("ID=%d\n", arguments.id);

    // transfer to global variables (CLI input)
    COUPLER_ID = arguments.id;
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

    bool addx509 = strlen(arguments.key) > 0 && strlen(arguments.certificate);
    bool addUserNamePasswordAuthentication = strlen(arguments.username) > 0 && strlen(arguments.password) > 0;

    //UA_Server *server = UA_Server_new();
    server = UA_Server_new();
    UA_ServerConfig_setMinimal(UA_Server_getConfig(server), arguments.port, NULL);
    UA_ServerConfig *config = UA_Server_getConfig(server);
    config->verifyRequestTimestamp = UA_RULEHANDLING_ACCEPT;

    // add variables representing physical relaray / inputs, etc
    addVariable(server);
    addValueCallbackToCurrentTimeVariable(server);

    /* Disable anonymous logins, enable two user/password logins */
    if (addUserNamePasswordAuthentication){
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
    if (addx509){
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

    // enable keep-alive

    // add a callback which will increment heart beat tics
    UA_UInt64 callbackId = 1;
    UA_Server_addRepeatedCallback(server, callbackTicHeartBeat, NULL, HEART_BEAT_INTERVAL, &callbackId);

    UA_Int32  defaultInt32 = 0; //XXX: use unsigned int
    UA_Int32  couplerID = COUPLER_ID; //XXX: use unsigned int
    const PublishedVariable publishedVariableArray[] = {
        // representing time in millis since start of process
        {
            .name = "heart_beat",
            .description = "Heart beat",
            .pdefaultValue = &defaultInt32,
            .type = UA_TYPES_INT32
        },
        // representing the ID of the coupler
        {
            .name = "id",
            .description = "ID",
            .pdefaultValue = &couplerID,
            .type = UA_TYPES_INT32
        }
    };

    UA_String transportProfile =
        UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
    UA_NetworkAddressUrlDataType networkAddressUrl =
        {UA_STRING_NULL , UA_STRING("opc.udp://224.0.0.22:4840/")};
    UA_ServerConfig_addPubSubTransportLayer(config, UA_PubSubTransportLayerUDPMP());
    addPubSubConnection(server, &transportProfile, &networkAddressUrl);
    addPublishedDataSet(server);
    for(i = 0; i < countof(publishedVariableArray); i++) {
        addPubSubVariable(server, publishedVariableArray[i]);
        addPubSubDataSetField(server, publishedVariableArray[i]);
    }
    addWriterGroup(server);
    addDataSetWriter(server);

    // run server
    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);

    // always leave attached slaves to a known safe shutdown state
    safeShutdownI2CSlaveList();

    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
