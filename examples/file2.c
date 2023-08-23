#include "file2.h"

#include "minunit.h"
#include <stdbool.h>
#include <stdio.h>

MU_TEST(test_1) {
    ASSERT_EQ(false, true, "True and False are not the same");
}

MU_TEST(test_2) {
    char errBuf[200];
    int expected = 1234;
    int real = 1230;
    int maxAllowedAbsErr = 2;

    (void)sprintf(errBuf, "Expected = %u  -  Real = %u, maximal allowed error = %u\n", expected, real, maxAllowedAbsErr);
    ASSERT_NEAR_INT(expected, real, maxAllowedAbsErr, errBuf);
}

MU_TEST_SUITE(test_suite_file2) {
    MU_RUN_TEST(test_1);
    MU_RUN_TEST(test_2);
}
