#ifndef FLOATS_ARRAY_H
#define FLOATS_ARRAY_H

#include <stddef.h>

typedef struct
{
    size_t capacity;
    size_t count;
    double* addr;

} fdarray;

enum fdarray_err_codes
{
    ARR_SIZE_OVERFLOW = 1,
    ARR_REALLOC_FAILURE
};

fdarray fdarray_new(size_t init_capacity);
int add_value(fdarray* array, double value);

#endif
