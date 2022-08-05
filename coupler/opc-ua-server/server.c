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
#include <time.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/accesscontrol_default.h>
#include <open62541/plugin/pubsub_ethernet.h>
#include <open62541/plugin/pubsub_udp.h>

// global ID of coupler
static int COUPLER_ID = 0;

// global server
UA_Server *server;

// global dictionary of subscribers
dict_t *SUBSCRIBER_DICT;

// The default port of OPC-UA server
const int DEFAULT_OPC_UA_PORT = 4840;
const int DEFAULT_MODE = 0;
const int DEFAULT_COUPLER_ID = 0;

int OPC_UA_PORT;
bool ENABLE_HEART_BEAT = false;
bool ENABLE_HEART_BEAT_CHECK = false;
bool ENABLE_X509 = false;
bool ENABLE_USERNAME_PASSWORD_AUTHENTICATION = false;
char *USERNAME;
char *PASSWORD;
char *X509_KEY_FILENAME;
char *X509_CERTIFICATE_FILENAME;

#include "keep_alive_publisher.h"
#include "keep_alive_subscriber.h"
#include "cli.h"

static volatile UA_Boolean running = true;

static void stopHandler(int sign)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}


int main(int argc, char **argv)
{
    // init dictionary only once$
    if (SUBSCRIBER_DICT==NULL){
      SUBSCRIBER_DICT = *dictAlloc();
    }

    // parse CLI
    handleCLI(argc, argv);

    // always start attached slaves from a know safe shutdown state
    safeShutdownI2CSlaveList();

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    server = UA_Server_new();
    UA_ServerConfig_setMinimal(UA_Server_getConfig(server), OPC_UA_PORT, NULL);
    UA_ServerConfig *config = UA_Server_getConfig(server);
    config->verifyRequestTimestamp = UA_RULEHANDLING_ACCEPT;

    // add variables representing physical relays / inputs, etc
    addVariable(server);
    addValueCallbackToCurrentTimeVariable(server);

    /* Disable anonymous logins, enable two user/password logins */
    if (ENABLE_USERNAME_PASSWORD_AUTHENTICATION){
      UA_UsernamePasswordLogin logins[1] = {
          {UA_STRING(USERNAME), UA_STRING(PASSWORD)},
      };

      config->accessControl.clear(&config->accessControl);
      UA_StatusCode retval1 = UA_AccessControl_default(config, false, NULL,
               &config->securityPolicies[config->securityPoliciesSize-1].policyUri, 1, logins);
    }

    /* Enable x509 */
    #ifdef UA_ENABLE_ENCRYPTION
    if (ENABLE_X509){
      /* Load certificate and private key */
      UA_ByteString certificate = loadFile(X509_CERTIFICATE_FILENAME);
      UA_ByteString privateKey  = loadFile(X509_KEY_FILENAME);

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

    // enable protocol for Pub/Sub
    UA_ServerConfig_addPubSubTransportLayer(config, UA_PubSubTransportLayerUDPMP());
 
    // enable publish keep-alive messages
    if (ENABLE_HEART_BEAT) {
      enablePublishHeartBeat(server, config);
    }

    // enable subscribe to keep-alive messages
    enableSubscribeToHeartBeat(server, config);

    // run server
    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);

    // always leave attached slaves to a known safe shutdown state
    safeShutdownI2CSlaveList();

    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
