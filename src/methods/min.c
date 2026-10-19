#include <Python.h>
#include <stdbool.h>
#include <float.h>
#include "../numagr.h"

#define UPDATE_MIN_VALUE_IF_STR(item)                   \
do                                                      \
{                                                       \
    if ( PyUnicode_Check(item) )                        \
    {                                                   \
        const char* str = PyUnicode_AsUTF8(item);       \
        if ( !str ) return STR_ENCODING_FAILURE;        \
                                                        \
        double value = strtod(str, NULL);               \
        if ( value < min_val ) min_val = value;         \
    }                                                   \
                                                        \
} while (0);

#define UPDATE_MIN_VALUE_IF_NUM(item)                       \
do                                                          \
{                                                           \
    if ( PyLong_Check(item) )                               \
    {                                                       \
        double value = PyLong_AsDouble(item);               \
        if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;     \
                                                            \
        if ( value < min_val ) min_val = value;             \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( value < min_val ) min_val = value;             \
    }                                                       \
                                                            \
} while (0);

#define iter_UPDATE_MIN_VALUE_IF_STR(item)                  \
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
        if ( value < min_val ) min_val = value;             \
    }                                                       \
                                                            \
} while (0);

#define iter_UPDATE_MIN_VALUE_IF_NUM(item)                  \
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
        if ( value < min_val ) min_val = value;             \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( value < min_val ) min_val = value;             \
    }                                                       \
                                                            \
} while (0);

static inline int get_min_in_list(PyObject* list, bool only_str, bool only_num,
                                  double* min)
{
    double min_val = *min;

    Py_ssize_t count = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            UPDATE_MIN_VALUE_IF_STR(item);
        }
        *min = min_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            UPDATE_MIN_VALUE_IF_NUM(item);
        }
        *min = min_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < count; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);
        UPDATE_MIN_VALUE_IF_STR(item);
        UPDATE_MIN_VALUE_IF_NUM(item);
    }

    *min = min_val;
    return 0;
}

static inline int get_min_in_tuple(PyObject* tuple, bool only_str, bool only_num,
                                   double* min)
{
    double min_val = *min;

    Py_ssize_t count = PyList_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            UPDATE_MIN_VALUE_IF_STR(item);
        }
        *min = min_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            UPDATE_MIN_VALUE_IF_NUM(item);
        }
        *min = min_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < count; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);
        UPDATE_MIN_VALUE_IF_STR(item);
        UPDATE_MIN_VALUE_IF_NUM(item);
    }

    *min = min_val;
    return 0;
}

static inline int get_min_in_iterable(PyObject* iter, bool only_str, bool only_num,
                                      double* min)
{
    double min_val = *min;

    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_UPDATE_MIN_VALUE_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        *min = min_val;
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_UPDATE_MIN_VALUE_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        *min = min_val;
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_UPDATE_MIN_VALUE_IF_STR(item);
        iter_UPDATE_MIN_VALUE_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    *min = min_val;
    return 0;
}

static PyObject* min(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;
    bool is_only_num = false;

    PARSE_ARGS_AND_KWARGS;

    double min = INFINITY;
    int err;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            err = get_min_in_list(arg, is_only_str, is_only_num, &min);
        }
        else if ( PyTuple_Check(arg) )
        {
            err = get_min_in_tuple(arg, is_only_str, is_only_num, &min);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            err = get_min_in_iterable(iter, is_only_str, is_only_num, &min);
        }
        if ( err ) return NULL;
    }

    return PyFloat_FromDouble(min);
}

numagr_method get_min_method_ptr(void)
{
    return min;
}
