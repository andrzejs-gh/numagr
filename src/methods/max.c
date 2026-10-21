#include <Python.h>
#include <stdbool.h>
#include "../numagr.h"

#define UPDATE_MAX_VALUE_IF_STR(item)                   \
do                                                      \
{                                                       \
    if ( PyUnicode_Check(item) )                        \
    {                                                   \
        const char* str = PyUnicode_AsUTF8(item);       \
        if ( !str ) return STR_ENCODING_FAILURE;        \
                                                        \
        double value = strtod(str, NULL);               \
        if ( value > max_val ) max_val = value;         \
    }                                                   \
                                                        \
} while (0);

#define UPDATE_MAX_VALUE_IF_NUM(item)                       \
do                                                          \
{                                                           \
    if ( PyLong_Check(item) )                               \
    {                                                       \
        double value = PyLong_AsDouble(item);               \
        if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;     \
                                                            \
        if ( value > max_val ) max_val = value;             \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( value > max_val ) max_val = value;             \
    }                                                       \
                                                            \
} while (0);

#define iter_UPDATE_MAX_VALUE_IF_STR(item)                  \
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
        double value = strtod(str, NULL);                   \
        if ( value > max_val ) max_val = value;             \
    }                                                       \
                                                            \
} while (0);

#define iter_UPDATE_MAX_VALUE_IF_NUM(item)                  \
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
        if ( value > max_val ) max_val = value;             \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( value > max_val ) max_val = value;             \
    }                                                       \
                                                            \
} while (0);

static inline int get_max_in_list(PyObject* list, bool only_str, bool only_num,
                                  double* max)
{
    double max_val = *max;

    Py_ssize_t count = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            UPDATE_MAX_VALUE_IF_STR(item);
        }
        *max = max_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            UPDATE_MAX_VALUE_IF_NUM(item);
        }
        *max = max_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < count; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);

        UPDATE_MAX_VALUE_IF_STR(item);
        UPDATE_MAX_VALUE_IF_NUM(item);
    }

    *max = max_val;
    return 0;
}

static inline int get_max_in_tuple(PyObject* tuple, bool only_str, bool only_num,
                                   double* max)
{
    double max_val = *max;

    Py_ssize_t count = PyList_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            UPDATE_MAX_VALUE_IF_STR(item);
        }
        *max = max_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            UPDATE_MAX_VALUE_IF_NUM(item);
        }
        *max = max_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < count; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);

        UPDATE_MAX_VALUE_IF_STR(item);
        UPDATE_MAX_VALUE_IF_NUM(item);
    }

    *max = max_val;
    return 0;
}

static inline int get_max_in_iterable(PyObject* iter, bool only_str, bool only_num,
                                      double* max)
{
    double max_val = *max;

    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_UPDATE_MAX_VALUE_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        *max = max_val;
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_UPDATE_MAX_VALUE_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        *max = max_val;
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_UPDATE_MAX_VALUE_IF_STR(item);
        iter_UPDATE_MAX_VALUE_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    *max = max_val;
    return 0;
}

static PyObject* max_value(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;
    bool is_only_num = false;

    PARSE_ARGS_AND_KWARGS;

    double max = -INFINITY;
    int err;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            err = get_max_in_list(arg, is_only_str, is_only_num, &max);
        }
        else if ( PyTuple_Check(arg) )
        {
            err = get_max_in_tuple(arg, is_only_str, is_only_num, &max);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            err = get_max_in_iterable(iter, is_only_str, is_only_num, &max);
        }
        if ( err ) return NULL;
    }

    return PyFloat_FromDouble(max);
}
