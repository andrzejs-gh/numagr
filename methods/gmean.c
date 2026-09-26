#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "../numagr.h"

static inline int get_log_sum_list(PyObject* list, bool only_str, double* log_sum)
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
                if ( value < 0.0 ) return NEGATIVE_VALUE;

                *log_sum += log(value);
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
            if ( value < 0.0 ) return NEGATIVE_VALUE;

            *log_sum += log(value);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
                return DOUBLE_OVERFLOW;
            else if ( value < 0.0 )
                return NEGATIVE_VALUE;

            *log_sum += log(value);
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value < 0.0 )
                return NEGATIVE_VALUE;

            *log_sum += log(value);
        }
    }

    return 0;
}

static inline int get_log_sum_tuple(PyObject* tuple, bool only_str, double* log_sum)
{
    Py_ssize_t count = PyTuple_GET_SIZE(tuple);

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
                if ( value < 0.0 ) return NEGATIVE_VALUE;

                *log_sum += log(value);
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
            if ( value < 0.0 ) return NEGATIVE_VALUE;

            *log_sum += log(value);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
                return DOUBLE_OVERFLOW;
            else if ( value < 0.0 )
                return NEGATIVE_VALUE;

            *log_sum += log(value);
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value < 0.0 )
                return NEGATIVE_VALUE;

            *log_sum += log(value);
        }
    }

    return 0;
}

static inline int get_log_sum_iterable(PyObject* iter, bool only_str, double* log_sum)
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
                if ( value < 0.0 )
                {
                    Py_DECREF(item); Py_DECREF(iter);
                    return NEGATIVE_VALUE;
                }
                *log_sum += log(value);
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
            if ( value < 0.0 )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return NEGATIVE_VALUE;
            }
            *log_sum += log(value);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return DOUBLE_OVERFLOW;
            }
            else if ( value < 0.0 )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return NEGATIVE_VALUE;
            }
            *log_sum += log(value);
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value < 0.0 )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return NEGATIVE_VALUE;
            }
            *log_sum += log(value);
        }
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static PyObject* gmean(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;

    PARSE_ARGS_AND_KWARGS;

    double log_sum = 0.0;
    uint64_t count = 0;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {

    }

    return Py_None;
    // return PyFloat_FromDouble();
}
