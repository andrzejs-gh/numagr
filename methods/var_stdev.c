#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "../numagr.h"
#include "../floats_array.h"

#define iter_ADD_AND_PUSH_IF_STR(item)                      \
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
        KAHAN_SUM_ADD(value, sum_val, correction_val);      \
                                                            \
        int ret = 0;                                        \
        FDARRAY_PUSH(array, value, ret);                    \
        if ( ret )                                          \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return ret;                                     \
        }                                                   \
    }                                                       \
                                                            \
} while (0);

#define iter_ADD_AND_PUSH_IF_NUM(item)                          \
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
                                                                \
        KAHAN_SUM_ADD(value, sum_val, correction_val);          \
                                                                \
        int ret = 0;                                            \
        FDARRAY_PUSH(array, value, ret);                        \
        if ( ret )                                              \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return ret;                                         \
        }                                                       \
    }                                                           \
    else if ( PyFloat_Check(item) )                             \
    {                                                           \
        double value = PyFloat_AS_DOUBLE(item);                 \
                                                                \
        KAHAN_SUM_ADD(value, sum_val, correction_val);          \
                                                                \
        int ret = 0;                                            \
        FDARRAY_PUSH(array, value, ret);                        \
        if ( ret )                                              \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return ret;                                         \
        }                                                       \
    }                                                           \
                                                                \
} while (0);

#define ADD_AND_PUSH_IF_STR(item)                           \
do                                                          \
{                                                           \
    if ( PyUnicode_Check(item) )                            \
    {                                                       \
        const char* str = PyUnicode_AsUTF8(item);           \
        if ( !str )                                         \
            return STR_ENCODING_FAILURE;                    \
                                                            \
        double value = strtod(str, NULL);                   \
        KAHAN_SUM_ADD(value, sum_val, correction_val);      \
                                                            \
        int ret = 0;                                        \
        FDARRAY_PUSH(array, value, ret);                    \
        if ( ret ) return ret;                              \
    }                                                       \
                                                            \
} while (0);

#define ADD_AND_PUSH_IF_NUM(item)                               \
do                                                              \
{                                                               \
    if ( PyLong_Check(item) )                                   \
    {                                                           \
        double value = PyLong_AsDouble(item);                   \
        if ( PyErr_Occurred() )                                 \
            return DOUBLE_OVERFLOW;                             \
                                                                \
        KAHAN_SUM_ADD(value, sum_val, correction_val);          \
                                                                \
        int ret = 0;                                            \
        FDARRAY_PUSH(array, value, ret);                        \
        if ( ret ) return ret;                                  \
                                                                \
    }                                                           \
    else if ( PyFloat_Check(item) )                             \
    {                                                           \
        double value = PyFloat_AS_DOUBLE(item);                 \
        KAHAN_SUM_ADD(value, sum_val, correction_val);          \
                                                                \
        int ret = 0;                                            \
        FDARRAY_PUSH(array, value, ret);                        \
        if ( ret ) return ret;                                  \
    }                                                           \
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

#define GET_DEVS_SUM_SQUARED                                        \
do                                                                  \
{                                                                   \
    double sum = 0.0;                                               \
    double correction = 0.0;                                        \
                                                                    \
    Py_ssize_t argc = PyTuple_GET_SIZE(args);                       \
                                                                    \
    bool is_only_str = false;                                       \
    bool is_only_num = false;                                       \
                                                                    \
    PARSE_ARGS_AND_KWARGS;                                          \
                                                                    \
    fdarray array;                                                  \
    bool array_allocated = false;                                   \
    int ret;                                                        \
                                                                    \
    for ( Py_ssize_t i = 0; i < argc; i++ )                         \
    {                                                               \
        PyObject* arg = PyTuple_GET_ITEM(args, i);                  \
                                                                    \
        if ( PyList_Check(arg) )                                    \
        {                                                           \
            if ( !array_allocated )                                 \
            {                                                       \
                Py_ssize_t size = PyList_GET_SIZE(arg);             \
                size = (size ? size : 1024);                        \
                ALLOC_ARRAY(size);                                  \
                if ( !array_allocated ) return NULL;                \
            }                                                       \
                                                                    \
            ret = add_and_push_list(arg, is_only_str, is_only_num,  \
                                    &sum, &correction, &array);     \
        }                                                           \
        else if ( PyTuple_Check(arg) )                              \
        {                                                           \
            if ( !array_allocated )                                 \
            {                                                       \
                Py_ssize_t size = PyTuple_GET_SIZE(arg);             \
                size = (size ? size : 1024);                        \
                ALLOC_ARRAY(size);                                  \
                if ( !array_allocated ) return NULL;                \
            }                                                       \
                                                                    \
            ret = add_and_push_tuple(arg, is_only_str, is_only_num, \
                                     &sum, &correction, &array);    \
        }                                                           \
        else                                                        \
        {                                                               \
            PyObject* iter = PyObject_GetIter(arg);                     \
            if ( !iter ) return NULL;                                   \
                                                                        \
            if ( !array_allocated )                                     \
            {                                                           \
                Py_ssize_t size = PyObject_LengthHint(iter, 1024);      \
                ALLOC_ARRAY(size);                                      \
                if ( !array_allocated )                                 \
                {                                                       \
                    Py_DECREF(iter);                                    \
                    return NULL;                                        \
                }                                                       \
            }                                                           \
                                                                        \
            ret = add_and_push_iterable(iter, is_only_str, is_only_num, \
                                        &sum, &correction, &array);     \
        }                                                               \
        if ( ret ) free(array.addr);                                    \
        HANDLE_RETURN_CODES(ret);                                       \
    }                                                                   \
                                                                        \
    count = array.count;                                                \
    if ( !count  || isinf(sum) )                                        \
    {                                                                   \
        free(array.addr);                                               \
        return PyFloat_FromDouble(NAN);                                 \
    }                                                                   \
                                                                        \
    double a_mean = sum / count;                                        \
    double* array_addr = array.addr;                                    \
    correction = 0.0;                                                   \
                                                                        \
    for ( size_t i = 0; i < count; i++ )                                \
    {                                                                   \
        double deviation = array_addr[i] - a_mean;                      \
        double old_sum = devs_sum_sq;                                   \
        double next = deviation*deviation - correction;                 \
        double new_sum = old_sum + next;                                \
        correction = (new_sum - old_sum) - next;                        \
        devs_sum_sq = new_sum;                                          \
    }                                                                   \
    free(array.addr);                                                   \
                                                                        \
} while (0);

static inline int add_and_push_list(PyObject* list, bool only_str, bool only_num,
                                    double* sum, double* correction, fdarray* array)
{
    DEREFERENCE(sum, correction);

    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_AND_PUSH_IF_STR(item);
        }
        REASSIGN(sum, correction);
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_AND_PUSH_IF_NUM(item);
        }
        REASSIGN(sum, correction);
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);
        ADD_AND_PUSH_IF_STR(item);
        ADD_AND_PUSH_IF_NUM(item);
    }

    REASSIGN(sum, correction);
    return 0;
}

