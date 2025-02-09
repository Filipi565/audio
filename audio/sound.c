#include <raudio.h>
#include "audio.h"

#define GETMETHOD(fname) Sound_##fname

typedef struct SoundObj SoundObj;

struct SoundObj
{
    PyObject _;
    Sound sound;
};

static PyObject *Sound_is_valid(SoundObj *self, PyObject *args)
{
    return PyBool_FromLong((long)IsSoundValid(self->sound));
}

static PyObject *Sound_is_playing(SoundObj *self, PyObject *args)
{
    return PyBool_FromLong((long)IsSoundPlaying(self->sound));
}

static PyObject *Sound_resume(SoundObj *self, PyObject *args)
{
    ResumeSound(self->sound);
    Py_RETURN_NONE;
}


static PyObject *Sound_pause(SoundObj *self, PyObject *args)
{
    PauseSound(self->sound);
    Py_RETURN_NONE;
}

static PyObject *Sound_play(SoundObj *self, PyObject *args)
{
    PlaySound(self->sound);
    Py_RETURN_NONE;
}

static PyObject *Sound_unload(SoundObj *self, PyObject *args)
{
    if (IsSoundValid(self->sound))
    {
        UnloadSound(self->sound);
    }

    Py_RETURN_NONE;
}

static PyObject *Sound_stop(SoundObj *self, PyObject *args)
{
    StopSound(self->sound);
    Py_RETURN_NONE;
}

static PyMethodDef methods[] = {
    {"__del__", (PyCFunction)Sound_unload, METH_NOARGS, NULL},
    METHOD(is_valid, METH_NOARGS),
    METHOD(unload, METH_NOARGS),
    METHOD(resume, METH_NOARGS),
    METHOD(pause, METH_NOARGS),
    METHOD(play, METH_NOARGS),
    METHOD(stop, METH_NOARGS),
    {NULL, NULL, 0, NULL}
};

static PyObject *Sound_getVolume(SoundObj *self, void *closure)
{
    return PyFloat_FromDouble((double)GetSoundVolume(self->sound));
}

static int Sound_setVolume(SoundObj *self, PyObject *value, void *closure)
{
    float volume;

    if (PyObject_IsInstance(value, &PyLong_Type) && !PyObject_IsInstance(value, &PyBool_Type))
    {
        volume = (float)PyLong_AsDouble(value);
    }
    else if (PyObject_IsInstance(value, &PyFloat_Type))
    {
        volume = (float)PyFloat_AsDouble(value);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "volume must be float or int object");
        return -1;
    }

    if (volume < 0)
    {
        PyErr_SetString(PyExc_ValueError, "volume must be positive");
        return -1;
    }

    SetSoundVolume(self->sound, volume);

    return 0;
}

static PyGetSetDef properties[] = {
    {"volume", (getter)Sound_getVolume, (setter)Sound_setVolume, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

PyTypeObject Sound_Type = {
    .tp_name = "_audio.Sound",
    .tp_basicsize = sizeof(SoundObj),
    .tp_methods = methods,
    .tp_getset = properties,
    .tp_new = NULL,
    .tp_flags = Py_TPFLAGS_BASETYPE | Py_TPFLAGS_DEFAULT
};

PyObject *SoundFromWave(PyObject *m, PyObject *args)
{
    PyObject *cls_obj;
    PyObject *obj;

    if (!PyArg_ParseTuple(args, "OO", &cls_obj, &obj))
    {
        return NULL;
    }

    if (!PyObject_IsSubclass(cls_obj, (PyObject *)&Sound_Type))
    {
        PyErr_SetString(PyExc_TypeError, "cls must be a subclass of _audio.Sound");
        return NULL;
    }

    if (!PyObject_IsInstance(obj, &Wave_Type))
    {
        PyErr_SetString(PyExc_TypeError, "wave must be a _audio.Wave object");
        return NULL;
    }

    PyTypeObject *cls = (PyTypeObject *)cls_obj;

    Wave *wave = (Wave *)(obj + 1);
    SoundObj *result = (SoundObj *)PyType_GenericNew(cls, NULL, NULL);

    result->sound = LoadSoundFromWave(*wave);

    return (PyObject *)result;
}