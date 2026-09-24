#include <python3.14/Python.h>
// #include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define PARSE_ARGS_AND_KWARGS                                                   \
do                                                                              \
{                                                                               \
    if ( !argc )                                                                \
    {                                                                           \
        PyErr_SetString(PyExc_TypeError,                                        \
                        "Expected at least one positional argument.");          \
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
                                                                                \
    while ( PyDict_Next(kwargs, &pos, &key, &value) )                           \
    {                                                                           \
        const char* kwarg_name = PyUnicode_AsUTF8(key);                         \
        if ( !kwarg_name )                                                      \
        {                                                                       \
            PyErr_SetString(PyExc_TypeError,                                    \
                            "Keyword name UTF8-encoding failure.");             \
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
                    PyExc_TypeError,                                            \
                    "Invalid keyword argument value, "                          \
                    "\"only_str\" value must evaluate to either True or False"  \
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
                PyExc_TypeError,                                                \
                "Invalid keyword argument name. "                               \
                "The only allowed kwarg is: \"only_str\"."                      \
            );                                                                  \
            return NULL;                                                        \
        }                                                                       \
    }                                                                           \
                                                                                \
} while (0);

enum
{
    FAILED_TO_GET_ITER = 1,
    STR_ENCODING_FAILURE,
    DOUBLE_OVERFLOW
};

static inline int sum_iterable(PyObject* iter, bool only_str, double* sum)
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
            *sum += value;
        }
        else if ( PyFloat_Check(item) )
        {
            *sum += PyFloat_AS_DOUBLE(item);
        }
        Py_DECREF(item);
    }
    Py_DECREF(iter);

    return 0;
}

static inline int sum_list(PyObject* list, bool only_str, double* sum)
{
    Py_ssize_t count = PyList_GET_SIZE(list);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyList_GET_ITEM(list, i);

            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str ) return STR_ENCODING_FAILURE;

                *sum += strtod(str, NULL);
            }
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < count; i++ )
    {
        PyObject* item = PyList_GET_ITEM(list, i);

        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str ) return STR_ENCODING_FAILURE;

            *sum += strtod(str, NULL);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;

            *sum += value;
        }
        else if ( PyFloat_Check(item) )
        {
            *sum += PyFloat_AS_DOUBLE(item);
        }
    }

    return 0;
}

static inline int sum_tuple(PyObject* tuple, bool only_str, double* sum)
{
    Py_ssize_t count = PyTuple_GET_SIZE(tuple);

    if ( only_str )
    {
        for ( Py_ssize_t i = 0; i < count; i++ )
        {
            PyObject* item = PyTuple_GET_ITEM(tuple, i);

            if ( PyUnicode_Check(item) )
            {
                const char* str = PyUnicode_AsUTF8(item);
                if ( !str ) return STR_ENCODING_FAILURE;

                *sum += strtod(str, NULL);
            }
        }
        return 0;
    }

    for ( Py_ssize_t i = 0; i < count; i++ )
    {
        PyObject* item = PyTuple_GET_ITEM(tuple, i);

        if ( PyUnicode_Check(item) )
        {
            const char* str = PyUnicode_AsUTF8(item);
            if ( !str ) return STR_ENCODING_FAILURE;

            *sum += strtod(str, NULL);
        }
        else if ( PyLong_Check(item) )
        {
            double value = PyLong_AsDouble(item);
            if ( PyErr_Occurred() ) return DOUBLE_OVERFLOW;

            *sum += value;
        }
        else if ( PyFloat_Check(item) )
        {
            *sum += PyFloat_AS_DOUBLE(item);
        }
    }

    return 0;
}

static PyObject* sum(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;

    PARSE_ARGS_AND_KWARGS;

    double sum = 0.0;
    // size_t count = 0;
    int err;

    for ( Py_ssize_t i = 0; i < argc; i++ )
    {
        PyObject* arg = PyTuple_GET_ITEM(args, i);

        if ( PyList_Check(arg) )
        {
            err = sum_list(arg, is_only_str, &sum);
            if ( err ) return NULL;
        }
        else if ( PyTuple_Check(arg) )
        {
            err = sum_tuple(arg, is_only_str, &sum);
            if ( err ) return NULL;
        }
        else
        {
            PyObject* iter = PyObject_GetIter(arg);
            if ( !iter ) return NULL;

            err = sum_iterable(iter, is_only_str, &sum);
            if ( err ) return NULL;
        }
    }

    return PyFloat_FromDouble(sum);
}

static PyObject* max(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    bool is_only_str = false;

    PARSE_ARGS_AND_KWARGS;



    return Py_None;
}

static PyMethodDef methods[] = {
    {"sum", (PyCFunction)sum, METH_VARARGS | METH_KEYWORDS, ""},
    // * * *
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef numagr_module = {
    PyModuleDef_HEAD_INIT,
    "numagr",
    "",
    -1,
    methods
};

PyMODINIT_FUNC PyInit_numagr(void)
{
    return PyModule_Create(&numagr_module);
}
