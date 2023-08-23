#include "file1.h"
#include "file2.h"
#include "minunit.h"

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite_file1, "Test file 1");
    MU_RUN_SUITE(test_suite_file2, "Test file 2");

    MU_REPORT();

    return MU_EXIT_CODE;
}
