#include <miniaudio.h>
#include <Python.h>
#include "audio.h"

typedef struct DeviceInfo DeviceInfo;

struct DeviceInfo
{
    PyObject _;
    ma_device_info info;
};

static PyObject *DeviceInfoMethod_name(DeviceInfo *self, void *closure)
{
    return PyUnicode_FromString(self->info.name);
}

static PyObject *DeviceInfoMethod_id(DeviceInfo *self, void *closure)
{
    PyObject *result = PyType_GenericNew(&DeviceId_Type, NULL, NULL);
    
    if (result == NULL)
    {
        return NULL;
    }

    ma_device_id *id = (ma_device_id *)(result + 1);
    (*id) = self->info.id;

    return result;
}

static PyObject *DeviceInfoMethod_Repr(DeviceInfo *self)
{
    char name[13 + sizeof(self->info.name)];
    snprintf(name, sizeof(name)/sizeof(char), "DeviceInfo(\'%s\')", self->info.name);

    return PyUnicode_FromString(name);
}

#define GETMETHOD(name) DeviceInfoMethod_##name

static PyGetSetDef properties[] = {
    {"name", (getter)GETMETHOD(name), NULL, NULL, NULL},
    {"id", (getter)GETMETHOD(id), NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

PyTypeObject DeviceInfo_Type = {
    .tp_new = NULL,
    .tp_name = "_audio.DeviceInfo",
    .tp_basicsize = sizeof(DeviceInfo),
    .tp_getset = properties,
    .tp_methods = NULL,
    .tp_flags = 0,
    .tp_repr = (reprfunc)GETMETHOD(Repr)
};