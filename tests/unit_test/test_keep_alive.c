/* ================ Includes ===================== */
#include <criterion/criterion.h>
#include <linux/i2c-dev.h>
#include <open62541/plugin/log_stdout.h>

#include "../../coupler/opc-ua-server/mod_io_i2c.h"
#include "../../coupler/opc-ua-server/keep_alive.h"


/* ================ Function Tests =============== */

// ############# test safe mode ##############

Test(keepalive, gotoSafeMode) {
    int result = 1;

    gotoSafeMode();

    cr_expect_eq(I2C_VIRTUAL_MODE, result);
}

// ############# test normal mode ##############

Test(keepalive, gotoNormalMode) {
    int result = 0;

    gotoNormalMode();

    cr_expect_eq(I2C_VIRTUAL_MODE, result);
}