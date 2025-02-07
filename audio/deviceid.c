#include <miniaudio.h>
#include <Python.h>

typedef struct DeviceId DeviceId;

struct DeviceId
{
    PyObject _;
    ma_device_id id;
};

PyTypeObject DeviceId_Type = {
    .tp_name = "_audio.DeviceId",
    .tp_basicsize = sizeof(DeviceId),
    .tp_methods = NULL,
    .tp_new = NULL,
    .tp_flags = 0
};