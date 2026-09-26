#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "../numagr.h"
#include "abstract.h"

static inline int get_log_sum_list(PyObject* list, bool only_str,
                                   double* log_sum, uint64_t* count)
{
    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);

            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str ) return STR_ENCODING_FAILURE;

                double value = strtod(str, NULL);
                if ( value < 0.0 )
                    return NEGATIVE_VALUE;
                if ( (*count)++ == UINT64_MAX )
                    return COUNT_OVERFLOW;

                *log_sum += log(value);
            }
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);

        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str ) return STR_ENCODING_FAILURE;

            double value = strtod(str, NULL);
            if ( value < 0.0 )
                return NEGATIVE_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *log_sum += log(value);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
                return DOUBLE_OVERFLOW;
            if ( value < 0.0 )
                return NEGATIVE_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *log_sum += log(value);
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value < 0.0 )
                return NEGATIVE_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *log_sum += log(value);
        }
    }

    return 0;
}

static inline int get_log_sum_tuple(PyObject* tuple, bool only_str,
                                    double* log_sum, uint64_t* count)
{
    Py_ssize_t tuple_len = PyTuple_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);

            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str ) return STR_ENCODING_FAILURE;

                double value = strtod(str, NULL);
                if ( value < 0.0 )
                    return NEGATIVE_VALUE;
                if ( (*count)++ == UINT64_MAX )
                    return COUNT_OVERFLOW;

                *log_sum += log(value);
            }
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tuple_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);

        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str ) return STR_ENCODING_FAILURE;

            double value = strtod(str, NULL);
            if ( value < 0.0 )
                return NEGATIVE_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *log_sum += log(value);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
                return DOUBLE_OVERFLOW;
            if ( value < 0.0 )
                return NEGATIVE_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *log_sum += log(value);
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value < 0.0 )
                return NEGATIVE_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *log_sum += log(value);
        }
    }

    return 0;
}

static inline int get_log_sum_iterable(PyObject* iter, bool only_str,
                                       double* log_sum, uint64_t* count)
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
                if ( (*count)++ == UINT64_MAX )
                {
                    Py_DECREF(item); Py_DECREF(iter);
                    return COUNT_OVERFLOW;
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
            if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
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
            if ( value < 0.0 )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return NEGATIVE_VALUE;
            }
            if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
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
            if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
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
    int ret;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            ret = get_log_sum_list(arg, is_only_str, &log_sum, &count);
        }
        else if ( PyTuple_Check(arg) )
        {
            ret = get_log_sum_tuple(arg, is_only_str, &log_sum, &count);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            ret = get_log_sum_iterable(iter, is_only_str, &log_sum, &count);
        }
        HANDLE_SUM_AND_COUNT_RET(ret);
    }

    if ( !count )
        return PyFloat_FromDouble(0.0);

    return PyFloat_FromDouble(exp(log_sum / count));
}
