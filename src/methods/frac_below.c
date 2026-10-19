#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "../numagr.h"

#define COUNT_IF_BELOW_IF_STR(item)                             \
do                                                              \
{                                                               \
    if ( PyUnicode_Check(item) )                                \
    {                                                           \
        const char* str = PyUnicode_AsUTF8(item);               \
        if ( !str ) return STR_ENCODING_FAILURE;                \
                                                                \
        if ( count_val++ == UINT64_MAX )                        \
            return COUNT_OVERFLOW;                              \
                                                                \
        if ( strtod(str, NULL) < target )                       \
            below_count_val++;                                  \
    }                                                           \
                                                                \
} while (0);

#define COUNT_IF_BELOW_IF_NUM(item)                             \
do                                                              \
{                                                               \
    if ( PyLong_Check(item) )                                   \
    {                                                           \
        double value = PyLong_AsDouble(item);                   \
        if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;         \
                                                                \
        if ( count_val++ == UINT64_MAX )                        \
            return COUNT_OVERFLOW;                              \
                                                                \
        if ( value < target )                                   \
            below_count_val++;                                  \
    }                                                           \
    else if ( PyFloat_Check(item) )                             \
    {                                                           \
        if ( count_val++ == UINT64_MAX )                        \
            return COUNT_OVERFLOW;                              \
                                                                \
        if ( PyFloat_AS_DOUBLE(item) < target )                 \
            below_count_val++;                                  \
    }                                                           \
                                                                \
} while (0);

#define iter_COUNT_IF_BELOW_IF_STR(item)                        \
do                                                              \
{                                                               \
    if ( PyUnicode_Check(item) )                                \
    {                                                           \
        const char* str = PyUnicode_AsUTF8(item);               \
        if ( !str )                                             \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return STR_ENCODING_FAILURE;                        \
        }                                                       \
                                                                \
        if ( count_val++ == UINT64_MAX )                        \
            return COUNT_OVERFLOW;                              \
                                                                \
        if ( strtod(str, NULL) < target )                       \
            below_count_val++;                                  \
    }                                                           \
                                                                \
} while (0);

#define iter_COUNT_IF_BELOW_IF_NUM(item)                        \
do                                                              \
{                                                               \
    if ( PyLong_Check(item) )                                   \
    {                                                           \
        double value = PyLong_AsDouble(item);                   \
        if ( PyErr_Occurred() )                                 \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return DOUBLE_OVERFLOW;                             \
        }                                                       \
                                                                \
        if ( count_val++ == UINT64_MAX )                        \
            return COUNT_OVERFLOW;                              \
                                                                \
        if ( value < target )                                   \
            below_count_val++;                                  \
    }                                                           \
    else if ( PyFloat_Check(item) )                             \
    {                                                           \
        if ( count_val++ == UINT64_MAX )                        \
            return COUNT_OVERFLOW;                              \
                                                                \
        if ( PyFloat_AS_DOUBLE(item) < target )                 \
            below_count_val++;                                  \
    }                                                           \
                                                                \
} while (0);

static inline int below_count_in_list(PyObject* list, bool only_str, bool only_num,
                                      double target, uint64_t* below_count,
                                      uint64_t* count)
{
    uint64_t below_count_val = *below_count;
    uint64_t count_val = *count;

    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            COUNT_IF_BELOW_IF_STR(item);
        }
        *below_count = below_count_val;
        *count = count_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            COUNT_IF_BELOW_IF_NUM(item);
        }
        *below_count = below_count_val;
        *count = count_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);
        COUNT_IF_BELOW_IF_STR(item);
        COUNT_IF_BELOW_IF_NUM(item);
    }

    *below_count = below_count_val;
    *count = count_val;
    return 0;
}

static inline int below_count_in_tuple(PyObject* tuple, bool only_str, bool only_num,
                                       double target, uint64_t* below_count,
                                       uint64_t* count)
{
    uint64_t below_count_val = *below_count;
    uint64_t count_val = *count;

    Py_ssize_t tuple_len = PyTuple_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            COUNT_IF_BELOW_IF_STR(item);
        }
        *below_count = below_count_val;
        *count = count_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            COUNT_IF_BELOW_IF_NUM(item);
        }
        *below_count = below_count_val;
        *count = count_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tuple_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);
        COUNT_IF_BELOW_IF_STR(item);
        COUNT_IF_BELOW_IF_NUM(item);
    }

    *below_count = below_count_val;
    *count = count_val;
    return 0;
}

static inline int below_count_in_iterable(PyObject* iter, bool only_str, bool only_num,
                                          double target, uint64_t* below_count,
                                          uint64_t* count)
{
    uint64_t below_count_val = *below_count;
    uint64_t count_val = *count;

    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_COUNT_IF_BELOW_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        *below_count = below_count_val;
        *count = count_val;
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_COUNT_IF_BELOW_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        *below_count = below_count_val;
        *count = count_val;
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_COUNT_IF_BELOW_IF_STR(item);
        iter_COUNT_IF_BELOW_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    *below_count = below_count_val;
    *count = count_val;
    return 0;
}

static PyObject* frac_below(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;
    bool is_only_num = false;
    double target;

    PARSE_TARGET_VALUE;
    PARSE_ARGS_AND_KWARGS;

    uint64_t below_count = 0;
    uint64_t count = 0;
    int ret;

    for ( Py_ssize_t i = 1; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            ret = below_count_in_list(arg, is_only_str, is_only_num, target,
                                      &below_count, &count);
        }
        else if ( PyTuple_Check(arg) )
        {
            ret = below_count_in_tuple(arg, is_only_str, is_only_num, target,
                                       &below_count, &count);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            ret = below_count_in_iterable(iter, is_only_str, is_only_num, target,
                                          &below_count, &count);
        }
        HANDLE_RETURN_CODES(ret);
    }

    if ( !count )
        return PyFloat_FromDouble(INFINITY);

    return PyFloat_FromDouble( (double)below_count / count );
}
