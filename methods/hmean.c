#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "../numagr.h"
#include "../floats_array.h"
#include "pyport.h"

#define ADD_TO_ARRAY_IF_STR(item)                           \
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
                                                            \
        int ret = 0;                                        \
        FDARRAY_PUSH(array, value, ret);                    \
        if ( ret )                                          \
            return ret;                                     \
                                                            \
        if ( value < min_value ) min_value = value;         \
    }                                                       \
                                                            \
} while (0);

#define ADD_TO_ARRAY_IF_NUM(item)                           \
do                                                          \
{                                                           \
    if ( PyLong_Check(item) )                               \
    {                                                       \
        double value = PyLong_AsDouble(item);               \
        if ( PyErr_Occurred() )                             \
            return DOUBLE_OVERFLOW;                         \
        if ( !value )                                       \
            return ZERO_VALUE;                              \
                                                            \
        int ret = 0;                                        \
        FDARRAY_PUSH(array, value, ret);                    \
        if ( ret )                                          \
            return ret;                                     \
                                                            \
        if ( value < min_value ) min_value = value;         \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( !value )                                       \
            return ZERO_VALUE;                              \
                                                            \
        int ret = 0;                                        \
        FDARRAY_PUSH(array, value, ret);                    \
        if ( ret )                                          \
            return ret;                                     \
                                                            \
        if ( value < min_value ) min_value = value;         \
    }                                                       \
                                                            \
} while (0);

#define iter_ADD_TO_ARRAY_IF_STR(item)                      \
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
                                                            \
        int ret = 0;                                        \
        FDARRAY_PUSH(array, value, ret);                    \
        if ( ret )                                          \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ret;                                     \
        }                                                   \
                                                            \
        if ( value < min_value ) min_value = value;         \
    }                                                       \
                                                            \
} while (0);

#define iter_ADD_TO_ARRAY_IF_NUM(item)                      \
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
                                                            \
        int ret = 0;                                        \
        FDARRAY_PUSH(array, value, ret);                    \
        if ( ret )                                          \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ret;                                     \
        }                                                   \
                                                            \
        if ( value < min_value ) min_value = value;         \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( !value )                                       \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ZERO_VALUE;                              \
        }                                                   \
                                                            \
        int ret = 0;                                        \
        FDARRAY_PUSH(array, value, ret);                    \
        if ( ret )                                          \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ret;                                     \
        }                                                   \
                                                            \
        if ( value < min_value ) min_value = value;         \
    }                                                       \
                                                            \
} while (0);

#define ALLOC_ARRAY(init_capacity)                          \
do                                                          \
{                                                           \
    array = fdarray_new(init_capacity);                     \
    if ( !array.addr )                                      \
    {                                                       \
        PyErr_SetString(PyExc_RuntimeError,                 \
                        "Memory allocation failure.");      \
        break;                                              \
    }                                                       \
    array_allocated = true;                                 \
                                                            \
} while (0);

#define recips_KAHAN_SUM_ADD(value, sum, correction)    \
do                                                      \
{                                                       \
    if ( !isfinite(sum) )                               \
    {                                                   \
        if ( isinf(sum) )                               \
        {                                               \
            sum += value;                               \
            break;                                      \
        }                                               \
        else if ( isnan(sum) )                          \
        {                                               \
            return PyFloat_FromDouble(NAN);             \
        }                                               \
    }                                                   \
    double next = (value) - correction;                 \
    double new_sum = sum + next;                        \
    correction = (new_sum - sum) - next;                \
    sum = new_sum;                                      \
                                                        \
} while (0);

static inline int add_to_array_list(PyObject* list, bool only_str, bool only_num,
                                    double* min, fdarray* array)
{
    double min_value = *min;

    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_TO_ARRAY_IF_STR(item);
        }
        *min = min_value;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_TO_ARRAY_IF_NUM(item);
        }
        *min = min_value;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);
        ADD_TO_ARRAY_IF_STR(item);
        ADD_TO_ARRAY_IF_NUM(item);
    }

    *min = min_value;
    return 0;
}

static inline int add_to_array_tuple(PyObject* tuple, bool only_str, bool only_num,
                                     double* min, fdarray* array)
{
    double min_value = *min;

    Py_ssize_t tuple_len = PyTuple_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            ADD_TO_ARRAY_IF_STR(item);
        }
        *min = min_value;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            ADD_TO_ARRAY_IF_NUM(item);
        }
        *min = min_value;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tuple_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);
        ADD_TO_ARRAY_IF_STR(item);
        ADD_TO_ARRAY_IF_NUM(item);
    }

    *min = min_value;
    return 0;
}

static inline int add_to_array_iterable(PyObject* iter, bool only_str, bool only_num,
                                        double* min, fdarray* array)
{
    double min_value = *min;

    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_TO_ARRAY_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        *min = min_value;
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_TO_ARRAY_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);
        *min = min_value;
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_ADD_TO_ARRAY_IF_STR(item);
        iter_ADD_TO_ARRAY_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    *min = min_value;
    return 0;
}

static PyObject* hmean(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);

    bool is_only_str = false;
    bool is_only_num = false;

    PARSE_ARGS_AND_KWARGS;

    fdarray array;
    bool array_allocated = false;

    double min = INFINITY;
    int ret;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            if ( !array_allocated )
            {
                Py_ssize_t size = PyList_GET_SIZE(arg);
                size = (size ? size : 1024);
                ALLOC_ARRAY(size);
                if ( !array_allocated ) return NULL;
            }

            ret = add_to_array_list(arg, is_only_str, is_only_num, &min, &array);
        }
        else if ( PyTuple_Check(arg) )
        {
            if ( !array_allocated )
            {
                Py_ssize_t size = PyTuple_GET_SIZE(arg);
                size = (size ? size : 1024);
                ALLOC_ARRAY(size);
                if ( !array_allocated ) return NULL;
            }

            ret = add_to_array_tuple(arg, is_only_str, is_only_num, &min, &array);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            if ( !array_allocated )
            {
                Py_ssize_t size = PyObject_LengthHint(iter, 1024);
                ALLOC_ARRAY(size);
                if ( !array_allocated )
                {
                    Py_DECREF(iter);
                    return NULL;
                }
            }

            ret = add_to_array_iterable(iter, is_only_str, is_only_num, &min, &array);
        }
        if ( ret ) free(array.addr);
        HANDLE_RETURN_CODES(ret);
    }


    size_t count = array.count;
    if ( !count )
    {
        free(array.addr);
        return PyFloat_FromDouble(NAN);
    }

    if ( min < 0.0 ) // 0 is already handled with return ZERO_VALUE
    {
        PyErr_SetString(NegativeValue,
                        "The iterable contains a negative value.");
        free(array.addr);
        return NULL;
    }

    double* array_addr = array.addr;

    double recip_sum = 0.0;
    double correction = 0.0;
    for ( size_t i = 0; i < count; i++ )
    {
        double value = min / array_addr[i];
        recips_KAHAN_SUM_ADD(value, recip_sum, correction); // will return with NaN
    }                                                       // if encountered
    free(array.addr);

    return PyFloat_FromDouble((min*count)/recip_sum);
}
