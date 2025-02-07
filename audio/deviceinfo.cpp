#include <miniaudio.h>
#include <Python.h>
#include "audio.h"

class DeviceInfo
{
    public:
    PyObject _;
    ma_device_info info;

    static PyObject *name(DeviceInfo *self, PyObject *args)
    {
        return PyUnicode_FromString(self->info.name);
    }

    static PyObject *id(DeviceInfo *self, PyObject *args)
    {
        PyObject *result = PyType_GenericNew(&DeviceId_Type, nullptr, nullptr);
        ma_device_id *id = reinterpret_cast<ma_device_id *>(((char *)result) + sizeof(PyObject));
        (*id) = self->info.id;

        return result;
    }
};

#define GETMETHOD(name) DeviceInfo::name

static PyMethodDef methods[] = {
    METHOD(name, METH_NOARGS),
    METHOD(id, METH_NOARGS),
    {nullptr, nullptr, 0, nullptr}
};

class DeviceInfoTypeObject: public PyTypeObject
{
    public:
    DeviceInfoTypeObject()
    {
        tp_new = nullptr;
        tp_name = "_audio.DeviceInfo";
        tp_basicsize = sizeof(DeviceInfo);
        tp_methods = methods;
        tp_flags = 0;
    }
};

PyTypeObject DeviceInfo_Type = DeviceInfoTypeObject();