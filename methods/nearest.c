#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "../numagr.h"

#define UPDATE_CUR_GAP_AND_VALUE_IF_STR(item)                   \
do                                                              \
{                                                               \
    if ( PyUnicode_Check(item) )                                \
    {                                                           \
        const char* str = PyUnicode_AsUTF8(item);               \
        if ( !str ) return STR_ENCODING_FAILURE;                \
                                                                \
        double value = strtod(str, NULL);                       \
        if ( value == target ) return EXACT_MATCH_FOUND;        \
                                                                \
        double distance = fabs(value - target);                 \
        if ( distance < *current_gap )                          \
        {                                                       \
            current_gap_val = distance;                         \
            nearest_value_val = value;                          \
        }                                                       \
    }                                                           \
                                                                \
} while (0);

#define UPDATE_CUR_GAP_AND_VALUE_IF_NUM(item)                   \
do                                                              \
{                                                               \
    if ( PyLong_Check(item) )                                   \
    {                                                           \
        double value = PyLong_AsDouble(item);                   \
        if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;         \
        if ( value == target ) return EXACT_MATCH_FOUND;        \
                                                                \
        double distance = fabs(value - target);                 \
        if ( distance < *current_gap )                          \
        {                                                       \
            current_gap_val = distance;                         \
            nearest_value_val = value;                          \
        }                                                       \
    }                                                           \
    else if ( PyFloat_Check(item) )                             \
    {                                                           \
        double value = PyFloat_AS_DOUBLE(item);                 \
        if ( value == target ) return EXACT_MATCH_FOUND;        \
                                                                \
        double distance = fabs(value - target);                 \
        if ( distance < *current_gap )                          \
        {                                                       \
            current_gap_val = distance;                         \
            nearest_value_val = value;                          \
        }                                                       \
    }                                                           \
                                                                \
} while (0);

#define iter_UPDATE_CUR_GAP_AND_VALUE_IF_STR(item)              \
do                                                              \
{                                                               \
    if ( PyUnicode_Check(item) )                                \
    {                                                           \
        const char* str = PyUnicode_AsUTF8(item);               \
        if ( !str )                                             \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return STR_ENCODING_FAILURE;                        \
        }                                                       \
                                                                \
        double value = strtod(str, NULL);                       \
        if ( value == target )                                  \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return EXACT_MATCH_FOUND;                           \
        }                                                       \
                                                                \
        double distance = fabs(value - target);                 \
        if ( distance < *current_gap )                          \
        {                                                       \
            current_gap_val = distance;                         \
            nearest_value_val = value;                          \
        }                                                       \
    }                                                           \
                                                                \
} while (0);

#define iter_UPDATE_CUR_GAP_AND_VALUE_IF_NUM(item)              \
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
        if ( value == target )                                  \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return EXACT_MATCH_FOUND;                           \
        }                                                       \
                                                                \
        double distance = fabs(value - target);                 \
        if ( distance < *current_gap )                          \
        {                                                       \
            current_gap_val = distance;                         \
            nearest_value_val = value;                          \
        }                                                       \
    }                                                           \
    else if ( PyFloat_Check(item) )                             \
    {                                                           \
        double value = PyFloat_AS_DOUBLE(item);                 \
        if ( value == target )                                  \
        {                                                       \
            Py_DECREF(item); Py_DECREF(iter);                   \
            return EXACT_MATCH_FOUND;                           \
        }                                                       \
                                                                \
        double distance = fabs(value - target);                 \
        if ( distance < *current_gap )                          \
        {                                                       \
            current_gap_val = distance;                         \
            nearest_value_val = value;                          \
        }                                                       \
    }                                                           \
                                                                \
} while (0);

static inline int search_list(PyObject* list, bool only_str, bool only_num,
                              double target, double* nearest_value,
                              double* current_gap)
{
    double nearest_value_val = *nearest_value;
    double current_gap_val = *current_gap;

    Py_ssize_t list_len = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            UPDATE_CUR_GAP_AND_VALUE_IF_STR(item);
        }
        *nearest_value = nearest_value_val;
        *current_gap = current_gap_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < list_len; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);
            UPDATE_CUR_GAP_AND_VALUE_IF_NUM(item);
        }
        *nearest_value = nearest_value_val;
        *current_gap = current_gap_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < list_len; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);
        UPDATE_CUR_GAP_AND_VALUE_IF_STR(item);
        UPDATE_CUR_GAP_AND_VALUE_IF_NUM(item);
    }

    *nearest_value = nearest_value_val;
    *current_gap = current_gap_val;
    return 0;
}

static inline int search_tuple(PyObject* tuple, bool only_str, bool only_num,
                               double target, double* nearest_value,
                               double* current_gap)
{
    double nearest_value_val = *nearest_value;
    double current_gap_val = *current_gap;

    Py_ssize_t tuple_len = PyTuple_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            UPDATE_CUR_GAP_AND_VALUE_IF_STR(item);
        }
        *nearest_value = nearest_value_val;
        *current_gap = current_gap_val;
        return 0;
    }

    if ( only_num )
    {
        for ( Py_ssize_t i = 0; i < tuple_len; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);
            UPDATE_CUR_GAP_AND_VALUE_IF_NUM(item);
        }
        *nearest_value = nearest_value_val;
        *current_gap = current_gap_val;
        return 0;
    }

    for ( Py_ssize_t i = 0; i < tuple_len; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);
        UPDATE_CUR_GAP_AND_VALUE_IF_STR(item);
        UPDATE_CUR_GAP_AND_VALUE_IF_NUM(item);
    }

    *nearest_value = nearest_value_val;
    *current_gap = current_gap_val;
    return 0;
}

static inline int search_iterable(PyObject* iter, bool only_str, bool only_num,
                                  double target, double* nearest_value,
                                  double* current_gap)
{
    double nearest_value_val = *nearest_value;
    double current_gap_val = *current_gap;

    PyObject* item;

    if ( only_str )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_UPDATE_CUR_GAP_AND_VALUE_IF_STR(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        *nearest_value = nearest_value_val;
        *current_gap = current_gap_val;
        return 0;
    }

    if ( only_num )
    {
        while ( (item = PyIter_Next(iter)) )
        {
            iter_UPDATE_CUR_GAP_AND_VALUE_IF_NUM(item);
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        *nearest_value = nearest_value_val;
        *current_gap = current_gap_val;
        return 0;
    }

    while ( (item = PyIter_Next(iter)) )
    {
        iter_UPDATE_CUR_GAP_AND_VALUE_IF_STR(item);
        iter_UPDATE_CUR_GAP_AND_VALUE_IF_NUM(item);
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    *nearest_value = nearest_value_val;
    *current_gap = current_gap_val;
    return 0;
}

static PyObject* nearest(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;
    bool is_only_num = false;
    double target;

    PARSE_TARGET_VALUE;
    PARSE_ARGS_AND_KWARGS;

    double nearest_value = INFINITY;
    double current_gap = INFINITY;
    int ret;

    for ( Py_ssize_t i = 1; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            ret = search_list(arg, is_only_str, is_only_num, target,
                              &nearest_value, &current_gap);
        }
        else if ( PyTuple_Check(arg) )
        {
            ret = search_tuple(arg, is_only_str, is_only_num, target,
                               &nearest_value, &current_gap);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            ret = search_iterable(iter, is_only_str, is_only_num, target,
                                  &nearest_value, &current_gap);
        }
        switch ( ret )
        {
            case                 0: continue;
            case EXACT_MATCH_FOUND: return PyFloat_FromDouble(target);
            default:                return NULL;
        }
    }

    return PyFloat_FromDouble(nearest_value);
}
