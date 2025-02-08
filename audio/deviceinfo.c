#include <miniaudio.h>
#include <Python.h>
#include "audio.h"

typedef struct DeviceInfo DeviceInfo;

struct DeviceInfo
{
    PyObject _;
    ma_device_info info;
};

static PyObject *DeviceInfoMethod_name(DeviceInfo *self, PyObject *args)
{
    return PyUnicode_FromString(self->info.name);
}

static PyObject *DeviceInfoMethod_id(DeviceInfo *self, PyObject *args)
{
    PyObject *result = PyType_GenericNew(&DeviceId_Type, NULL, NULL);
    ma_device_id *id = (ma_device_id *)(result + 1);
    (*id) = self->info.id;

    return result;
}

#define GETMETHOD(name) DeviceInfoMethod_##name

static PyMethodDef methods[] = {
    METHOD(name, METH_NOARGS),
    METHOD(id, METH_NOARGS),
    {NULL, NULL, 0, NULL}
};

PyTypeObject DeviceInfo_Type = {
    .tp_new = NULL,
    .tp_name = "_audio.DeviceInfo",
    .tp_basicsize = sizeof(DeviceInfo),
    .tp_methods = methods,
    .tp_flags = 0
};