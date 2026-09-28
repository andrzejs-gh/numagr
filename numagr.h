#ifndef NUMAGR_H
#define NUMAGR_H

#include <Python.h>

#define PARSE_ARGS_AND_KWARGS                                                       \
do                                                                                  \
{                                                                                   \
    if ( !argc )                                                                    \
    {                                                                               \
        PyErr_SetString(PyExc_TypeError,                                            \
        "Expected at least one positional argument.");                              \
        return NULL;                                                                \
    }                                                                               \
                                                                                    \
    if ( !kwargs )                                                                  \
        break;                                                                      \
                                                                                    \
        PyObject* key;                                                              \
        PyObject* value;                                                            \
        Py_ssize_t pos = 0;                                                         \
        bool only_str_found = false;                                                \
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
                    PyErr_SetString                                                 \
                    (                                                               \
                    PyExc_TypeError,                                                \
                    "Invalid keyword argument value, "                              \
                    "\"only_str\" value must evaluate to either True or False"      \
                    );                                                              \
                    return NULL;                                                    \
                }                                                                   \
                is_only_str = ret;                                                  \
                only_str_found = true;                                              \
            }                                                                       \
            else                                                                    \
            {                                                                       \
                PyErr_SetString                                                     \
                (                                                                   \
                PyExc_TypeError,                                                    \
                "Invalid keyword argument name. "                                   \
                "The only allowed kwarg is: \"only_str\"."                          \
                );                                                                  \
                return NULL;                                                        \
            }                                                                       \
        }                                                                           \
                                                                                    \
} while (0);

#define KAHAN_SUM(sum_arg_name, value)                  \
do                                                      \
{                                                       \
    double old_sum = *(sum_arg_name);                   \
    double next = (value) - *correction;                \
    double new_sum = old_sum + next;                    \
    *correction = (new_sum - old_sum) - next;           \
    *(sum_arg_name) = new_sum;                          \
                                                        \
} while (0);

#define HANDLE_SUM_AND_COUNT_RET(return_code)                               \
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

enum
{
    FAILED_TO_GET_ITER = 1,
    STR_ENCODING_FAILURE,
    DOUBLE_OVERFLOW,
    COUNT_OVERFLOW,
    EXACT_MATCH_FOUND,
    ZERO_VALUE,
    NEGATIVE_VALUE,
    INVALID_VALUE
};

typedef PyObject* (*numagr_method)(PyObject*, PyObject*, PyObject*);

numagr_method get_min_method_ptr(void);

#endif
