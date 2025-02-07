#ifndef AUDIO_H
#define AUDIO_H

#include <Python.h>

#define METHOD(name, flags) {#name, (PyCFunction)GETMETHOD(name), flags, NULL}

#ifdef __cplusplus
extern "C" {
#endif

PyTypeObject DeviceId_Type;

#ifdef __cplusplus
}
#endif

#endif