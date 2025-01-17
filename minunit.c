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
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#include <Windows.h>
#if defined(_MSC_VER) && _MSC_VER < 1900
#define(void) snprintf _(void) snprintf
#endif

#elif defined(__unix__) || defined(__unix) || defined(unix) \
        || (defined(__APPLE__) && defined(__MACH__))

/* Change POSIX C SOURCE version for pure c99 compilers */
#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L
#undef _POSIX_C_SOURCE           // cppcheck-suppress[misra-c2012-20.5, misra-c2012-21.1]
#define _POSIX_C_SOURCE 200112L  // cppcheck-suppress[misra-c2012-21.1]
#endif

#include <sys/resource.h>
#include <sys/time.h>  // gethrtime(), gettimeofday()
#include <sys/times.h>
#include <time.h> /* clock_gettime(), time()*/  // cppcheck-suppress[misra-c2012-21.10]
#include <unistd.h>                             // POSIX flags

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#else
#error "Unable to define timers for an unknown OS."
#endif

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>  // cppcheck-suppress[misra-c2012-21.6]
#include <string.h>

#include "minunit.h"

/*  Accuracy with which floats are compared */
#define MINUNIT_EPSILON 1E-12

/*  Misc. counters */
static uint32_t minunit_cntTests = 0u;
static uint32_t minunit_cntSuites = 0u;
static uint32_t minunit_cntFailures = 0u;
static MU_testStatus_E minunit_testStatus = MU_testStatus_OK;

/*  Timers */
static double minunit_real_timer_start = 0.0;
static double minunit_proc_timer_start = 0.0;

/*  Last message */
static char minunit_last_message[MINUNIT_MESSAGE_LEN];
char minunit_last_message_str[MINUNIT_MESSAGE_LEN];

/*  Test setup and teardown function pointers */
static MINUNIT_setup_fn_T minunit_setup = NULL;
static MINUNIT_setup_fn_T minunit_teardown = NULL;

static void MINUNIT_fail_test(void) {
    minunit_cntFailures++;
    (void)printf("F");
    (void)printf("\n%s\n", minunit_last_message);
}

void MINUNIT_run_suite(MINUNIT_test_suite_fn_T inTestSuiteFn, const char *inSuiteName) {
    if(inSuiteName != NULL) {
        (void)printf("Running \"%s\" suite\n", (const char *)inSuiteName);
        (void)fflush(stdout);
    };

    minunit_cntSuites++;

    inTestSuiteFn();

    minunit_setup = NULL;
    minunit_teardown = NULL;

    if(inSuiteName != NULL) {
        (void)putchar('\n');
    }
}

void MINUNIT_suite_configure(MINUNIT_setup_fn_T inSetupFn, MINUNIT_teardown_fn_T inTeardownFn) {
    minunit_setup = inSetupFn;
    minunit_teardown = inTeardownFn;
}

void MINUNIT_run_test(MINUNIT_test_fn_T inTestFn) {
    (void)printf(".");

    if((minunit_real_timer_start == 0.0) && (minunit_proc_timer_start == 0.0)) {
        minunit_real_timer_start = mu_timer_real();
        minunit_proc_timer_start = mu_timer_cpu();
    };

    minunit_testStatus = MU_testStatus_OK;
    minunit_cntTests++;

    // cppcheck-suppress[misra-c2012-15.4]
    do {
        if(minunit_setup != NULL) {
            (*minunit_setup)();
        };
        if(minunit_testStatus != MU_testStatus_OK) {
            // Test failed inside of test setup function
            MINUNIT_fail_test();
            break;
        }

        // Execute test function
        inTestFn();
        if(minunit_testStatus != MU_testStatus_OK) {
            // Test failed inside of test function
            MINUNIT_fail_test();
            break;
        };

        if(minunit_teardown != NULL) {
            (*minunit_teardown)();
        }
        if(minunit_testStatus != MU_testStatus_OK) {
            // Test failed inside of test setup function
            MINUNIT_fail_test();
            break;
        }
    } while(false);

    (void)fflush(stdout);
}

