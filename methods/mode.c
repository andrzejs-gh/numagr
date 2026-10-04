#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "../numagr.h"
#include "../floats_hash_table.h"

#define iter_HASH_IF_STR(item)                              \
do                                                          \
{                                                           \
    if ( PyUnicode_Check(item) )                            \
    {                                                       \
        const char* str = PyUnicode_AsUTF8(item);           \
        if ( !str )                                         \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return STR_ENCODING_FAILURE;                    \
        }                                                   \
                                                            \
        int ret = fhtable_inc(table, strtod(str, NULL));    \
        if ( ret )                                          \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ret;                                     \
        }                                                   \
    }                                                       \
                                                            \
} while (0);

#define iter_HASH_IF_NUM(item)                                      \
do                                                                  \
{                                                                   \
    if ( PyLong_Check(item) )                                       \
    {                                                               \
        double value = PyLong_AsDouble(item);                       \
        if ( PyErr_Occurred() )                                     \
        {                                                           \
            Py_DECREF(item); Py_DECREF(iter);                       \
            return DOUBLE_OVERFLOW;                                 \
        }                                                           \
                                                                    \
        int ret = fhtable_inc(table, value);                        \
        if ( ret )                                                  \
        {                                                           \
            Py_DECREF(item); Py_DECREF(iter);                       \
            return ret;                                             \
        }                                                           \
                                                                    \
    }                                                               \
    else if ( PyFloat_Check(item) )                                 \
    {                                                               \
        int ret = fhtable_inc(table, PyFloat_AS_DOUBLE(item));      \
        if ( ret )                                                  \
        {                                                           \
            Py_DECREF(item); Py_DECREF(iter);                       \
            return ret;                                             \
        }                                                           \
    }                                                               \
                                                                    \
} while (0);

#define HASH_IF_STR(item)                                   \
do                                                          \
{                                                           \
    if ( PyUnicode_Check(item) )                            \
    {                                                       \
        const char* str = PyUnicode_AsUTF8(item);           \
        if ( !str )                                         \
            return STR_ENCODING_FAILURE;                    \
                                                            \
        int ret = fhtable_inc(table, strtod(str, NULL));    \
        if ( ret ) return ret;                              \
    }                                                       \
                                                            \
} while (0);

#define HASH_IF_NUM(item)                                           \
do                                                                  \
{                                                                   \
    if ( PyLong_Check(item) )                                       \
    {                                                               \
        double value = PyLong_AsDouble(item);                       \
        if ( PyErr_Occurred() )                                     \
            return DOUBLE_OVERFLOW;                                 \
                                                                    \
        int ret = fhtable_inc(table, value);                        \
        if ( ret ) return ret;                                      \
    }                                                               \
    else if ( PyFloat_Check(item) )                                 \
    {                                                               \
        int ret = fhtable_inc(table, PyFloat_AS_DOUBLE(item));      \
        if ( ret ) return ret;                                      \
    }                                                               \
                                                                    \
} while (0);

#define ALLOC_FTABLE(init_est_size)                         \
do                                                          \
{                                                           \
    table = fhtable_new(init_est_size);                     \
    if ( !table.table )                                     \
    {                                                       \
        PyErr_SetString(PyExc_RuntimeError,                 \
                        "Memory allocation failure.");      \
        break;                                              \
    }                                                       \
    table_allocated = true;                                 \
                                                            \
} while (0);

static inline int hash_values_iterable(PyObject* iter, bool only_str,
                                       bool only_num, fhtable* table)
{
    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_HASH_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_HASH_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_HASH_IF_STR(item);
        iter_HASH_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static inline int hash_values_list(PyObject* list, bool only_str, bool only_num,
                                   fhtable* table)
{
    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            HASH_IF_STR(item);
        }
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            HASH_IF_NUM(item);
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);

        HASH_IF_STR(item);
        HASH_IF_NUM(item);
    }

    return 0;
}

static inline int hash_values_tuple(PyObject* list, bool only_str, bool only_num,
                                    fhtable* table)
{
    Py_ssize_t tupl_len = PyTuple_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tupl_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(list, i);
            HASH_IF_STR(item);
        }
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < tupl_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(list, i);
            HASH_IF_NUM(item);
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tupl_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(list, i);

        HASH_IF_STR(item);
        HASH_IF_NUM(item);
    }

    return 0;
}

static inline double get_mode(fhtable* table)
{
    size_t table_capacity = table->capacity;
    fhtable_entry* entries = table->table;
    double current_mode = INFINITY;
    size_t current_top_count = 0;

    for ( size_t i = 0; i < table_capacity; i++ )
    {
        if ( entries[i].count > current_top_count )
        {
            current_top_count = entries[i].count;
            current_mode = entries[i].key;
        }
    }

    return current_mode;
}

static PyObject* mode(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;
    bool is_only_num = false;

    PARSE_ARGS_AND_KWARGS;

    fhtable table;
    bool table_allocated = false;

    int ret;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            if ( !table_allocated )
            {
                ALLOC_FTABLE(PyList_GET_SIZE(arg));
                if ( !table_allocated ) return NULL;
            }

            ret = hash_values_list(arg, is_only_str, is_only_num, &table);
        }
        else if ( PyTuple_Check(arg) )
        {
            if ( !table_allocated )
            {
                ALLOC_FTABLE(PyTuple_GET_SIZE(arg));
                if ( !table_allocated ) return NULL;
            }

            ret = hash_values_tuple(arg, is_only_str, is_only_num, &table);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter )
                return NULL;

            if ( !table_allocated )
            {
                Py_ssize_t size = PyObject_LengthHint(iter, 1024);
                ALLOC_FTABLE(size);
                if ( !table_allocated )
                {
                    Py_DECREF(iter);
                    return NULL;
                }
            }

            ret = hash_values_iterable(iter, is_only_str, is_only_num, &table);
        }
        switch ( ret )
        {
            case 0:
                continue;

            case REALLOC_FAILURE:
                PyErr_SetString(PyExc_RuntimeError,
                                "Memory reallocation failure.");
                fhtable_free(&table);
                return NULL;

            default:
                fhtable_free(&table);
                return NULL;
        }
    }

    double mode = get_mode(&table);
    fhtable_free(&table);

    return PyFloat_FromDouble(mode);
}
