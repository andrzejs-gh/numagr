#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "floats_array.h"

fdarray fdarray_new(size_t init_capacity)
{
    double* addr = malloc(init_capacity * sizeof *addr);
    if ( !addr )
        return (fdarray){0};

    return (fdarray)
           {
                .capacity = init_capacity,
                .count = 0,
                .addr = addr
           };
}

int fdarray_push(fdarray* array, double value)
{
    size_t count = array->count;

    if ( count == SIZE_MAX )
    {
        free(array->addr);
        return ARR_SIZE_OVERFLOW;
    }

    if ( count == array->capacity )
    {
        double* new_addr = realloc(array->addr, 2*array->capacity);
        if ( !new_addr )
        {
            free(array->addr);
            return ARR_REALLOC_FAILURE;
        }

        array->addr = new_addr;
    }
    array->addr[count] = value;
    array->count++;

    return 0;
}