void MINUNIT_report(void) {
    double minunit_real_timer_end;
    double minunit_proc_timer_end;

    (void)printf("\n\n%" PRIu32 " tests from %" PRIu32 " test suites ran, %" PRIu32 " failures\n",
                 minunit_cntTests,
                 minunit_cntSuites,
                 minunit_cntFailures);

    minunit_real_timer_end = mu_timer_real();
    minunit_proc_timer_end = mu_timer_cpu();

    (void)printf("\nFinished in %.8f seconds (real) %.8f seconds (proc)\n\n",
                 minunit_real_timer_end - minunit_real_timer_start,
                 minunit_proc_timer_end - minunit_proc_timer_start);
}

uint32_t MINUNIT_getNumbOfFailedTests(void) {
    return minunit_cntFailures;
}

void MINUNIT_failAndPrintMsg(const char *inFuncName,
                             const char *inFileName,
                             int inLine,
                             const char *inMsg) {
    (void)snprintf(minunit_last_message,
                   MINUNIT_MESSAGE_LEN,
                   "%s failed:\n\t%s:%d: %s",
                   inFuncName,
                   inFileName,
                   inLine,
                   inMsg);

    minunit_testStatus = MU_testStatus_FAIL;
}

MU_testStatus_E MINUNIT_assert_int(int inExpected, int inResult) {
    MU_testStatus_E testFail = MU_testStatus_FAIL;

    if(inExpected != inResult) {
        // Differ
        (void)snprintf(minunit_last_message_str,
                       MINUNIT_MESSAGE_LEN,
                       "%d expected but was %d",
                       inExpected,
                       inResult);
    } else {
        // Same
        testFail = MU_testStatus_OK;
    }
    return testFail;
}

MU_testStatus_E MINUNIT_assert_double(double inExpected, double inResult) {
    MU_testStatus_E testFail = MU_testStatus_FAIL;
    int minunit_significant_figures = 1 - log10(MINUNIT_EPSILON);

    if(fabs(inExpected - inResult) > MINUNIT_EPSILON) {  // Differ
        (void)snprintf(minunit_last_message_str,
                       MINUNIT_MESSAGE_LEN,
                       "%.*g expected but was %.*g",
                       minunit_significant_figures,
                       inExpected,
                       minunit_significant_figures,
                       inResult);
    } else {
        // Same
        testFail = MU_testStatus_OK;
    }
    return testFail;
}

MU_testStatus_E MINUNIT_assert_streq(const char *inExpected, const char *inResult) {
    MU_testStatus_E testFail = MU_testStatus_FAIL;
    const char *tmp_expected;
    const char *tmp_result;

    if(inExpected == NULL) {
        tmp_expected = "<null pointer>";
    } else {
        tmp_expected = inExpected;
    }

    if(inResult == NULL) {
        tmp_result = "<null pointer>";
    } else {
        tmp_result = inResult;
    }

    if(strcmp(tmp_expected, tmp_result) != 0) {
        // Differ
        (void)snprintf(minunit_last_message_str,
                       MINUNIT_MESSAGE_LEN,
                       "'%s' expected but was '%s'",
                       tmp_expected,
                       tmp_result);
    } else {
        // Same
        testFail = MU_testStatus_OK;
    }
    return testFail;
}

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
double mu_timer_real(void) {
#if defined(_WIN32)
    /* Windows 2000 and later. ---------------------------------- */
    LARGE_INTEGER Time;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&Time);

    Time.QuadPart *= 1000000;
    Time.QuadPart /= Frequency.QuadPart;

    return (double)Time.QuadPart / 1000000.0;

#elif(defined(__hpux) || defined(hpux))                          \
        || ((defined(__sun__) || defined(__sun) || defined(sun)) \
            && (defined(__SVR4) || defined(__svr4__)))
    /* HP-UX, Solaris. ------------------------------------------ */
    return (double)gethrtime() / 1000000000.0;

