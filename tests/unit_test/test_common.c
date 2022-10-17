/* ================ Includes ===================== */
#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "../../coupler/opc-ua-server/common.h"

/* ================ Function Tests =============== */

// ############# get milliseconds since epoch ##############

Test(common, getMilliSecondsSinceEpoch) {
    unsigned long int ms, retval;
    struct timeval current_time;

    gettimeofday(&current_time, NULL);
    ms = current_time.tv_sec * 1000 + current_time.tv_usec / 1000;
    retval = getMilliSecondsSinceEpoch();

    cr_expect_eq(retval, ms);
}

// ############# load File parses the certificate file ##############

// ############# Generate a random String ##############

Test(common, randomString) {
    char *result = "PKdht";
    char *retval = randomString(5);

    cr_expect_str_eq(retval, result);
}

// ############# Convert integer to String ##############

Test(common, convertInt2Str) {
    char *result = "5";
    char *retval = convertInt2Str(5);

    cr_expect_str_eq(retval, result);
}

// ############# Convert a long integer to String ##############

Test(common, convertLongInt2Str) {
    char *result = "5";
    char *retval = convertLongInt2Str(5);

    cr_expect_str_eq(retval, result);
}

// ############# A key/value dictionary system ##############

Test(common, dict) {
        /* Create a dict */
    dict_t **dict = dictAlloc();
    char *result = "bar";
    
    /* lets add foo to the dict */
    addItem(dict, "foo", "bar");
    
    /* and print their values */
    char *retval = (char *)getItem(*dict, "foo");
    cr_expect_str_eq(retval, result);
    
    /* lets delete them */
    delItem(dict, "foo");
    
    /* see, their gone, there NULL */
    retval = (char *)getItem(*dict, "foo");
    cr_expect_null(retval);
    
    /* add them again to proof it works */
    addItem(dict, "foo", "bar");
    
    /* see, here */
    retval = (char *)getItem(*dict, "foo");
    cr_expect_str_eq(retval, result);

    delItem(dict, "foo");
    
    dictDealloc(dict);

}