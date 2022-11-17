/* ================ Includes ===================== */
#define DOING_UNIT_TESTS
#include <criterion/criterion.h>
#include "../../coupler/opc-ua-server/server.c"


/* ================ Function Tests =============== */

// ############# check heart beats of the server ##############

Test(keepalivepublisher, callbackTicHeartBeat) {
    int result = 1;

    server = UA_Server_new();
    callbackTicHeartBeat();
    UA_Server_delete(server);
    
    cr_expect_geq(HEART_BEATS, result);
}
