#ifndef FLOATS_HASH_TABLE_H
#define FLOATS_HASH_TABLE_H

#include <stddef.h>
#include <stdint.h>

#define FHTABLE_LOAD_FACTOR 0.5
#define FHTABLE_GROWTH_FACTOR 2.0

#define FNV_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3

#define VALUE_MISSING SIZE_MAX

typedef struct
{
    double key;
    size_t count;
    uint64_t hash;

} fhtable_entry;

typedef struct
{
    size_t count;
    size_t capacity;
    fhtable_entry* table;

} fhtable;

enum fhtable_error_codes
{
    REALLOC_FAILURE = 1
};

fhtable fhtable_new(size_t init_est_count);
void fhtable_free(fhtable* fht);
int fhtable_inc(fhtable* fht, double key);
size_t fhtable_get_count(fhtable* fht, double key);

#endif
