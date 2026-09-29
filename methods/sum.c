#include <Python.h>
#include <stdbool.h>
#include "../numagr.h"

static inline int sum_iterable(PyObject* iter, bool only_str, double* sum,
                                double* correction)
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
                KAHAN_SUM(sum, correction, strtod(str, NULL));
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
            KAHAN_SUM(sum, correction, strtod(str, NULL));
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return DOUBLE_OVERFLOW;
            }
            KAHAN_SUM(sum, correction, value);
        }
        else if ( PyFloat_Check(item) )
        {
            KAHAN_SUM(sum, correction, PyFloat_AS_DOUBLE(item));
        }
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static inline int sum_list(PyObject* list, bool only_str, double* sum,
                            double* correction)
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

                KAHAN_SUM(sum, correction, strtod(str, NULL));
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

            KAHAN_SUM(sum, correction, strtod(str, NULL));
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;

            KAHAN_SUM(sum, correction, value);
        }
        else if ( PyFloat_Check(item) )
        {
            KAHAN_SUM(sum, correction, PyFloat_AS_DOUBLE(item));
        }
    }

    return 0;
}

static inline int sum_tuple(PyObject* tuple, bool only_str, double* sum,
                             double* correction)
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

                KAHAN_SUM(sum, correction, strtod(str, NULL));
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

            KAHAN_SUM(sum, correction, strtod(str, NULL));
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;

            KAHAN_SUM(sum, correction, value);
        }
        else if ( PyFloat_Check(item) )
        {
            KAHAN_SUM(sum, correction, PyFloat_AS_DOUBLE(item));
        }
    }

    return 0;
}

static PyObject* sum(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;

    PARSE_ARGS_AND_KWARGS;

    double sum = 0.0;
    double correction = 0.0;
    int err;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            err = sum_list(arg, is_only_str, &sum, &correction);
        }
        else if ( PyTuple_Check(arg) )
        {
            err = sum_tuple(arg, is_only_str, &sum, &correction);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            err = sum_iterable(iter, is_only_str, &sum, &correction);
        }
        if ( err ) return NULL;
    }

    return PyFloat_FromDouble(sum);
}
