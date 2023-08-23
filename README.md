Forked from https://github.com/siu/minunit and modified to support tests in multiple `.c` files.

## MinUnit

Minunit is a minimal unit testing framework for C/C++

It provides a way to define and configure test suites and a few handy assertion types.
It reports the summary of the number of tests run, number of assertions and time elapsed.

## How to use it

```c
// file1.c
#include "file1.h"
#include "minunit.h"

MU_TEST(test_file1) {
    ASSERT_EQ_INT(5, 10);
    ASSERT_STREQ("Hello", "World!");
}

MU_TEST_SUITE(test_suite_file1) {
	MU_RUN_TEST(test_file1);
}

// file1.h
void test_suite_file1(void);

// file2.c
#include "file1.h"
#include "minunit.h"
#include <stdio.h>

MU_TEST(test_file2) {
    char errBuf[200];
    int expected = 1234;
    int real = 1230
    int maxAllowedAbsErr = 2;

    (void)sprintf(errBuf, "Expected = %u  -  Real = %u, maximal allowed error = %u\n", expected, real, maxAllowedAbsErr);
    ASSERT_NEAR_INT(expected, real, maxAllowedAbsErr, errBuf);
}

MU_TEST_SUITE(test_suite_file2) {
	MU_RUN_TEST(test_file2);
}

// file2.h
void test_suite_file2(void);

// main.c
#include "file1.h"
#include "file2.h"

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite_file1, "Test file 1");
	MU_RUN_SUITE(test_suite_file2, "Test file 2");

	MU_REPORT();

	return MU_EXIT_CODE;
}
```

Check out `example` folder to see a more detailed example. With terminal move to the `example`
folder and execute `make run`

Which should produce some thing like:

```
gcc -Wall -g -I.. -c file2.c -o file2.o
gcc -Wall -g -I.. -c main.c -o main.o
gcc -Wall -g -I.. -c file1.c -o file1.o
gcc -Wall -g -I.. -c ../minunit.c -o ../minunit.o
gcc  file2.o  main.o  file1.o  ../minunit.o -Wall -lrt -lm -o example
./example
Running "Test file 1" suite
.F
test_1 failed:
	file1.c:6: 5 not equal to 7
.F
test_2 failed:
	file1.c:10: 5 expected but was 10
..F
test_4 failed:
	file1.c:20: 'Hello' expected but was 'World!'

Running "Test file 2" suite
.F
test_1 failed:
	file2.c:8: True and False are not the same
.F
test_2 failed:
	file2.c:18: Expected = 1234  -  Real = 1230, maximal allowed error = 2




6 tests from 2 test suites ran, 5 failures

Finished in 0.00005476 seconds (real) 0.00005348 seconds (proc)

make: *** [Makefile:11: run] Error 5
```

## Setup and teardown functions

One can define setup and teardown functions and configure the test suite to run
them by using the macro `MU_SUITE_CONFIGURE` with within a `MU_TEST_SUITE`
declaration.

## Assertion types

- `mu_check(condition, inMsg)`
- `mu_assert(condition, inMsg)`
- `mu_fail(message)`
- `FAIL(message)`
- `ASSERT_EQ(inParam1, inParam2, inMsg)`
- `ASSERT_NE(inParam1, inParam2, inMsg)`
- `ASSERT_LE(inParam1, inParam2, inMsg)`
- `ASSERT_LT(inParam1, inParam2, inMsg)`
- `ASSERT_GE(inParam1, inParam2, inMsg)`
- `ASSERT_GT(inParam1, inParam2, inMsg)`
- `ASSERT_NEAR_INT(inParam1, inParam2, inAbsErr, inMsg)`
- `ASSERT_NEAR_DOUBLE(inParam1, inParam2, inAbsErr, inMsg)`
- `ASSERT_EQ_INT(inExpected, inResult)`
- `mu_assert_int_eq(expected, result)`
- `ASSERT_EQ_DOUBLE(inExpected, inResult)`
- `mu_assert_double_eq(expected, result)`
- `ASSERT_STREQ(inExpected, inResult)`
- `mu_assert_string_eq(expected, result)`
