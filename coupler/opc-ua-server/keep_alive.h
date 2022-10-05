// OPC UA's Pub/Sub profile
char *DEFAULT_TRANSPORT_PROFILE = "http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp";
char *NETWORK_ADDRESS_URL_DATA_TYPE = "opc.udp://224.0.0.22:4840/";

// global HEART BEATs of coupler
static unsigned int HEART_BEATS = 0;

// the heart beat interval$$
const int DEFAULT_HEART_BEAT_INTERVAL = 50;
static int HEART_BEAT_INTERVAL = DEFAULT_HEART_BEAT_INTERVAL;

// the timeout in millis after which a coupler is considered down
const int DEFAULT_HEART_BEAT_TIMEOUT_INTERVAL = 4 * DEFAULT_HEART_BEAT_INTERVAL;
static int HEART_BEAT_TIMEOUT_INTERVAL = DEFAULT_HEART_BEAT_TIMEOUT_INTERVAL;

// the list of couplers onto which we depend for properly running$
// XXX: assume ONLY 8 couplers!
unsigned int HEART_BEAT_ID_LIST[] = {0, 0, 0, 0, 0, 0, 0, 0};

// the interval for publishing messages$
const int PUBLISHING_INTERVAL = 100;

// a hard coded writer group, data set and publisher ID$
// (should be same for publisher / subscriber)$
const int WRITER_GROUP_ID = 100;
const int DATASET_WRITER_ID = 62541;
const int PUBLISHER_ID = 2234;

void gotoSafeMode() {
  /*
   * In this mode coupler will shutdown all
   * relays of attached I2C slaves
   */
   if (!I2C_VIRTUAL_MODE) {
     UA_LOG_INFO(UA_Log_Stdout, \
                 UA_LOGCATEGORY_USERLAND, \
                 "Go to SAFE MODE");

    // do shutdown all attached I2C slaves (MOD-IO's relays).
    safeShutdownI2CSlaveList();
    // set to virtual mode which means that coupler will mimic working but
    // not set any related relays' state
    I2C_VIRTUAL_MODE = 1;
  }

}

void gotoNormalMode() {
  /*
   * In this mode coupler will set all
   * relays of attached I2C slaves.
   * This is the normal mode of operation (
   * unless changed over CLI with "-m" switch.)
   */
  UA_LOG_INFO(UA_Log_Stdout, \
              UA_LOGCATEGORY_USERLAND, \
              "Go to NORMAL MODE");
  I2C_VIRTUAL_MODE = 0;

}
