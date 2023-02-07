// global relay state
uint8_t I2C_0_RELAYS_STATE = 0; // state of 4 relays at I2C slave 0
uint8_t I2C_1_RELAYS_STATE = 0; // state of 4 relays at I2C slave 1

// the default addresses of MOD-IOs
static char *DEFAULT_I2C_0_ADDR = "0x58";

// the list of attached I2C slaves
const int DEFAULT_I2C_SLAVE_ADDR = 0x58;

// XXX:code assumes only 8 I2C slaves but it can be more
int I2C_SLAVE_ADDR_LIST[] = {0, 0, 0, 0, 0, 0, 0, 0};

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
