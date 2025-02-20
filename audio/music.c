#include <raudio.h>
#include "audio.h"

typedef struct MusicObj MusicObj;

struct MusicObj
{
    PyObject _;
    Music music;
    char *data;
};

static PyObject *Music_play(MusicObj *self, PyObject *args)
{
    PlayMusicStream(self->music);
    Py_RETURN_NONE;
}

static PyObject *Music_is_valid(MusicObj *self, PyObject *args)
{
    return PyBool_FromLong((long)IsMusicValid(self->music));
}

static PyObject *Music_unload(MusicObj *self, PyObject *args)
{
    UnloadMusicStream(self->music);
    free(self->data);
    Py_RETURN_NONE;
}

static PyObject *Music_stop(MusicObj *self, PyObject *args)
{
    StopMusicStream(self->music);
    Py_RETURN_NONE;
}

static PyObject *Music_update(MusicObj *self, PyObject *args)
{
    UpdateMusicStream(self->music);
    Py_RETURN_NONE;
}

static PyObject *Music_pause(MusicObj *self, PyObject *args)
{
    PauseMusicStream(self->music);
    Py_RETURN_NONE;
}

static PyObject *Music_resume(MusicObj *self, PyObject *args)
{
    ResumeMusicStream(self->music);
    Py_RETURN_NONE;
}

static PyObject *Music_seek(MusicObj *self, PyObject *args)
{
    double volume;

    if (!PyArg_ParseTuple(args, "d", &volume))
    {
        return NULL;
    }

    SeekMusicStream(self->music, (float)volume);
    Py_RETURN_NONE;
}

static PyObject *Music_time_length(MusicObj *self, PyObject *args)
{
    return PyFloat_FromDouble((double)GetMusicTimeLength(self->music));
}

static PyObject *Music_time_played(MusicObj *self, PyObject *args)
{
    return PyFloat_FromDouble((double)GetMusicTimePlayed(self->music));
}

static PyObject *Music_is_playing(MusicObj *self, PyObject *args)
{
    return PyBool_FromLong((long)IsMusicStreamPlaying(self->music));
}

#define GETMETHOD(name) Music_##name

static PyMethodDef methods[] = {
    METHOD(time_length, METH_NOARGS),
    METHOD(time_played, METH_NOARGS),
    METHOD(is_playing, METH_NOARGS),
    METHOD(is_valid, METH_NOARGS),
    METHOD(unload, METH_NOARGS),
    METHOD(update, METH_NOARGS),
    METHOD(resume, METH_NOARGS),
    METHOD(seek, METH_VARARGS),
    METHOD(pause, METH_NOARGS),
    METHOD(stop, METH_NOARGS),
    METHOD(play, METH_NOARGS),
    {NULL, NULL, 0, NULL}
};

static PyObject *Music_getVolume(MusicObj *self, void *closure)
{
    return PyFloat_FromDouble((double)GetMusicVolume(self->music));
}

static int Music_setVolume(MusicObj *self, PyObject *other, void *closure)
{
    if (!PyObject_IsInstance(other, &PyFloat_Type))
    {
        PyErr_SetString(PyExc_TypeError, "Music.volume must be a float object");
        return 1;
    }

    float volume = (float)PyFloat_AsDouble(other);
    SetMusicVolume(self->music, volume);

    return 0;
}

static PyGetSetDef properties[] = {
    {"volume", (getter)GETMETHOD(getVolume), (setter)GETMETHOD(setVolume), NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

static void Music_delete(MusicObj *self)
{
    UnloadMusicStream(self->music);
    free(self->data);
}

PyTypeObject Music_Type = {
    .tp_name = "_audio.Music",
    .tp_methods = methods,
    .tp_new = NULL,
    .tp_flags = Py_TPFLAGS_BASETYPE | Py_TPFLAGS_DEFAULT,
    .tp_basicsize = sizeof(MusicObj),
    .tp_getset = properties,
    .tp_dealloc = (destructor)Music_delete
};

PyObject *MusicFromBytes(PyObject *m, PyObject *args)
{
    char *filetype;
    PyObject *cls_obj;
    PyObject *bytes;

    if (!PyArg_ParseTuple(args, "sOO", &filetype, &cls_obj, &bytes))
    {
        return NULL;
    }

    if (!IsAudioDeviceReady())
    {
        PyErr_Warn(PyExc_Warning, "AudioDevice should be initialized before loading a Music object");
    }

    if (!PyObject_IsSubclass(cls_obj, (PyObject *)&Music_Type))
    {
        PyErr_SetString(PyExc_TypeError, "cls must be Music type object");
        return NULL;
    }

    if (!PyObject_IsInstance(bytes, &PyBytes_Type))
    {
        PyErr_SetString(PyExc_TypeError, "Expected bytes object");
        return NULL;
    }

    PyTypeObject *cls = (PyTypeObject *)cls_obj;

    MusicObj *result = (MusicObj *)PyType_GenericNew(cls, NULL, NULL);
    if (result == NULL)
    {
        return NULL;
    }

    char *tmp_buffer;
    Py_ssize_t length;
    if (PyBytes_AsStringAndSize(bytes, &tmp_buffer, &length) != 0)
    {
        Py_DECREF(result);
        return NULL;
    }

    char *buffer = (char *)malloc((size_t)(length*sizeof(char)));

    if (buffer == NULL)
    {
        Py_DECREF(result);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }

    result->data = buffer;

    memmove(buffer, tmp_buffer, (size_t)(length*sizeof(char)));

    int err;
    result->music = LoadMusicStreamFromMemory(filetype, buffer, (int)length, &err);

    if (err != 0)
    {
        Py_DECREF(result);
    }

    if (err == 1)
    {
        PyErr_SetString(PyExc_MemoryError, "Error on loading audio data");
        return NULL;
    }
    else if (err == -3)
    {
        PyErr_SetString(PyExc_IOError, "Invalid file type");
        return NULL;
    }
    else if (err == -2)
    {
        PyErr_SetString(PyExc_IOError, "Music data could not be loaded");
        return NULL;
    }

    return (PyObject *)result;
}