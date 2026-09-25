#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include "numagr.h"

#define HANDLE_SUM_AND_COUNT_RET(return_code)                       \
switch ( return_code )                                              \
{                                                                   \
    case 0:                                                         \
        continue;                                                   \
    case STR_ENCODING_FAILURE:                                      \
        return NULL;                                                \
    case COUNT_OVERFLOW:                                            \
        PyErr_SetString(PyExc_RuntimeError,                         \
                        "List contains too many elements");         \
        return NULL;                                                \
    case DOUBLE_OVERFLOW:                                           \
        return NULL;                                                \
}

static inline int sum_and_count_iterable(PyObject* iter, bool only_str,
                                         double* sum, uint64_t* count)
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
                else if ( (*count)++ == UINT64_MAX )
                {
                    Py_DECREF(item); Py_DECREF(iter);
                    return COUNT_OVERFLOW;
                }
                *sum += strtod(str, NULL);
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
            else if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
            }
            *sum += strtod(str, NULL);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return DOUBLE_OVERFLOW;
            }
            else if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
            }
            *sum += value;
        }
        else if ( PyFloat_Check(item) )
        {
            if ( (*count)++ == UINT64_MAX )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return COUNT_OVERFLOW;
            }
            *sum += PyFloat_AS_DOUBLE(item);
        }
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static inline int sum_and_count_list(PyObject* list, bool only_str,
                                      double* sum, uint64_t* count)
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
                if ( !str )
                    return STR_ENCODING_FAILURE;
                else if ( (*count)++ == UINT64_MAX )
                    return COUNT_OVERFLOW;

                *sum += strtod(str, NULL);
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
            if ( !str )
                return STR_ENCODING_FAILURE;
            else if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *sum += strtod(str, NULL);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
                return DOUBLE_OVERFLOW;
            else if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *sum += value;
        }
        else if ( PyFloat_Check(item) )
        {
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;
            *sum += PyFloat_AS_DOUBLE(item);
        }
    }

    return 0;
}

static inline int sum_and_count_tuple(PyObject* list, bool only_str,
                                        double* sum, uint64_t* count)
{
    Py_ssize_t tupl_len = PyTuple_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tupl_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(list, i);

            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str )
                    return STR_ENCODING_FAILURE;
                else if ( (*count)++ == UINT64_MAX )
                    return COUNT_OVERFLOW;

                *sum += strtod(str, NULL);
            }
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tupl_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(list, i);

        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str )
                return STR_ENCODING_FAILURE;
            else if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *sum += strtod(str, NULL);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
                return DOUBLE_OVERFLOW;
            else if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;

            *sum += value;
        }
        else if ( PyFloat_Check(item) )
        {
            if ( (*count)++ == UINT64_MAX )
                return COUNT_OVERFLOW;
            *sum += PyFloat_AS_DOUBLE(item);
        }
    }

    return 0;
}

static PyObject* amean(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;

    PARSE_ARGS_AND_KWARGS;

    double sum = 0.0;
    uint64_t count = 0;
    int ret;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            ret = sum_and_count_list(arg, is_only_str, &sum, &count);
            HANDLE_SUM_AND_COUNT_RET(ret);
        }
        else if ( PyTuple_Check(arg) )
        {
            ret = sum_and_count_tuple(arg, is_only_str, &sum, &count);
            HANDLE_SUM_AND_COUNT_RET(ret);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            ret = sum_and_count_iterable(iter, is_only_str, &sum, &count);
            HANDLE_SUM_AND_COUNT_RET(ret);
        }
    }

    return PyFloat_FromDouble(sum);
}
