#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "../numagr.h"

#define ADD_LOG_IF_STR(item)                                \
do                                                          \
{                                                           \
    if ( PyUnicode_Check(item) )                            \
    {                                                       \
        const char* str = PyUnicode_AsUTF8(item);           \
        if ( !str ) return STR_ENCODING_FAILURE;            \
                                                            \
        double value = strtod(str, NULL);                   \
        if ( value <= 0.0 )                                 \
        {                                                   \
            SET_INV_STR_ERROR;                              \
            return INVALID_VALUE;                           \
        }                                                   \
        if ( value == INFINITY )                            \
        {                                                   \
            sum_val += value;                               \
            break;                                          \
        }                                                   \
        if ( count_val++ == UINT64_MAX )                    \
            return COUNT_OVERFLOW;                          \
                                                            \
        value = log(value);                                 \
        KAHAN_SUM_ADD(value, sum_val, correction_val);      \
    }                                                       \
                                                            \
} while (0);

#define ADD_LOG_IF_NUM(item)                                \
do                                                          \
{                                                           \
    if ( PyLong_Check(item) )                               \
    {                                                       \
        double value = PyLong_AsDouble(item);               \
        if ( PyErr_Occurred() )                             \
            return DOUBLE_OVERFLOW;                         \
        if ( value < 0.0 )                                  \
            return NEGATIVE_VALUE;                          \
        if ( value == INFINITY )                            \
        {                                                   \
            sum_val += value;                               \
            break;                                          \
        }                                                   \
        if ( count_val++ == UINT64_MAX )                    \
            return COUNT_OVERFLOW;                          \
                                                            \
        value = log(value);                                 \
        KAHAN_SUM_ADD(value, sum_val, correction_val);      \
                                                            \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
        if ( value < 0.0 )                                  \
            return NEGATIVE_VALUE;                          \
        if ( value == INFINITY )                            \
        {                                                   \
            sum_val += value;                               \
            break;                                          \
        }                                                   \
        if ( count_val++ == UINT64_MAX )                    \
            return COUNT_OVERFLOW;                          \
                                                            \
        value = log(value);                                 \
        KAHAN_SUM_ADD(value, sum_val, correction_val);      \
    }                                                       \
                                                            \
} while (0);

#define iter_ADD_LOG_IF_STR(item)                           \
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
        if ( value <= 0.0 )                                 \
        {                                                   \
            SET_INV_STR_ERROR;                              \
            Py_DECREF(item); Py_DECREF(iter);               \
            return INVALID_VALUE;                           \
        }                                                   \
        if ( value == INFINITY )                            \
        {                                                   \
            sum_val += value;                               \
            Py_DECREF(item); Py_DECREF(iter);               \
            break;                                          \
        }                                                   \
        if ( count_val++ == UINT64_MAX )                    \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return COUNT_OVERFLOW;                          \
        }                                                   \
                                                            \
        value = log(value);                                 \
        KAHAN_SUM_ADD(value, sum_val, correction_val);      \
    }                                                       \
                                                            \
} while (0);

#define iter_ADD_LOG_IF_NUM(item)                           \
do                                                          \
{                                                           \
    if ( PyLong_Check(item) )                               \
    {                                                       \
        double value = PyLong_AsDouble(item);               \
                                                            \
        if ( PyErr_Occurred() )                             \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return DOUBLE_OVERFLOW;                         \
        }                                                   \
        if ( value < 0.0 )                                  \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return NEGATIVE_VALUE;                          \
        }                                                   \
        if ( value == INFINITY )                            \
        {                                                   \
            sum_val += value;                               \
            Py_DECREF(item); Py_DECREF(iter);               \
            break;                                          \
        }                                                   \
        if ( count_val++ == UINT64_MAX )                    \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return COUNT_OVERFLOW;                          \
        }                                                   \
                                                            \
        value = log(value);                                 \
        KAHAN_SUM_ADD(value, sum_val, correction_val);      \
    }                                                       \
    else if ( PyFloat_Check(item) )                         \
    {                                                       \
        double value = PyFloat_AS_DOUBLE(item);             \
                                                            \
        if ( value < 0.0 )                                  \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return NEGATIVE_VALUE;                          \
        }                                                   \
        if ( value == INFINITY )                            \
        {                                                   \
            sum_val += value;                               \
            Py_DECREF(item); Py_DECREF(iter);               \
            break;                                          \
        }                                                   \
        if ( count_val++ == UINT64_MAX )                    \
        {                                                   \
            Py_DECREF(item); Py_DECREF(iter);               \
            return COUNT_OVERFLOW;                          \
        }                                                   \
                                                            \
        value = log(value);                                 \
        KAHAN_SUM_ADD(value, sum_val, correction_val);      \
    }                                                       \
                                                            \
} while (0);

static inline int get_log_sum_list(PyObject* list, bool only_str, bool only_num,
                                   double* log_sum, double* correction,
                                   uint64_t* count)
{
    DEREFERENCE(log_sum, correction);
    uint64_t count_val = *count;

    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_LOG_IF_STR(item);
        }
        REASSIGN(log_sum, correction);
        *count = count_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            ADD_LOG_IF_NUM(item);
        }
        REASSIGN(log_sum, correction);
        *count = count_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);
        ADD_LOG_IF_STR(item);
        ADD_LOG_IF_NUM(item);
    }

    REASSIGN(log_sum, correction);
    *count = count_val;
    return 0;
}

static inline int get_log_sum_tuple(PyObject* tuple, bool only_str, bool only_num,
                                    double* log_sum, double* correction,
                                    uint64_t* count)
{
    DEREFERENCE(log_sum, correction);
    uint64_t count_val = *count;

    Py_ssize_t tuple_len = PyTuple_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            ADD_LOG_IF_STR(item);
        }
        REASSIGN(log_sum, correction);
        *count = count_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            ADD_LOG_IF_NUM(item);
        }
        REASSIGN(log_sum, correction);
        *count = count_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tuple_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);
        ADD_LOG_IF_STR(item);
        ADD_LOG_IF_NUM(item);
    }

    REASSIGN(log_sum, correction);
    *count = count_val;
    return 0;
}

static inline int get_log_sum_iterable(PyObject* iter, bool only_str, bool only_num,
                                       double* log_sum, double* correction,
                                       uint64_t* count)
{
    DEREFERENCE(log_sum, correction);
    uint64_t count_val = *count;

    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_LOG_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        REASSIGN(log_sum, correction);
        *count = count_val;
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_ADD_LOG_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        REASSIGN(log_sum, correction);
        *count = count_val;
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_ADD_LOG_IF_STR(item);
        iter_ADD_LOG_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    REASSIGN(log_sum, correction);
    *count = count_val;
    return 0;
}

static PyObject* gmean(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;
    bool is_only_num = false;

    PARSE_ARGS_AND_KWARGS;

    double log_sum = 0.0;
    double correction = 0.0;
    uint64_t count = 0;
    int ret;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            ret = get_log_sum_list(arg, is_only_str, is_only_num,
                                   &log_sum, &correction, &count);
        }
        else if ( PyTuple_Check(arg) )
        {
            ret = get_log_sum_tuple(arg, is_only_str, is_only_num,
                                    &log_sum, &correction, &count);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            ret = get_log_sum_iterable(iter, is_only_str, is_only_num,
                                       &log_sum, &correction, &count);
        }
        HANDLE_RETURN_CODES(ret);
    }

    if ( !count )
        return PyFloat_FromDouble(NAN);

    return PyFloat_FromDouble(exp(log_sum / count));
}
