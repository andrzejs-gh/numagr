#ifndef FLOATS_ARRAY_H
#define FLOATS_ARRAY_H

#include <stddef.h>

typedef struct
{
    size_t capacity;
    size_t count;
    double* addr;

} fdarray;

fdarray fdarray_new(size_t init_capacity);
int fdarray_push(fdarray* array, double value);

#endif
