#include <Python.h>
#include <stdbool.h>
#include <float.h>
#include "../numagr.h"

static inline int get_max_in_list(PyObject* list, bool only_str, double* max)
{
    Py_ssize_t count = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);

            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str ) return STR_ENCODING_FAILURE;

                double value = strtod(str, NULL);
                if ( value > *max ) *max = value;
            }
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < count; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);

        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str ) return STR_ENCODING_FAILURE;

            double value = strtod(str, NULL);
            if ( value > *max ) *max = value;
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;

            if ( value > *max ) *max = value;
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value > *max ) *max = value;
        }
    }

    return 0;
}

static inline int get_max_in_tuple(PyObject* tuple, bool only_str, double* max)
{
    Py_ssize_t count = PyList_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);

            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str ) return STR_ENCODING_FAILURE;

                double value = strtod(str, NULL);
                if ( value > *max ) *max = value;
            }
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < count; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);

        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str ) return STR_ENCODING_FAILURE;

            double value = strtod(str, NULL);
            if ( value > *max ) *max = value;
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;

            if ( value > *max ) *max = value;
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value > *max ) *max = value;
        }
    }

    return 0;
}

static inline int get_max_in_iterable(PyObject* iter, bool only_str, double* max)
{
    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str )
                {
                    Py_DECREF(item); Py_DECREF(iter);
                    return STR_ENCODING_FAILURE;
                }
                double value = strtod(str, NULL);
                if ( value > *max ) *max = value;
            }
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return STR_ENCODING_FAILURE;
            }
            double value = strtod(str, NULL);
            if ( value > *max ) *max = value;
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return DOUBLE_OVERFLOW;
            }
            if ( value > *max ) *max = value;
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value > *max ) *max = value;
        }
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static PyObject* max(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;

    PARSE_ARGS_AND_KWARGS;

    double max = -INFINITY;
    int err;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            err = get_max_in_list(arg, is_only_str, &max);
        }
        else if ( PyTuple_Check(arg) )
        {
            err = get_max_in_tuple(arg, is_only_str, &max);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            err = get_max_in_iterable(iter, is_only_str, &max);
        }
        if ( err ) return NULL;
    }

    return PyFloat_FromDouble(max);
}
