#include <python3.14/Python.h>
#include <stdio.h>

static PyObject* hello(PyObject* self, PyObject* args)
{
    puts("hello from C");

    return Py_None;
}

static PyMethodDef methods[] = {
                                    {"hello", hello, METH_NOARGS, "test module"},
                                    {NULL, NULL, 0, NULL}
                               };

static struct PyModuleDef hello_module = {
    PyModuleDef_HEAD_INIT,
    "hello",
    "A simple C extension",
    -1,
    methods
};

PyMODINIT_FUNC PyInit_hello(void)
{
    return PyModule_Create(&hello_module);
}
