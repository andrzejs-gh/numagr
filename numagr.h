#ifndef NUMAGR_H
#define NUMAGR_H

#include "floats_array.h"
#include <Python.h>

#define SET_INV_KWARG_VAL_ERR(kwarg_name)                           \
PyErr_Format                                                        \
(                                                                   \
    PyExc_TypeError,                                                \
    "Invalid keyword argument value, "                              \
    "\"%s\" value must evaluate to either True or False",           \
    (kwarg_name)                                                    \
);

#define PARSE_ARGS_AND_KWARGS                                                   \
do                                                                              \
{                                                                               \
    if ( !argc )                                                                \
    {                                                                           \
        PyErr_SetString(PyExc_TypeError,                                        \
        "Expected at least one positional argument.");                          \
        return NULL;                                                            \
    }                                                                           \
                                                                                \
    if ( !kwargs )                                                              \
        break;                                                                  \
                                                                                \
    PyObject* key;                                                              \
    PyObject* value;                                                            \
    Py_ssize_t pos = 0;                                                         \
    bool only_str_found = false;                                                \
    bool only_num_found = false;                                                \
                                                                                \
    while ( PyDict_Next(kwargs, &pos, &key, &value) )                           \
    {                                                                           \
        const char* kwarg_name = PyUnicode_AsUTF8(key);                         \
        if ( !kwarg_name )                                                      \
        {                                                                       \
            PyErr_SetString(PyExc_TypeError,                                    \
            "Keyword name UTF8-encoding failure.");                             \
            return NULL;                                                        \
        }                                                                       \
                                                                                \
        if ( !only_str_found && !strcmp(kwarg_name, "only_str") )               \
        {                                                                       \
            int ret = PyObject_IsTrue(value);                                   \
            if ( ret < 0 )                                                      \
            {                                                                   \
                SET_INV_KWARG_VAL_ERR(kwarg_name);                              \
                return NULL;                                                    \
            }                                                                   \
            is_only_str = ret;                                                  \
            only_str_found = true;                                              \
        }                                                                       \
        else if ( !only_num_found && !strcmp(kwarg_name, "only_num") )          \
        {                                                                       \
            int ret = PyObject_IsTrue(value);                                   \
            if ( ret < 0 )                                                      \
            {                                                                   \
                SET_INV_KWARG_VAL_ERR(kwarg_name);                              \
                return NULL;                                                    \
            }                                                                   \
            is_only_num = ret;                                                  \
            only_num_found = true;                                              \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            PyErr_Format                                                        \
            (                                                                   \
                PyExc_TypeError,                                                \
                "Invalid keyword argument name. \"%s\".",                       \
                kwarg_name                                                      \
            );                                                                  \
            return NULL;                                                        \
        }                                                                       \
    }                                                                           \
                                                                                \
    if ( is_only_str && is_only_num )                                           \
    {                                                                           \
        PyErr_SetString                                                         \
        (                                                                       \
            PyExc_TypeError,                                                    \
            "Keyword arguments \"str_only\" and \"num_only\" "                  \
            "are mutually exclusive."                                           \
        );                                                                      \
        return NULL;                                                            \
    }                                                                           \
                                                                                \
} while (0);

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

#define KAHAN_SUM(sum_ptr, correction_ptr, value)       \
do                                                      \
{                                                       \
    double old_sum = *(sum_ptr);                        \
    double next = (value) - *correction_ptr;            \
    double new_sum = old_sum + next;                    \
    *correction_ptr = (new_sum - old_sum) - next;       \
    *(sum_ptr) = new_sum;                               \
                                                        \
} while (0);

#define HANDLE_RETURN_CODES(return_code)                                    \
switch ( return_code )                                                      \
{                                                                           \
    case 0:                                                                 \
        continue;                                                           \
                                                                            \
    case COUNT_OVERFLOW:                                                    \
        PyErr_SetString(PyExc_RuntimeError,                                 \
                        "The iterable contains too many elements.");        \
        return NULL;                                                        \
                                                                            \
    case ZERO_VALUE:                                                        \
        PyErr_SetString(PyExc_RuntimeError,                                 \
                        "The iterable contains a zero value.");             \
        return NULL;                                                        \
                                                                            \
    case NEGATIVE_VALUE:                                                    \
        PyErr_SetString(PyExc_RuntimeError,                                 \
                        "The iterable contains a negative value.");         \
        return NULL;                                                        \
                                                                            \
    case ZERO_IN_ITERABLE:                                                  \
        return PyFloat_FromDouble(0.0);                                     \
                                                                            \
    case ARR_SIZE_OVERFLOW:                                                 \
        PyErr_SetString(PyExc_RuntimeError,                                 \
                        "The iterable contains too many elements.");        \
        return NULL;                                                        \
                                                                            \
    case ARR_REALLOC_FAILURE:                                               \
        PyErr_SetString(PyExc_RuntimeError,                                 \
                        "Memory reallocation failure.");                    \
        return NULL;                                                        \
                                                                            \
    default:                                                                \
        return NULL;                                                        \
}

#define SET_INV_STR_ERROR                                               \
PyErr_Format                                                            \
(                                                                       \
    PyExc_RuntimeError,                                                 \
    "The iterable contains a str that "                                 \
    "cannot be evaluated to a positive value: \"%s\". \n"               \
    "For geometric mean, all strings in the iterable "                  \
    "must evaluate to positive values.",                                \
    str                                                                 \
);

#define FDARRAY_PUSH(array, value, ret)                                             \
do                                                                                  \
{                                                                                   \
    size_t count = array->count;                                                    \
                                                                                    \
    if ( count == SIZE_MAX )                                                        \
    {                                                                               \
        free(array->addr);                                                          \
        ret = ARR_SIZE_OVERFLOW;                                                    \
        break;                                                                      \
    }                                                                               \
                                                                                    \
    if ( count == array->capacity )                                                 \
    {                                                                               \
        size_t new_capacity = 2*array->capacity*sizeof(double);                     \
                                                                                    \
        double* new_addr = realloc(array->addr, new_capacity);                      \
        if ( !new_addr )                                                            \
        {                                                                           \
            free(array->addr);                                                      \
            ret = ARR_REALLOC_FAILURE;                                              \
            break;                                                                  \
        }                                                                           \
                                                                                    \
        array->addr = new_addr;                                                     \
        array->capacity = new_capacity;                                             \
    }                                                                               \
    array->addr[count] = value;                                                     \
    array->count++;                                                                 \
                                                                                    \
} while (0);

enum
{
    FAILED_TO_GET_ITER = 1,
    STR_ENCODING_FAILURE,
    DOUBLE_OVERFLOW,
    COUNT_OVERFLOW,
    EXACT_MATCH_FOUND,
    ZERO_VALUE,
    NEGATIVE_VALUE,
    INVALID_VALUE,
    ZERO_IN_ITERABLE
};

typedef PyObject* (*numagr_method)(PyObject*, PyObject*, PyObject*);

numagr_method get_min_method_ptr(void);
numagr_method get_amean_method_ptr(void);

#endif
