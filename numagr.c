#include <Python.h>
#include "methods.c"

static PyMethodDef methods[] = {
    {"sum", (PyCFunction)sum, METH_VARARGS | METH_KEYWORDS, ""},
    {"max", (PyCFunction)max, METH_VARARGS | METH_KEYWORDS, ""},
    {"min", (PyCFunction)min, METH_VARARGS | METH_KEYWORDS, ""},
    {"amean", (PyCFunction)amean, METH_VARARGS | METH_KEYWORDS, ""},
    {"gmean", (PyCFunction)gmean, METH_VARARGS | METH_KEYWORDS, ""},
    {"hmean", (PyCFunction)hmean, METH_VARARGS | METH_KEYWORDS, ""},
    {"nearest", (PyCFunction)nearest, METH_VARARGS | METH_KEYWORDS, ""},

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
