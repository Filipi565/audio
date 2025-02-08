#include <miniaudio.h>
#include <raudio.h>
#include "audio.h"

#include <string.h>

typedef struct WaveObj WaveObj;

struct WaveObj
{
    PyObject _;
    Wave wave;
};

static PyObject *Wave_is_valid(WaveObj *self, PyObject *args)
{
    return PyBool_FromLong((long)IsWaveValid(self->wave));
}

static PyObject *Wave_unload(WaveObj *self, PyObject *args)
{
    if (IsWaveValid(self->wave))
    {
        UnloadWave(self->wave);
    }
    
    Py_RETURN_NONE;
}

#define GETMETHOD(fname) Wave_##fname

static PyMethodDef methods[] = {
    METHOD(is_valid, METH_NOARGS),
    METHOD(unload, METH_NOARGS),
    {NULL, NULL, 0, NULL}
};

PyTypeObject Wave_Type = {
    .tp_name = "_audio.Wave",
    .tp_basicsize = sizeof(WaveObj),
    .tp_methods = methods,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE
};

PyObject *WaveFromBytes(PyObject *m, PyObject *args)
{
    char *filetype;
    PyObject *cls_obj;
    PyObject *bytes;

    if (!PyArg_ParseTuple(args, "sOO", &filetype, &cls_obj, &bytes))
    {
        return NULL;
    }

    if (!PyObject_IsInstance(cls_obj, (PyObject *)&Wave_Type))
    {
        PyErr_SetString(PyExc_TypeError, "cls must be Wave type object");
    }

    if (!PyObject_IsInstance(bytes, (PyObject *)&PyBytes_Type))
    {
        PyErr_SetString(PyExc_TypeError, "Expected bytes object");
    }

    PyTypeObject *cls = (PyTypeObject *)cls_obj;

    WaveObj *result = (WaveObj *)cls->tp_new(cls, NULL, NULL);
    if (result == NULL)
    {
        return NULL;
    }

    char *buffer;
    Py_ssize_t length;
    if (PyBytes_AsStringAndSize(bytes, &buffer, &length) != 0)
    {
        return NULL;
    }

    int err;
    result->wave = LoadWaveFromMemory(filetype, buffer, (int)length, &err);

    if (err == 1)
    {
        PyErr_SetString(PyExc_OSError, "Error on loading audio data");
    }
    else if (err == -1)
    {
        PyErr_SetString(PyExc_IOError, "Invalid file type");
    }
}