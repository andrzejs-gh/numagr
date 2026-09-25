#include <Python.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "floatobject.h"
#include "numagr.h"
#include "pyerrors.h"

#define PARSE_TARGET_VALUE                                              \
do                                                                      \
{                                                                       \
    if ( argc < 2 )                                                     \
    {                                                                   \
        PyErr_SetString                                                 \
        (                                                               \
            PyExc_TypeError,                                            \
            "Missing positional arguments. "                            \
            "Enter the target value followed by iterable(s)."           \
        );                                                              \
        return NULL;                                                    \
    }                                                                   \
                                                                        \
    PyObject* arg = PyTuple_GET_ITEM(args, 0);                          \
    if ( PyLong_Check(arg) )                                            \
    {                                                                   \
        target = PyLong_AsDouble(arg);                                  \
        if ( PyErr_Occurred() ) return NULL;                            \
    }                                                                   \
    else if ( PyFloat_Check(arg) )                                      \
    {                                                                   \
        target = PyFloat_AsDouble(arg);                                 \
        if ( PyErr_Occurred() ) return NULL;                            \
    }                                                                   \
    else                                                                \
    {                                                                   \
        PyErr_SetString                                                 \
        (                                                               \
            PyExc_TypeError,                                            \
            "Invalid target value. Must be either int or float."        \
        );                                                              \
        return NULL;                                                    \
    }                                                                   \
                                                                        \
} while (0);

static inline int search_list(PyObject* list, bool only_str, double target,
                              double* nearest_value, double* current_gap)
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
                if ( value == target ) return EXACT_MATCH_FOUND;

                double distance = fabs(value - target);
                if ( distance < *current_gap )
                {
                    *current_gap = distance;
                    *nearest_value = value;
                }
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
            if ( value == target ) return EXACT_MATCH_FOUND;

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;
            if ( value == target ) return EXACT_MATCH_FOUND;

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value == target ) return EXACT_MATCH_FOUND;

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
    }

    return 0;
}

static inline int search_tuple(PyObject* tuple, bool only_str, double target,
                               double* nearest_value, double* current_gap)
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
                if ( value == target ) return EXACT_MATCH_FOUND;

                double distance = fabs(value - target);
                if ( distance < *current_gap )
                {
                    *current_gap = distance;
                    *nearest_value = value;
                }
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
            if ( value == target ) return EXACT_MATCH_FOUND;

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;
            if ( value == target ) return EXACT_MATCH_FOUND;

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value == target ) return EXACT_MATCH_FOUND;

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
    }

    return 0;
}

static inline int search_iterable(PyObject* iter, bool only_str, double target,
                                  double* nearest_value, double* current_gap)
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
                if ( value == target )
                {
                    Py_DECREF(item); Py_DECREF(iter);
                    return EXACT_MATCH_FOUND;
                }

                double distance = fabs(value - target);
                if ( distance < *current_gap )
                {
                    *current_gap = distance;
                    *nearest_value = value;
                }
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
            if ( value == target )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return EXACT_MATCH_FOUND;
            }

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return DOUBLE_OVERFLOW;
            }
            if ( value == target )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return EXACT_MATCH_FOUND;
            }

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
        else if ( PyFloat_Check(item) )
        {
            double value = PyFloat_AS_DOUBLE(item);
            if ( value == target )
            {
                Py_DECREF(item); Py_DECREF(iter);
                return EXACT_MATCH_FOUND;
            }

            double distance = fabs(value - target);
            if ( distance < *current_gap )
            {
                *current_gap = distance;
                *nearest_value = value;
            }
        }
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static PyObject* near(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;
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
            ret = search_list(arg, is_only_str, target,
                              &nearest_value, &current_gap);
        }
        else if ( PyTuple_Check(arg) )
        {
            ret = search_tuple(arg, is_only_str, target,
                               &nearest_value, &current_gap);
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            ret = search_iterable(iter, is_only_str, target,
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
