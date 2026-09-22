#include <python3.14/Python.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    bool only_str;
    bool ret_int;
    bool error;

} kwargs_state;

static inline kwargs_state parse_kwargs(PyObject* kwargs)
{
    if ( !kwargs )
        return (kwargs_state){false};

    kwargs_state state = {false};

    PyObject* key;
    PyObject* value;
    Py_ssize_t pos = 0;
    bool only_str_found = false;
    bool ret_int_found = false;

    while ( PyDict_Next(kwargs, &pos, &key, &value) )
    {
        const char* kwarg_name = PyUnicode_AsUTF8(key);
        if ( !kwarg_name )
            return (kwargs_state){.error = true};

        if ( !only_str_found && !strcmp(kwarg_name, "only_str") )
        {
            int ret = PyObject_IsTrue(value);
            if ( ret < 0 )
                return (kwargs_state){.error = true};

            state.only_str = ret;
            only_str_found = true;
        }
        else if ( !ret_int_found && !strcmp(kwarg_name, "ret_int") )
        {
            int ret = PyObject_IsTrue(value);
            if ( ret < 0 )
                return (kwargs_state){.error = true};

            state.ret_int = ret;
            ret_int_found = true;
        }
        else
        {
            return (kwargs_state){.error = true};
        }
    }

    return state;
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

    kwargs_state state = parse_kwargs(kwargs);
    if ( state.error )
    {
        PyErr_SetString(PyExc_TypeError,
                        "Invalid keyword argument(s).");
        return NULL;
    }

    printf("you've passed %zu pos args\n", argc);
    state.only_str ? puts("only_str is True") : puts("only str is False");
    state.ret_int ? puts("ret_int is True") : puts("ret_int is False");
    //printf("%zu\n", sizeof state);

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
