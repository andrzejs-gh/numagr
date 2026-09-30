#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "floats_hash_table.h"

static inline uint64_t get_hash(double key)
{
    if ( !key ) key = 0.0; // normalize negative zero just in case

    unsigned char* bytes = (unsigned char*)&key;
    uint64_t hash = FNV_OFFSET_BASIS;

    for ( size_t i = 0; i < sizeof(double); i++ )
    {
        hash ^= bytes[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

fhtable fhtable_new(size_t init_est_count)
{
    fhtable_entry* table = calloc(2 * init_est_count, sizeof(fhtable_entry));
    if ( !table )
        return (fhtable){0};

    return (fhtable)
           {
               .count = 0,
               .capacity = 2*init_est_count,
               .table = table
           };
}

void fhtable_free(fhtable* fht)
{
    free(fht->table);
    *fht = (fhtable){0};

    return;
}

static inline fhtable* fhtable_grow(fhtable* fht)
{
    size_t old_capacity = fht->capacity;
    size_t new_capacity = FHTABLE_GROWTH_FACTOR*old_capacity;

    fhtable_entry* new_table = calloc(new_capacity, sizeof(fhtable_entry));
    if ( !new_table )
        return NULL;

    fhtable_entry* old_table = fht->table;

    for ( size_t i = 0; i < old_capacity; i++ )
    {
        if ( old_table[i].count )
        {
            size_t new_index = old_table[i].hash % new_capacity;

            while ( new_table[new_index].count )
            {
                if ( ++new_index == new_capacity )
                    new_index = 0;
            }

            new_table[new_index] = (fhtable_entry)
                                   {
                                       .key = old_table[i].key,
                                       .count = old_table[i].count,
                                       .hash = old_table[i].hash
                                   };
        }
    }

    fht->capacity = new_capacity;
    fht->table = new_table;
    free(old_table);

    return fht;
}

int fhtable_inc(fhtable* fht, double key)
{
    size_t fht_capacity = fht->capacity;
    uint64_t hash = get_hash(key);
    size_t index = hash % fht_capacity;
    fhtable_entry* table = fht->table;

    while ( table[index].count )
    {
        if ( table[index].key == key )
        {
            table[index].count++;
            return 0;
        }
        if ( ++index == fht_capacity )
            index = 0;
    }

    table[index] = (fhtable_entry){key, 1, hash};

    if ( (++fht->count / (double)fht_capacity) > FHTABLE_LOAD_FACTOR )
    {
        void* ret = fhtable_grow(fht);
        if ( !ret )
            return REALLOC_FAILURE;
    }

    return 0;
}

size_t fhtable_get_count(fhtable* fht, double key)
{
    size_t fht_capacity  = fht->capacity;
    size_t index = get_hash(key) % fht_capacity;
    fhtable_entry* table = fht->table;

    while ( table[index].count )
    {
        if ( table[index].key == key )
            return table[index].count;

        if ( ++index == fht_capacity )
            index = 0;
    }

    return VALUE_MISSING;
}
