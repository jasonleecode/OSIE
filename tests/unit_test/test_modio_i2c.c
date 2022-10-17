/* ================ Includes ===================== */
#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>

#include "../../coupler/opc-ua-server/mod_io_i2c.h"

/* ================ Function Tests =============== */

// ############# get I2C Slave List Length ##############

Test(modioi2c, getI2CSlaveListLength) {
    int result, retval;

    retval = getI2CSlaveListLength();

    cr_expect_eq(retval, result);
}

// ############# Set Relay State (only virtual mode) ##############

Test(modioi2c, setRelayState) {
    int result, retval, command = 0x00, i2c_addr = 0x58;

    retval = setRelayState(command, i2c_addr);

    cr_expect_eq(retval, result);
}

// ############# Get Digital Input State (only virtual mode) ##############

Test(modioi2c, getDigitalInputState) {
    int result = 0, retval, i2c_addr = 0x58;
    char *syscall_str = NULL;

    retval = getDigitalInputState(i2c_addr, &syscall_str);

    cr_expect_eq(retval, result);
}

// ############# Get Analog Input State AIN (only virtual mode) ##############

Test(modioi2c, getAnalogInputStateAIN) {
    int result = 0, retval, i2c_addr = 0x58;
    int *syscall_int = NULL;
    uint8_t read_reg = 0x30;

    retval = getAnalogInputStateAIN(i2c_addr, &syscall_int, read_reg);

    cr_expect_eq(retval, result);
}

// ############# Safe Shutdown I2C Slave List ##############

Test(modioi2c, safeShutdownI2CSlaveList) {
    int result = 0, retval;
    
    safeShutdownI2CSlaveList();

    cr_expect_eq(retval, result);
}