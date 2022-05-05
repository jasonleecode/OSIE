/* 
 *  Command Line Interface handling.
 */
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
  {"heart-beat",  'b', "0", 0, "Publish heart beat to other couplers."},
  {"heart-beat-interval",  't', "500", 0, "Heart beat interval in ms."},
  {"heart-beat-id-list",  'l', "", 0, "Comma separated list of IDs of couplers to watch for heart beats. \
If heart beat is missing coupler goes to safe mode."},
  {"network-address-url-data-type", 'n', "opc.udp://224.0.0.22:4840/", 0, "Netwrok address URL type used for Pub/Sub."},

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
    bool heart_beat;
    int heart_beat_interval;
    char *heart_beat_id_list;
    char *network_address_url_data_type;
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
      arguments->id = arg ? atoi (arg) : DEFAULT_ID;
      break;
    case 'b':
      arguments->heart_beat = atoi (arg);
      break;
    case 't':
      arguments->heart_beat_interval = arg ? atoi (arg) : DEFAULT_HEART_BEAT_INTERVAL;
      break;
    case 'l':
      arguments->heart_beat_id_list = arg;
      break;
    case 'n':
      arguments->network_address_url_data_type = arg;
      break;
    case ARGP_KEY_ARG:
      return 0;
    default: 
      return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
