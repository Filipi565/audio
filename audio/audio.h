#ifndef AUDIO_H
#define AUDIO_H

#include <Python.h>

#define PyObject_IsInstance(l, r) PyObject_IsInstance((PyObject *)(l), (PyObject *)(r))
#define METHOD(name, flags) {#name, (PyCFunction)GETMETHOD(name), flags, NULL}

#ifdef __cplusplus
extern "C" {
#endif

extern PyTypeObject AudioDevice_Type;
extern PyTypeObject DeviceInfo_Type;
extern PyTypeObject DeviceId_Type;
extern PyObject *MiniAudioError;
extern PyTypeObject Sound_Type;
extern PyTypeObject Wave_Type;

#ifdef __cplusplus
}
#endif

#endif