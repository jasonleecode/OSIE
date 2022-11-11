/* ================ Includes ===================== */
#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <open62541/plugin/log_stdout.h>

#include "../../coupler/opc-ua-server/mod_io_i2c.h"
#include "../../coupler/opc-ua-server/keep_alive.h"


/* ================ Function Tests =============== */

// ############# get I2C Slave List Length ##############

Test(keepalive, gotoSafeMode) {
    int result = 1;

    gotoSafeMode();

    cr_expect_eq(I2C_VIRTUAL_MODE, result);
}

// ############# Set Relay State (only virtual mode) ##############

Test(keepalive, gotoNormalMode) {
    int result = 0;

    gotoNormalMode();

    cr_expect_eq(I2C_VIRTUAL_MODE, result);
}