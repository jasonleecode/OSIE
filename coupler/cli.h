/* 
 *  Command Line Interface handling.
 */
const char *argp_program_version = "OSIE OPC-UA coupler 0.0.1";
const char *argp_program_bug_address = "info@nexedi.com";
static char doc[] = "OPC-UA server which controls MOD-IO's relays' state over OPC-UA protocol.";
static char args_doc[] = "...";
static struct argp_option options[] = {
  {"port",                  'p', "4840",       0, "Port to bind to."},
  {"server-ip-address",     'a', "",           0, "[not yet available] Server address to bind to."},
  {"device",                'd', "/dev/i2c-1", 0, "Linux block device path."},
  {"slave-address-list",    's', "0x58",       0, "Comma separated list of slave I2C addresses."},
  {"mode",                  'm', "0",          0, "Set different modes of operation of coupler. Default (0) is set attached \
                                                   I2C's state state. Virtual (1) which does NOT set any I2C slaves' state."},
  {"username",              'u', "",           0, "Username."},
  {"password",              'w', "",           0, "Password."},
  {"key",                   'k', "",           0, "x509 key."},
  {"certificate",           'c', "",           0, "X509 certificate."},
  {"id",                    'i', "0",          0, "ID of coupler."},
  {"heart-beat",            'b', "0",          0, "Publish heart beat to other couplers."},
  {"heart-beat-interval",   't', "500",        0, "Heart beat interval in ms."},
  {"heart-beat-timeout-interval",
                            'o', "2000",       0, "Heart beat timeout interval in ms."},
  {"heart-beat-id-list",    'l', "",           0, "Comma separated list of IDs of couplers to watch for heart beats. \
                                                   If a heart beat is missing coupler goes to safe mode."},
  {"network-address-url-data-type",
                            'n', "opc.udp://224.0.0.22:4840/", 0, "Network address URL type used for Pub/Sub."},
  {"network-interface",     'j', "",           0, "Network interface to use for Pub/Sub."},
  {0}
};

struct arguments
{
    int mode;
    int port;
    char *server_ip_address;
    char *device;
    char *slave_address_list;
    char *username;
    char *password;
    char *key;
    char *certificate;
    int id;
    bool heart_beat;
    int heart_beat_interval;
    int heart_beat_timeout_interval;
    char *heart_beat_id_list;
    char *network_address_url_data_type;
    char *network_interface;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch (key) {
    case 'p':
      arguments->port = arg ? atoi (arg) : DEFAULT_OPC_UA_PORT;
      break;
    case 'a':
      arguments->server_ip_address = arg;
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
      arguments->id = arg ? atoi (arg) : DEFAULT_COUPLER_ID;
      break;
    case 'b':
      arguments->heart_beat = atoi (arg);
      break;
    case 't':
      arguments->heart_beat_interval = arg ? atoi (arg) : DEFAULT_HEART_BEAT_INTERVAL;
      break;
    case 'o':
      arguments->heart_beat_timeout_interval = arg ? atoi (arg) : DEFAULT_HEART_BEAT_TIMEOUT_INTERVAL;
      break;
    case 'l':
      arguments->heart_beat_id_list = arg;
      break;
    case 'n':
      arguments->network_address_url_data_type = arg;
      break;
    case 'j':
      arguments->network_interface = arg;
      break;
    case ARGP_KEY_ARG:
      return 0;
    default: 
      return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

void handleCLI(int argc, char **argv) {
    // parse CLI
    int i;
    int length;
    long result;
    char *eptr;


    static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

    struct arguments arguments;
    arguments.port = DEFAULT_OPC_UA_PORT;
    arguments.server_ip_address = "";
    arguments.mode = DEFAULT_MODE;
    arguments.device = DEFAULT_I2C_BLOCK_DEVICE_NAME;
    arguments.slave_address_list = DEFAULT_I2C_0_ADDR;
    arguments.username = "";
    arguments.password = "";
    arguments.key = "";
    arguments.certificate = "";
    arguments.id = DEFAULT_COUPLER_ID;
    arguments.heart_beat_interval = DEFAULT_HEART_BEAT_INTERVAL;
    arguments.heart_beat_timeout_interval = DEFAULT_HEART_BEAT_TIMEOUT_INTERVAL;
    arguments.heart_beat_id_list = "";
    arguments.network_address_url_data_type = NETWORK_ADDRESS_URL_DATA_TYPE;
    arguments.network_interface = "";
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    printf("Mode=%d\n", arguments.mode);
    printf("Listening port=%d\n", arguments.port);
    printf("server_ip_address=%s\n", arguments.server_ip_address);
    printf("Block device=%s\n", arguments.device);
    printf("Slave address list=%s\n", arguments.slave_address_list);
    printf("Key=%s\n", arguments.key);
    printf("Certificate=%s\n", arguments.certificate);
    printf("ID=%d\n", arguments.id);
    printf("Heart beat=%d\n", arguments.heart_beat);
    printf("Heart beat interval=%d ms\n", arguments.heart_beat_interval);
    printf("Heart beat timeout interval=%d ms\n", arguments.heart_beat_timeout_interval);
    printf("Heart beat ID list=%s\n", arguments.heart_beat_id_list);
    printf("Network address URL data type=%s\n", arguments.network_address_url_data_type);
    printf("Network interface=%s\n", arguments.network_interface);

    // transfer to global variables (CLI input)
    COUPLER_ID = arguments.id;
    I2C_VIRTUAL_MODE = arguments.mode;
    I2C_BLOCK_DEVICE_NAME = arguments.device;
    HEART_BEAT_INTERVAL = arguments.heart_beat_interval;
    PUBLISHING_INTERVAL = HEART_BEAT_INTERVAL; // we assume that each heart_beat leads to a publish event
    HEART_BEAT_TIMEOUT_INTERVAL = arguments.heart_beat_timeout_interval;
    NETWORK_ADDRESS_URL_DATA_TYPE = arguments.network_address_url_data_type;
    NETWORK_INTERFACE = arguments.network_interface;
    USERNAME = arguments.username;
    PASSWORD = arguments.password;
    OPC_UA_PORT = arguments.port;
    OPC_UA_ADDRESS = arguments.server_ip_address;
    ENABLE_X509 = strlen(arguments.key) > 0 && strlen(arguments.certificate);
    ENABLE_USERNAME_PASSWORD_AUTHENTICATION = strlen(arguments.username) > 0 && strlen(arguments.password) > 0;
    ENABLE_HEART_BEAT = arguments.heart_beat;
    X509_KEY_FILENAME = arguments.key;
    X509_CERTIFICATE_FILENAME = arguments.certificate;
    printf("PUBLISHING_INTERVAL=%d\n", PUBLISHING_INTERVAL);

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

    // convert arguments.heart_beat_id_list -> HEART_BEAT_ID_LIST
    i = 0;
    char *tk= strtok(arguments.heart_beat_id_list, ",");
    while (tk != NULL)
    {
      // from CLI we get a  comma separated list on INTs representing coupler' ID
      result = strtol(tk, &eptr, 16);
      HEART_BEAT_ID_LIST[i++] = result;
      // enable heart beat checks
      ENABLE_HEART_BEAT_CHECK = true;
      tk = strtok(NULL, ",");
      }

    printf("Heart beat check=%d\n", ENABLE_HEART_BEAT_CHECK);

    // update current measurement mode
    const char* s = getenv("CURRENT_GPIO_MODE");
    if (s != NULL) CURRENT_GPIO_MODE = 1;
    printf("GPIO measurement mode = %d\n", CURRENT_GPIO_MODE);
}


