#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Prints an error (internal usage)
#define _ASSERT_FAILURE_QUIET(cond) \
    fprintf(stdout, "F\n"); \
    report_failure(#cond, __FILE__, __LINE__); \

// Prints  context of an error and an error (internal usage)
#define _ASSERT_FAILURE(cond, fmt, ...) \
    _ASSERT_FAILURE_QUIET(cond) \
    fprintf(stdout, "Context: " fmt "\n", ##__VA_ARGS__); \

// Assert without context
#define ASSERT(cond) \
    if(!(cond)) { \
        _ASSERT_FAILURE_QUIET(cond); \
        return false; \
    } else { \
        fprintf(stdout, "."); \
    }

// Assert with context
#define ASSERTV(cond, fmt, ...) \
    if(!(cond)) { \
        _ASSERT_FAILURE(cond, fmt, ##__VA_ARGS__); \
        return false; \
    } else { \
        fprintf(stdout, "."); \
    }

// Assert without success indicator
#define ASSERT_QUIET_SUCCESS(cond, fmt, ...) \
    if(!(cond)) { \
        _ASSERT_FAILURE(cond, fmt, ##__VA_ARGS__); \
        return false; \
    }

typedef bool (*test_func)(void);

void test(const char *description, test_func func)
{
    fprintf(stdout, "Running %s: ", description);

    (*func)();

    fprintf(stdout, "\n");
}

void report_failure(const char *condition, const char *file, int line)
{
    fprintf(stderr, "Failure in %s:%d: %s\n", file, line, condition);
}

#endif /* TEST_H */
