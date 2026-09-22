#include <python3.14/Python.h>
#include <stdio.h>
#include <stdbool.h>

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

    PyObject* only_str_kwarg;
    PyObject* ret_int_kwarg;
    int ret;

    if ( (only_str_kwarg = PyDict_GetItemString(kwargs, "only_str")) )
    {
        ret = PyObject_IsTrue(only_str_kwarg);
        if ( ret < 0 )
            return (kwargs_state){.error = true};

        state.only_str = ret;
    }
    if ( (ret_int_kwarg = PyDict_GetItemString(kwargs, "ret_int")) )
    {
        ret = PyObject_IsTrue(ret_int_kwarg);
        if ( ret < 0 )
            return (kwargs_state){.error = true};

        state.ret_int = ret;
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
    if ( state.error ) return NULL;

    printf("you've passed %zu pos args\n", argc);
    state.only_str ? puts("only_str is True") : puts("only str is False");
    state.ret_int ? puts("ret_int is True") : puts("ret_int is False");


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
