#include <python3.14/Python.h>
// #include <python3.14/pyerrors.h>
// #include <python3.14/pyport.h>
// #include <python3.14/tupleobject.h>
#include <stdio.h>

static PyObject* sum(PyObject* self, PyObject* args, PyObject* kwargs)
{
    Py_ssize_t argc = PyTuple_GET_SIZE(args);
    if ( !argc )
    {
        PyErr_SetString(PyExc_TypeError,
                        "Expected at least one positional argument.");
        return NULL;
    }

    PyObject* only_str_kwarg;
    PyObject* ret_int_kwarg;
    int only_str = 0;
    int ret_int = 0;
    if ( kwargs )
    {
        if ( (only_str_kwarg = PyDict_GetItemString(kwargs, "only_str")) )
        {
            only_str = PyObject_IsTrue(only_str_kwarg);
            if ( only_str < 0 ) return NULL;
        }
        if ( (ret_int_kwarg = PyDict_GetItemString(kwargs, "ret_int")) )
        {
            ret_int = PyObject_IsTrue(ret_int_kwarg);
            if ( ret_int < 0 ) return NULL;
        }
    }

    printf("you've passed %zu pos args\n", argc);
    only_str ? puts("only_str is True") : puts("only str is False");
    ret_int ? puts("ret_int is True") : puts("ret_int is False");
    // for ( int i = 0; i < argc; i++ )
    // {
    //     puts();
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