#elif defined(__MACH__) && defined(__APPLE__)
    /* OSX. ----------------------------------------------------- */
    static double timeConvert = 0.0;
    if(timeConvert == 0.0) {
        mach_timebase_info_data_t timeBase;
        (void)mach_timebase_info(&timeBase);
        timeConvert = (double)timeBase.numer / (double)timeBase.denom / 1000000000.0;
    }
    return (double)mach_absolute_time() * timeConvert;

#elif defined(_POSIX_VERSION)
    /* POSIX. --------------------------------------------------- */
    struct timeval tm;
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    {
        struct timespec ts;
#if defined(CLOCK_MONOTONIC_PRECISE)
        /* BSD. --------------------------------------------- */
        const clockid_t id = CLOCK_MONOTONIC_PRECISE;
#elif defined(CLOCK_MONOTONIC_RAW)
        /* Linux. ------------------------------------------- */
        const clockid_t id = CLOCK_MONOTONIC_RAW;
#elif defined(CLOCK_HIGHRES)
        /* Solaris. ----------------------------------------- */
        const clockid_t id = CLOCK_HIGHRES;
#elif defined(CLOCK_MONOTONIC)
        /* AIX, BSD, Linux, POSIX, Solaris. ----------------- */
        const clockid_t id = CLOCK_MONOTONIC;
#elif defined(CLOCK_REALTIME)
        /* AIX, BSD, HP-UX, Linux, POSIX. ------------------- */
        const clockid_t id = CLOCK_REALTIME;
#else
        const clockid_t id = (clockid_t)-1; /* Unknown. */
#endif /* CLOCK_* */
        if(id != (clockid_t)-1 && clock_gettime(id, &ts) != -1) {
            return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
        }
        /* Fall thru. */
    }
#endif /* _POSIX_TIMERS */

    /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, POSIX, Solaris. ----- */
    gettimeofday(&tm, NULL);
    return (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
#else
    return -1.0; /* Failed. */
#endif
}

/**
 * Returns the amount of CPU time used by the current process,
 * in seconds, or -1.0 if an error occurred.
 */
double mu_timer_cpu(void) {
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    FILETIME createTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;

    /* This approach has a resolution of 1/64 second. Unfortunately, Windows' API does not offer
     * better */
    if(GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime) != 0) {
        ULARGE_INTEGER userSystemTime;
        memcpy(&userSystemTime, &userTime, sizeof(ULARGE_INTEGER));
        return (double)userSystemTime.QuadPart / 10000000.0;
    }

#elif defined(__unix__) || defined(__unix) || defined(unix) \
        || (defined(__APPLE__) && defined(__MACH__))
    /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    /* Prefer high-res POSIX timers, when available. */
    {
        clockid_t id;
        struct timespec ts;
#if _POSIX_CPUTIME > 0
        /* Clock ids vary by OS.  Query the id, if possible. */
        if(clock_getcpuclockid(0, &id) == -1)
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
            /* Use known clock id for AIX, Linux, or Solaris. */
            id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
        /* Use known clock id for BSD or HP-UX. */
        id = CLOCK_VIRTUAL;
#else
        id = (clockid_t)-1;
#endif
        if(id != (clockid_t)-1 && clock_gettime(id, &ts) != -1) {
            return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
        }
    }
#endif

#if defined(RUSAGE_SELF)
    {
        struct rusage rusage;
        if(getrusage(RUSAGE_SELF, &rusage) != -1) {
            return (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / 1000000.0;
        }
    }
#endif

#if defined(_SC_CLK_TCK)
    {
        const double ticks = (double)sysconf(_SC_CLK_TCK);
        struct tms tms;
        if(times(&tms) != (clock_t)-1) {
            return (double)tms.tms_utime / ticks;
        }
    }
#endif

#if defined(CLOCKS_PER_SEC)
    {
        clock_t cl = clock();
        if(cl != (clock_t)-1) {
            return (double)cl / (double)CLOCKS_PER_SEC;
        }
    }
#endif

#endif

    return -1; /* Failed. */
}

#ifdef __cplusplus
}
#endif
