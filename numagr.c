#include <python3.14/Python.h>
#include <python3.14/abstract.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    bool only_str;
    bool kwarg_name_encoding_failure;
    bool invalid_kwarg_name;
    bool invalid_kwarg_value;

} kwarg_state;

static inline bool kwarg_is_valid(kwarg_state state)
{
    if ( state.kwarg_name_encoding_failure )
    {
        PyErr_SetString(PyExc_TypeError,
                        "Keyword name UTF8-encoding failure.");
        return false;
    }
    else if ( state.invalid_kwarg_name )
    {
        PyErr_SetString(PyExc_TypeError,
                        "Invalid keyword argument name. "
                        "The only allowed kwarg is: \"only_str\".");
        return false;
    }
    else if ( state.invalid_kwarg_value )
    {
        PyErr_SetString(PyExc_TypeError,
                        "Invalid keyword argument value, "
                        "\"only_str\" value must evaluate to either True or False");
        return false;
    }

    return true;
}

static inline kwarg_state parse_kwarg(PyObject* kwargs)
{
    if ( !kwargs )
        return (kwarg_state){false};

    kwarg_state state = {false};

    PyObject* key;
    PyObject* value;
    Py_ssize_t pos = 0;
    bool only_str_found = false;

    while ( PyDict_Next(kwargs, &pos, &key, &value) )
    {
        const char* kwarg_name = PyUnicode_AsUTF8(key);
        if ( !kwarg_name )
            return (kwarg_state){.kwarg_name_encoding_failure = true};

        if ( !only_str_found && !strcmp(kwarg_name, "only_str") )
        {
            int ret = PyObject_IsTrue(value);
            if ( ret < 0 )
                return (kwarg_state){.invalid_kwarg_value = true};

            state.only_str = ret;
            only_str_found = true;
        }
        else
        {
            return (kwarg_state){.invalid_kwarg_name = true};
        }
    }

    return state;
}

static inline double sum_iterable(PyObject* iterable)
{
    double sum = 0.0;



    PyObject* iter = PyObject_GetIter(iterable);
    if ( !iter ) return -1.0;

    PyObject* item;
    while ( (item = PyIter_Next(iter)) )
    {


        Py_DECREF(item);
    }
    Py_DECREF(iter);


    return sum;
}

static PyObject* sum(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    if ( !argc )
    {
        PyErr_SetString(PyExc_TypeError,
                        "Expected at least one positional argument.");
        return NULL;
    }

    kwarg_state state = parse_kwarg(kwargs);
    if ( state.only_str && !kwarg_is_valid(state) )
        return NULL;

    printf("you've passed %zu pos args\n", argc);
    state.only_str ? puts("only_str is True") : puts("only str is False");
    // state.ret_int ? puts("ret_int is True") : puts("ret_int is False");

    // for ( Py_ssize_t i = 0; i < argc; i++ )
    // {
    //     ;
    // }

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
