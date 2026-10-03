#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "../numagr.h"

#define ADD_RECIP_AND_COUNT_IF_STR(item)                    \
do                                                          \
{                                                           \
    if ( PyUnicode_Check(item) )                            \
    {                                                       \
        const char* str = PyUnicode_AsUTF8(item);           \
        if ( !str ) return STR_ENCODING_FAILURE;            \
                                                            \
        double value = strtod(str, NULL);                   \
        if ( !value )                                       \
            return ZERO_VALUE;                              \
        if ( (*count)++ == UINT64_MAX )                     \
            return COUNT_OVERFLOW;                          \
                                                            \
        KAHAN_SUM(recip_sum, correction, min/value);        \
    }                                                       \
                                                            \
} while (0);

#define ADD_RECIP_AND_COUNT_IF_NUM(item)                    \
do                                                          \
{                                                           \
    if ( PyLong_Check(item) )                               \
    {                                                       \
        double value = PyLong_AsDouble(item);               \
        if ( PyErr_Occurred() )                             \
            return DOUBLE_OVERFLOW;                         \
        if ( !value )                                       \
            return ZERO_VALUE;                              \
        if ( (*count)++ == UINT64_MAX )                     \
            return COUNT_OVERFLOW;                          \
                                                            \
        KAHAN_SUM(recip_sum, correction, min/value);        \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( !value )                                       \
            return ZERO_VALUE;                              \
        if ( (*count)++ == UINT64_MAX )                     \
            return COUNT_OVERFLOW;                          \
                                                            \
        KAHAN_SUM(recip_sum, correction, min/value);        \
    }                                                       \
                                                            \
} while (0);

#define iter_ADD_RECIP_AND_COUNT_IF_STR(item)               \
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
        double value = strtod(str, NULL);                   \
        if ( !value )                                       \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ZERO_VALUE;                              \
        }                                                   \
        if ( (*count)++ == UINT64_MAX )                     \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return COUNT_OVERFLOW;                          \
        }                                                   \
        KAHAN_SUM(recip_sum, correction, min/value);        \
    }                                                       \
                                                            \
} while (0);

#define iter_ADD_RECIP_AND_COUNT_IF_NUM(item)               \
do                                                          \
{                                                           \
    if ( PyLong_Check(item) )                               \
    {                                                       \
        double value = PyLong_AsDouble(item);               \
        if ( PyErr_Occurred() )                             \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return DOUBLE_OVERFLOW;                         \
        }                                                   \
        if ( !value )                                       \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ZERO_VALUE;                              \
        }                                                   \
        if ( (*count)++ == UINT64_MAX )                     \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return COUNT_OVERFLOW;                          \
        }                                                   \
        KAHAN_SUM(recip_sum, correction, min/value);        \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( !value )                                       \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ZERO_VALUE;                              \
        }                                                   \
        if ( (*count)++ == UINT64_MAX )                     \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return COUNT_OVERFLOW;                          \
        }                                                   \
        KAHAN_SUM(recip_sum, correction, min/value);        \
    }                                                       \
                                                            \
} while (0);

static inline int get_recip_sum_list(PyObject* list, bool only_str, bool only_num,
                                     double min, double* recip_sum, double* correction,
                                     uint64_t* count)
{
    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_RECIP_AND_COUNT_IF_STR(item);
        }
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_RECIP_AND_COUNT_IF_NUM(item);
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);
        ADD_RECIP_AND_COUNT_IF_STR(item);
        ADD_RECIP_AND_COUNT_IF_NUM(item);
    }

    return 0;
}

static inline int get_recip_sum_tuple(PyObject* tuple, bool only_str, bool only_num,
                                      double min, double* recip_sum,
                                      double* correction, uint64_t* count)
{
    Py_ssize_t tuple_len = PyTuple_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            ADD_RECIP_AND_COUNT_IF_STR(item);
        }
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            ADD_RECIP_AND_COUNT_IF_NUM(item);
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tuple_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);
        ADD_RECIP_AND_COUNT_IF_STR(item);
        ADD_RECIP_AND_COUNT_IF_NUM(item);
    }

    return 0;
}

static inline int get_recip_sum_iterable(PyObject* iter, bool only_str, bool only_num,
                                         double min, double* recip_sum,
                                         double* correction, uint64_t* count)
{
    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_RECIP_AND_COUNT_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_RECIP_AND_COUNT_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_ADD_RECIP_AND_COUNT_IF_STR(item);
        iter_ADD_RECIP_AND_COUNT_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static inline PyObject* get_normalized_args(Py_ssize_t argc, PyObject* args)
{
    PyObject* normalized_args = PyTuple_New(argc);
    if ( !normalized_args )
        return NULL;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) || PyTuple_Check(arg) )
        {
            Py_INCREF(arg);
            PyTuple_SET_ITEM(normalized_args, i, arg);
        }
        else
        {
            PyObject* tuple = PySequence_Tuple(arg);
            if ( !tuple )
            {
                Py_DECREF(normalized_args);
                return NULL;
            }
            PyTuple_SET_ITEM(normalized_args, i, tuple);
        }
    }

    return normalized_args;
}

static PyObject* hmean(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);

    bool is_only_str = false;
    bool is_only_num = false;

    PARSE_ARGS_AND_KWARGS;

    PyObject* normalized_args = get_normalized_args(argc, args);
    if ( !normalized_args )
        return NULL;

    PyObject* py_float = get_min_method_ptr()(self, normalized_args, kwargs);
    if ( !py_float )
    {
        Py_DECREF(normalized_args);
        return NULL;
    }

    double min_val = PyFloat_AS_DOUBLE(py_float);
    if ( min_val <= 0.0 )
    {
        PyErr_SetString(PyExc_RuntimeError,
                        "The iterable contains a non-positive value.");
        Py_DECREF(normalized_args);
        return NULL;
    }

    double recip_sum = 0.0;
    double correction = 0.0;
    uint64_t count = 0;
    int ret;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(normalized_args, i);

        if ( PyList_Check(arg) )
        {
            ret = get_recip_sum_list(arg, is_only_str, is_only_num, min_val,
                                     &recip_sum, &correction, &count);
        }
        else
        {
            ret = get_recip_sum_tuple(arg, is_only_str, is_only_num, min_val,
                                      &recip_sum, &correction, &count);
        }
        HANDLE_SUM_AND_COUNT_RET(ret);
    }
    Py_DECREF(normalized_args);

    if ( !count )
        return PyFloat_FromDouble(INFINITY);

    return PyFloat_FromDouble((min_val*count)/recip_sum);
}