static inline int add_and_push_tuple(PyObject* tuple, bool only_str, bool only_num,
                                     double* sum, double* correction, fdarray* array)
{
    DEREFERENCE(sum, correction);

    Py_ssize_t tuple_len = PyTuple_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            ADD_AND_PUSH_IF_STR(item);
        }
        REASSIGN(sum, correction);
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            ADD_AND_PUSH_IF_NUM(item);
        }
        REASSIGN(sum, correction);
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tuple_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);
        ADD_AND_PUSH_IF_STR(item);
        ADD_AND_PUSH_IF_NUM(item);
    }

    REASSIGN(sum, correction);
    return 0;
}

static inline int add_and_push_iterable(PyObject* iter, bool only_str, bool only_num,
                                        double* sum, double* correction,
                                        fdarray* array)
{
    DEREFERENCE(sum, correction);

    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_AND_PUSH_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        REASSIGN(sum, correction);
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_AND_PUSH_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        REASSIGN(sum, correction);
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_ADD_AND_PUSH_IF_STR(item);
        iter_ADD_AND_PUSH_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    REASSIGN(sum, correction);
    return 0;
}

static PyObject* variance(PyObject* self, PyObject* args, PyObject* kwargs)
{
    double devs_sum_sq = 0.0;
    double count;

    GET_DEVS_SUM_SQUARED;

    return PyFloat_FromDouble( devs_sum_sq / (count - 1) );
}

static PyObject* pvariance(PyObject* self, PyObject* args, PyObject* kwargs)
{
    double devs_sum_sq = 0.0;
    double count;

    GET_DEVS_SUM_SQUARED;

    return PyFloat_FromDouble( devs_sum_sq / count );
}

static PyObject* stdev(PyObject* self, PyObject* args, PyObject* kwargs)
{
    double devs_sum_sq = 0.0;
    double count;

    GET_DEVS_SUM_SQUARED;

    return PyFloat_FromDouble( sqrt(devs_sum_sq / (count - 1)) );
}

static PyObject* pstdev(PyObject* self, PyObject* args, PyObject* kwargs)
{
    double devs_sum_sq = 0.0;
    double count;

    GET_DEVS_SUM_SQUARED;

    return PyFloat_FromDouble( sqrt(devs_sum_sq / count) );
}
