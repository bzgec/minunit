#include "file1.h"

#include "minunit.h"

MU_TEST(test_1) {
    mu_check(5 == 7, "5 not equal to 7");
}

MU_TEST(test_2) {
    ASSERT_EQ_INT(5, 10);

    ASSERT_STREQ("Hello", "World!");
}

MU_TEST(test_3) {
    ASSERT_STREQ("Equal str", "Equal str");
}

MU_TEST(test_4) {
    ASSERT_STREQ("Hello", "World!");
}

MU_TEST_SUITE(test_suite_file1) {
    MU_RUN_TEST(test_1);
    MU_RUN_TEST(test_2);
    MU_RUN_TEST(test_3);
    MU_RUN_TEST(test_4);
}
