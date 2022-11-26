/*
 * Copyright (c) 2012 David Siñuela Pastor, siu.4coders@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef MINUNIT_MINUNIT_H
#define MINUNIT_MINUNIT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER < 1900
#define __func__ __FUNCTION__  // cppcheck-suppress[misra-c2012-21.1]
#endif

#elif defined(__unix__) || defined(__unix) || defined(unix) \
        || (defined(__APPLE__) && defined(__MACH__))

#if __GNUC__ >= 5 && !defined(__STDC_VERSION__)
#define __func__ __extension__ __FUNCTION__  // cppcheck-suppress[misra-c2012-21.1]
#endif

#else
//#error "Unable to define timers for an unknown OS."
#endif

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

/*  Maximum length of last message */
#define MINUNIT_MESSAGE_LEN 1024u

typedef enum MU_testStatus_ENUM {
    MU_testStatus_OK = 0u,
    MU_testStatus_FAIL = 1u,
} MU_testStatus_E;

typedef void (*MINUNIT_test_fn_T)(void);
typedef void (*MINUNIT_test_suite_fn_T)(void);
typedef void (*MINUNIT_setup_fn_T)(void);
typedef void (*MINUNIT_teardown_fn_T)(void);

void MINUNIT_run_suite(MINUNIT_test_suite_fn_T inTestSuiteFn, const char *inSuiteName);
void MINUNIT_suite_configure(MINUNIT_setup_fn_T inSetupFn, MINUNIT_teardown_fn_T inTeardownFn);
void MINUNIT_run_test(MINUNIT_test_fn_T inTestFn);
void MINUNIT_report(void);
uint32_t MINUNIT_getNumbOfFailedTests(void);
void MINUNIT_failAndPrintMsg(const char *inFuncName,
                             const char *inFileName,
                             int inLine,
                             const char *inMsg);
MU_testStatus_E MINUNIT_assert_streq(const char *inExpected, const char *inResult);
MU_testStatus_E MINUNIT_assert_int(int inExpected, int inResult);
MU_testStatus_E MINUNIT_assert_double(double inExpected, double inResult);

/*  Last message */
extern char minunit_last_message_str[MINUNIT_MESSAGE_LEN];

/*  Definitions */
#define MU_TEST(method_name) static void method_name(void)  // cppcheck-suppress[misra-c2012-20.7]
#define MU_TEST_SUITE(suite_name) void suite_name(void)     // cppcheck-suppress[misra-c2012-20.7]

// cppcheck-suppress[misra-c2012-20.7]
#define MU__SAFE_BLOCK(block) \
    do {                      \
        block                 \
    } while(0)

/*  Run test suite and unset setup and teardown functions */
#define MU_RUN_SUITE(inTestSuiteFn, inSuiteName) MINUNIT_run_suite(&(inTestSuiteFn), (inSuiteName))

/*  Configure setup and teardown functions */
#define MU_SUITE_CONFIGURE(inSetupFn, inTeardownFn) \
    MINUNIT_suite_configure((inSetupFn), (inTeardownFn))

/*  Test runner */
#define MU_RUN_TEST(inTestFn) MINUNIT_run_test(&(inTestFn))

/*  Report */
#define MU_REPORT() MINUNIT_report()

#define MU_EXIT_CODE MINUNIT_getNumbOfFailedTests()

/*  Assertions */
#define mu_check(test)                                                  \
    MU__SAFE_BLOCK(if(!(test)) {                                        \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define mu_assert(test, inMsg)                                          \
    MU__SAFE_BLOCK(if(!(test)) {                                        \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define FAIL(inMsg) \
    MU__SAFE_BLOCK(MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); return;)

#define mu_fail(message) FAIL((message))

#define ASSERT_EQ(inParam1, inParam2, inMsg)                            \
    MU__SAFE_BLOCK(if(!((inParam1) == (inParam2))) {                    \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define ASSERT_NE(inParam1, inParam2, inMsg)                            \
    MU__SAFE_BLOCK(if(!((inParam1) != (inParam2))) {                    \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define ASSERT_LE(inParam1, inParam2, inMsg)                            \
    MU__SAFE_BLOCK(if(!((inParam1) <= (inParam2))) {                    \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define ASSERT_LT(inParam1, inParam2, inMsg)                            \
    MU__SAFE_BLOCK(if(!((inParam1) < (inParam2))) {                     \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define ASSERT_GE(inParam1, inParam2, inMsg)                            \
    MU__SAFE_BLOCK(if(!((inParam1) >= (inParam2))) {                    \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define ASSERT_GT(inParam1, inParam2, inMsg)                            \
    MU__SAFE_BLOCK(if(!((inParam1) > (inParam2))) {                     \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

// Verifies that the difference between inParam1 and inParam2 does not exceed the absolute error
// bound inAbsErr.
#define ASSERT_NEAR_INT(inParam1, inParam2, inAbsErr, inMsg)            \
    MU__SAFE_BLOCK(if(!((inAbsErr) > abs((inParam1) - (inParam2)))) {   \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define ASSERT_NEAR_DOUBLE(inParam1, inParam2, inAbsErr, inMsg)         \
    MU__SAFE_BLOCK(if(!((inAbsErr) > fabs((inParam1) - (inParam2)))) {  \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (inMsg)); \
        return;                                                         \
    })

#define ASSERT_EQ_INT(inExpected, inResult)                                                \
    MU__SAFE_BLOCK(if(MINUNIT_assert_int((inExpected), (inResult)) != MU_testStatus_OK) {  \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (minunit_last_message_str)); \
        return;                                                                            \
    })

#define mu_assert_int_eq(expected, result) ASSERT_EQ_INT((expected), (result))

#define ASSERT_EQ_DOUBLE(inExpected, inResult)                                               \
    MU__SAFE_BLOCK(if(MINUNIT_assert_double((inExpected), (inResult)) != MU_testStatus_OK) { \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (minunit_last_message_str));   \
        return;                                                                              \
    })

#define mu_assert_double_eq(expected, result) ASSERT_EQ_DOUBLE(expected, result)

#define ASSERT_STREQ(inExpected, inResult)                                                  \
    MU__SAFE_BLOCK(if(MINUNIT_assert_streq((inExpected), (inResult)) != MU_testStatus_OK) { \
        MINUNIT_failAndPrintMsg(__func__, __FILE__, __LINE__, (minunit_last_message_str));  \
        return;                                                                             \
    })

#define mu_assert_string_eq(expected, result) ASSERT_STREQ((expected), (result))

/*
 * The following two functions were written by David Robert Nadeau
 * from http://NadeauSoftware.com/ and distributed under the
 * Creative Commons Attribution 3.0 Unported License
 */

/**
 * Returns the real time, in seconds, or -1.0 if an error occurred.
 *
 * Time is measured since an arbitrary and OS-dependent start time.
 * The returned real time is only useful for computing an elapsed time
 * between two calls to this function.
 */
double mu_timer_real(void);

/**
 * Returns the amount of CPU time used by the current process,
 * in seconds, or -1.0 if an error occurred.
 */
double mu_timer_cpu(void);

#ifdef __cplusplus
}
#endif

#endif /* MINUNIT_MINUNIT_H */
