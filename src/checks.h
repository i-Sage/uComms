#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>       /// Error integer and strerror() function

typedef enum
{
    ERR = -1,
    OK = 1,
    EXPECTED = 0
} err_type_t;


#define CHECK(condition, msg)                                                \
    do                                                                       \
    {                                                                        \
        if (!(condition))                                                    \
        {                                                                    \
            fprintf(stderr, "ERROR: %s (%s:%d)\n", msg, __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                                              \
        }                                                                    \
    } while (0)


#define CHECK_OPEN(condition)                                                   \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            fprintf(stderr, "ERROR: from open (%s:%d)\n", __FILE__, __LINE__);  \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
    } while (0)



#define CHECK_PTR(ptr) do {                                                                        \
    if ((ptr) == NULL) {                                                                           \
        fprintf(stderr, "ERROR: NULL POINTER DETECTED: %s AT %s:%d\n", #ptr, __FILE__, __LINE__);  \
        exit(EXIT_FAILURE);                                                                        \
    }                                                                                              \
} while(0)   


#define ERROR_CHECK(call, expected) do {                                                           \
        int _ret = (call);                                                                         \
        if (_ret != (expected)) {                                                                  \
            fprintf(stderr, "ERROR: call '%s' returned %d, expected %d\n", #call, _ret, expected); \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while(0)

