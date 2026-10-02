#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "../numagr.h"

#define iter_ADD_AND_COUNT_IF_STR(item)                     \
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
        if ( (*count)++ == UINT64_MAX )                     \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return COUNT_OVERFLOW;                          \
        }                                                   \
        KAHAN_SUM(sum, correction, strtod(str, NULL));      \
    }                                                       \
                                                            \
} while (0);

#define iter_ADD_AND_COUNT_IF_NUM(item)                         \
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
        if ( (*count)++ == UINT64_MAX )                         \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return COUNT_OVERFLOW;                              \
        }                                                       \
        KAHAN_SUM(sum, correction, value);                      \
    }                                                           \
    else if ( PyFloat_Check(item) )                             \
    {                                                           \
        if ( (*count)++ == UINT64_MAX )                         \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return COUNT_OVERFLOW;                              \
        }                                                       \
        KAHAN_SUM(sum, correction, PyFloat_AS_DOUBLE(item));    \
    }                                                           \
                                                                \
} while (0);

#define ADD_AND_COUNT_IF_STR(item)                          \
do                                                          \
{                                                           \
    if ( PyUnicode_Check(item) )                            \
    {                                                       \
        const char* str = PyUnicode_AsUTF8(item);           \
        if ( !str )                                         \
            return STR_ENCODING_FAILURE;                    \
        if ( (*count)++ == UINT64_MAX )                     \
            return COUNT_OVERFLOW;                          \
                                                            \
        KAHAN_SUM(sum, correction, strtod(str, NULL));      \
    }                                                       \
                                                            \
} while (0);

#define ADD_AND_COUNT_IF_NUM(item)                              \
do                                                              \
{                                                               \
    if ( PyLong_Check(item) )                                   \
    {                                                           \
        double value = PyLong_AsDouble(item);                   \
        if ( PyErr_Occurred() )                                 \
            return DOUBLE_OVERFLOW;                             \
        if ( (*count)++ == UINT64_MAX )                         \
            return COUNT_OVERFLOW;                              \
                                                                \
        KAHAN_SUM(sum, correction, value);                      \
    }                                                           \
    else if ( PyFloat_Check(item) )                             \
    {                                                           \
        if ( (*count)++ == UINT64_MAX )                         \
            return COUNT_OVERFLOW;                              \
        KAHAN_SUM(sum, correction, PyFloat_AS_DOUBLE(item));    \
    }                                                           \
                                                                \
} while (0);

static inline int sum_and_count_iterable(PyObject* iter, bool only_str,
                                         bool only_num, double* sum,
                                         double* correction, uint64_t* count)
{
    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_AND_COUNT_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_AND_COUNT_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_ADD_AND_COUNT_IF_STR(item);
        iter_ADD_AND_COUNT_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static inline int sum_and_count_list(PyObject* list, bool only_str, bool only_num,
                                     double* sum, double* correction, uint64_t* count)
{
    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_AND_COUNT_IF_STR(item);
        }
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_AND_COUNT_IF_NUM(item);
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);

        ADD_AND_COUNT_IF_STR(item);
        ADD_AND_COUNT_IF_NUM(item);
    }

    return 0;
}

static inline int sum_and_count_tuple(PyObject* list, bool only_str, bool only_num,
                                      double* sum, double* correction,
                                      uint64_t* count)
{
    Py_ssize_t tupl_len = PyTuple_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tupl_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(list, i);
            ADD_AND_COUNT_IF_STR(item);
        }
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < tupl_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(list, i);
            ADD_AND_COUNT_IF_NUM(item);
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tupl_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(list, i);

        ADD_AND_COUNT_IF_STR(item);
        ADD_AND_COUNT_IF_NUM(item);
    }

    return 0;
}

static PyObject* amean(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;
    bool is_only_num = false;

    PARSE_ARGS_AND_KWARGS;

    double sum = 0.0;
    double correction = 0.0;
    uint64_t count = 0;
    int ret;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            ret = sum_and_count_list(arg, is_only_str, is_only_num, &sum,
                                     &correction, &count);
        }
        else if ( PyTuple_Check(arg) )
        {
            ret = sum_and_count_tuple(arg, is_only_str, is_only_num, &sum,
                                      &correction, &count);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            ret = sum_and_count_iterable(iter, is_only_str, is_only_num, &sum,
                                         &correction, &count);
        }
        HANDLE_SUM_AND_COUNT_RET(ret);
    }

    if ( !count )
        return PyFloat_FromDouble(INFINITY);

    return PyFloat_FromDouble(sum / count);
}
