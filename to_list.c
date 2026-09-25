#include <Python.h>
#include <stdbool.h>
#include "numagr.h"

static inline int append_from_list(PyObject* dest_list, PyObject* src_list)
{
    Py_ssize_t src_len = PyList_GET_SIZE(src_list);

    for ( Py_ssize_t i = 0; i < src_len; i++ )
    {

    }

    return 0;
}

static inline int append_from_tuple(PyObject* dest_list, PyObject* src_tuple)
{
    return 0;
}

static inline int append_from_iterable(PyObject* dest_list, PyObject* src_iter)
{
    return 0;
}
