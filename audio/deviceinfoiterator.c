#include <miniaudio.h>
#include "audio.h"

#define GETMETHOD(name) DeviceInfoIter_##name

#define PlaybackInfos(self_obj) ((ma_device_info *)(self_obj->pPlaybackInfos))

static PyObject *create_device_info(ma_device_info *info)
{
    PyObject *info_obj = PyType_GenericNew(&DeviceInfo_Type, NULL, NULL);

    if (info == NULL)
    {
        return NULL;
    }

    ma_device_info *device_info = (ma_device_info *)(info_obj + 1);
    (*device_info) = (*info);

    return info_obj;
}

static PyObject *DeviceInfoIter_iter(DeviceInfoIterator *self)
{
    Py_INCREF(self);
    return self;
}

static PyObject *DeviceInfoIter_next(DeviceInfoIterator *self)
{
    if (self->current >= self->playbackCount)
    {
        PyErr_SetString(PyExc_StopIteration, "");
        self->current = 0;
        return NULL;
    }
    PyObject *item = create_device_info(PlaybackInfos(self) + self->current);
    self->current++;
    return item;
}

static Py_ssize_t DeviceInfoIter_len(DeviceInfoIterator *self)
{
    return (Py_ssize_t)self->playbackCount;
}

static PyObject *DeviceInfoIter_getitem(DeviceInfoIterator *self, Py_ssize_t index)
{
    if (index < 0 || ((uint32_t)index) >= self->playbackCount)
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        return NULL;
    }
    return create_device_info(PlaybackInfos(self) + index);
}

static PySequenceMethods sequence = {
    .sq_length = (lenfunc)GETMETHOD(len),
    .sq_item = (ssizeargfunc)GETMETHOD(getitem)
};

PyTypeObject DeviceInfoIterator_Type = {
    .tp_name = "DeviceInfoIterator",
    .tp_basicsize = sizeof(DeviceInfoIterator),
    .tp_iter = (getiterfunc)GETMETHOD(iter),
    .tp_iternext = (iternextfunc)GETMETHOD(next),
    .tp_as_sequence = &sequence
};