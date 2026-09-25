#ifndef NUMAGR_H
#define NUMAGR_H

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

enum
{
    FAILED_TO_GET_ITER = 1,
    STR_ENCODING_FAILURE,
    DOUBLE_OVERFLOW,
    COUNT_OVERFLOW
};

#endif
