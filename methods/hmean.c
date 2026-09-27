#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include "../numagr.h"

static inline int get_recip_sum_list(PyObject* list, bool only_str, double min,
                                     double* recip_sum, uint64_t* count)
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
                if ( !value )
                    return ZERO_VALUE;
                if ( (*count)++ == UINT64_MAX )
                    return COUNT_OVERFLOW;

                *recip_sum += (min / value);
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
            if ( !value )
                return ZERO_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *recip_sum += (min / value);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
                return DOUBLE_OVERFLOW;
            if ( !value )
                return ZERO_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *recip_sum += (min / value);
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( !value )
                return ZERO_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *recip_sum += (min / value);
        }
    }

    return 0;
}

static inline int get_recip_sum_tuple(PyObject* list, bool only_str, double min,
                                     double* recip_sum, uint64_t* count)
{
    Py_ssize_t tuple_len = PyTuple_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(list, i);

            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str ) return STR_ENCODING_FAILURE;

                double value = strtod(str, NULL);
                if ( !value )
                    return ZERO_VALUE;
                if ( (*count)++ == UINT64_MAX )
                    return COUNT_OVERFLOW;

                *recip_sum += (min / value);
            }
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tuple_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(list, i);

        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str ) return STR_ENCODING_FAILURE;

            double value = strtod(str, NULL);
            if ( !value )
                return ZERO_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *recip_sum += (min / value);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
                return DOUBLE_OVERFLOW;
            if ( !value )
                return ZERO_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *recip_sum += (min / value);
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( !value )
                return ZERO_VALUE;
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *recip_sum += (min / value);
        }
    }

    return 0;
}

static inline int get_recip_sum_iterable(PyObject* iter, bool only_str, double min,
                                        double* recip_sum, uint64_t* count)
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
                if ( !value )
                {
                    Py_DECREF(item); Py_DECREF(iter);
                    return ZERO_VALUE;
                }
                if ( (*count)++ == UINT64_MAX )
                {
                    Py_DECREF(item); Py_DECREF(iter);
                    return COUNT_OVERFLOW;
                }
                *recip_sum += (min / value);
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
            if ( !value )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return ZERO_VALUE;
            }
            if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
            }
            *recip_sum += (min / value);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return DOUBLE_OVERFLOW;
            }
            if ( !value )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return ZERO_VALUE;
            }
            if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
            }
            *recip_sum += (min / value);
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( !value )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return ZERO_VALUE;
            }
            if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
            }
            *recip_sum += (min / value);
        }
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static PyObject* hmean(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;

    PARSE_ARGS_AND_KWARGS;

    PyObject* py_float = get_min_method_ptr()(self, args, kwargs);
    if ( !py_float )
        return NULL;

    double min_val = PyFloat_AS_DOUBLE(py_float);
    if ( min_val <= 0.0 )
    {
        PyErr_SetString(PyExc_RuntimeError,
                        "The iterable contains a non-positive value.");
        return NULL;
    }

    double recip_sum = 0.0;
    uint64_t count = 0;
    int ret;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            ret = get_recip_sum_list(arg, is_only_str, min_val, &recip_sum, &count);
        }
        else if ( PyTuple_Check(arg) )
        {
            ret = get_recip_sum_tuple(arg, is_only_str, min_val, &recip_sum, &count);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            ret = get_recip_sum_iterable(iter, is_only_str, min_val,
                                         &recip_sum, &count);
        }
        HANDLE_SUM_AND_COUNT_RET(ret);
    }

    if ( !count )
        return PyFloat_FromDouble(0.0);

    return PyFloat_FromDouble((min_val*count)/recip_sum);
}
