#include <Python.h>
#include "numagr.h"

DECLARE_EXCEPTIONS;
#include "methods.c"

static PyMethodDef methods[] = {
    {"sum", (PyCFunction)sum, METH_VARARGS | METH_KEYWORDS, ""},
    {"max", (PyCFunction)max, METH_VARARGS | METH_KEYWORDS, ""},
    {"min", (PyCFunction)min, METH_VARARGS | METH_KEYWORDS, ""},
    {"amean", (PyCFunction)amean, METH_VARARGS | METH_KEYWORDS, ""},
    {"gmean", (PyCFunction)gmean, METH_VARARGS | METH_KEYWORDS, ""},
    {"hmean", (PyCFunction)hmean, METH_VARARGS | METH_KEYWORDS, ""},
    {"nearest", (PyCFunction)nearest, METH_VARARGS | METH_KEYWORDS, ""},
    {"count", (PyCFunction)count, METH_VARARGS | METH_KEYWORDS, ""},
    {"frac_below", (PyCFunction)frac_below, METH_VARARGS | METH_KEYWORDS, ""},
    {"frac_above", (PyCFunction)frac_above, METH_VARARGS | METH_KEYWORDS, ""},
    {"mode", (PyCFunction)mode, METH_VARARGS | METH_KEYWORDS, ""},
    {"var", (PyCFunction)variance, METH_VARARGS | METH_KEYWORDS, ""},
    {"pvar", (PyCFunction)pvariance, METH_VARARGS | METH_KEYWORDS, ""},
    {"stdev", (PyCFunction)stdev, METH_VARARGS | METH_KEYWORDS, ""},
    {"pstdev", (PyCFunction)pstdev, METH_VARARGS | METH_KEYWORDS, ""},

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
    PyObject* module = PyModule_Create(&numagr_module);
    if ( !module )
        return NULL;

    ADD_EXCEPTION(module, CountOverflow, "CountOverflow");
    ADD_EXCEPTION(module, InvalidStringValue, "InvalidStringValue");
    ADD_EXCEPTION(module, NegativeValue, "NegativeValue");
    ADD_EXCEPTION(module, ZeroValue, "ZeroValue");

    return module;
}
