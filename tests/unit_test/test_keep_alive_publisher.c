/* ================ Includes ===================== */
#include <criterion/criterion.h>
#include <linux/i2c-dev.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>


// #include "../../coupler/opc-ua-server/server.c"
// #include "../../coupler/opc-ua-server/common.h"
// #include "../../coupler/opc-ua-server/mod_io_i2c.h"
// #include "../../coupler/opc-ua-server/keep_alive_publisher.h"

/* ================ Function Tests =============== */

// ############# check heart beats of the server ##############

// Test(keepalivepublisher, callbackTicHeartBeat) {
//     int result = 1;

//     callbackTicHeartBeat();

//     cr_expect_geq(HEART_BEATS, result);
// }
