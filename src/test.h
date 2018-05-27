#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define ASSERT(cond) ASSERTV(cond, "None")

#define ASSERTV(cond, fmt, ...) \
    if(!(cond)) { \
        fprintf(stdout, "F\n"); \
        report_failure(#cond, __FILE__, __LINE__); \
        fprintf(stdout, "Context: " fmt "\n", ##__VA_ARGS__); \
        return false; \
    } else { \
        fprintf(stdout, "."); \
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
