#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *cvarint_encode(PyObject *self, PyObject *args) {
    unsigned long long value;
    char buffer[10];
    int length = 0;
    unsigned long long mask = 127;

    if (!PyArg_ParseTuple(args, "K", &value))
        return NULL;

    if (value == 0) {
        return PyBytes_FromStringAndSize(buffer, length);
        buffer[0] = 0;
        length = 1;
    }   

    while(value) {
            int payload = value & mask;
            value = value >> 7;

            if (value) {
                buffer[length] = payload | 128;
            } else {
                buffer[length] = payload;
            }

            length++;
        }

    return PyBytes_FromStringAndSize(buffer, length);
}


static PyObject *cvarint_decode(PyObject *self, PyObject *args) {
    const char *buffer;
    Py_ssize_t length;
    unsigned long long value = 0;

    if (!PyArg_ParseTuple(args, "y#", &buffer, &length))
        return NULL;

    if (length == 0) {
        PyErr_SetString(PyExc_ValueError, "cannot decode an empty varint");
        return NULL;
    }

    if (length > 10) {
        PyErr_SetString(PyExc_ValueError, "varint is too long for uint64");
        return NULL;
    }

    for (Py_ssize_t index = 0; index < length; index++) {
        unsigned char byte = (unsigned char)buffer[index];
        unsigned long long payload = byte & 0x7f;

        if (index == 9 && payload > 1) {
            PyErr_SetString(PyExc_ValueError, "varint exceeds uint64 range");
            return NULL;
        }

        value |= payload << (index * 7);
    }

    return PyLong_FromUnsignedLongLong(value);
}

static PyMethodDef CVarintMethods[] = {
    {"encode", cvarint_encode, METH_VARARGS, "Encode an integer as varint."},
    {"decode", cvarint_decode, METH_VARARGS,
     "Decode varint bytes to an integer."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef cvarintmodule = {
    PyModuleDef_HEAD_INIT, "cvarint",
    "A C implementation of protobuf varint encoding", -1, CVarintMethods};

PyMODINIT_FUNC PyInit_cvarint(void) { return PyModule_Create(&cvarintmodule); }
