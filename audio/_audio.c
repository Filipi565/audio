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

    PyTypeObject *obj[] = {&AudioDevice_Type, &DeviceId_Type, &DeviceInfo_Type};
    const char *names[] = {"_AudioDeviceHelper", "DeviceId", "DeviceInfo"};
    size_t s = sizeof(obj)/sizeof(void *);

    for (int i = 0; i < s; i++)
    {
        if (PyType_Ready(obj[i]) != 0)
        {
            return NULL;
        }
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

    for (int i = 0; i < s; i++)
    {
        Py_INCREF(obj[i]);
        if (PyModule_AddObject(m, names[i], obj[i]) != 0)
        {
            for (int j = 0; j < (i+1); j++)
            {
                Py_DECREF(obj[i]);
            }
            Py_DECREF(MiniAudioError);
            Py_DECREF(m);
            return NULL;
        }
    }
    
    return m;
}