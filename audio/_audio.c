#include "audio.h"

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_audio",
    .m_methods = NULL
};

PyObject *MiniAudioError;

PyMODINIT_FUNC PyInit__audio()
{
    MiniAudioError = PyErr_NewException("_audio.MiniAudioError", PyExc_Exception, NULL);
    if (MiniAudioError == NULL)
    {
        return NULL;
    }

    PyObject *m = PyModule_Create(&module);

    if (m == NULL)
    {
        return NULL;
    }

    Py_INCREF(MiniAudioError);
    if (PyModule_AddObject(m, "MiniAudioError", MiniAudioError) != 0)
    {
        Py_DECREF(MiniAudioError);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}