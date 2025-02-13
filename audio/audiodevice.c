#include <miniaudio.h>
#include <Python.h>
#include "audio.h"

#define PyExc_MiniAudioError (PyObject *)()

typedef unsigned char bool;

#define false 0
#define true 1

typedef void (*AudioCallback)(void *bufferData, unsigned int frames);

typedef struct rAudioProcessor rAudioProcessor;
typedef struct rAudioBuffer rAudioBuffer;

struct rAudioProcessor {
    AudioCallback process;          // Processor callback function
    rAudioProcessor *next;          // Next audio processor on the list
    rAudioProcessor *prev;          // Previous audio processor on the list
};

struct rAudioBuffer {
    ma_data_converter converter;    // Audio data converter

    AudioCallback callback;         // Audio buffer callback for buffer filling on audio threads
    rAudioProcessor *processor;     // Audio processor

    float volume;                   // Audio buffer volume
    float pitch;                    // Audio buffer pitch
    float pan;                      // Audio buffer pan (0.0f to 1.0f)

    bool playing;                   // Audio buffer state: AUDIO_PLAYING
    bool paused;                    // Audio buffer state: AUDIO_PAUSED
    bool looping;                   // Audio buffer looping, default to true for AudioStreams
    int usage;                      // Audio buffer usage mode: STATIC or STREAM

    bool isSubBufferProcessed[2];   // SubBuffer processed (virtual double buffer)
    unsigned int sizeInFrames;      // Total buffer size in frames
    unsigned int frameCursorPos;    // Frame cursor position
    unsigned int framesProcessed;   // Total frames processed in this buffer (required for play timing)

    unsigned char *data;            // Data buffer, on music stream keeps filling

    rAudioBuffer *next;             // Next audio buffer on the list
    rAudioBuffer *prev;             // Previous audio buffer on the list
};

typedef struct AudioData {
    struct {
        ma_context context;         // miniaudio context data
        ma_device device;           // miniaudio device
        ma_mutex lock;              // miniaudio mutex lock
        bool isReady;               // Check if audio device is ready
        size_t pcmBufferSize;       // Pre-allocated buffer size
        void *pcmBuffer;            // Pre-allocated buffer to read audio data from file/memory
    } System;
    struct {
        rAudioBuffer *first;         // Pointer to first AudioBuffer in the list
        rAudioBuffer *last;          // Pointer to last AudioBuffer in the list
        int defaultSize;            // Default audio buffer size for audio streams
    } Buffer;
    rAudioProcessor *mixedProcessor;
    PyObject *device_id;
} AudioData;

extern AudioData AUDIO;

void OnSendAudioDataToDevice(ma_device *pDevice, void *pFramesOut, const void *pFramesInput, ma_uint32 frameCount);

static bool isContextInitialized = false;
static ma_context_config context_config;

static PyObject *AudioDevice_init(PyObject *self, PyObject *args)
{
    ma_device_id *device_id;
    PyObject *device_id_obj;
    ma_result result;

    if (!PyArg_ParseTuple(args, "O", &device_id_obj))
    {
        return NULL;
    }

    if (Py_IsNone(device_id_obj))
    {
        device_id = NULL;
    }
    else if (!PyObject_IsInstance(device_id_obj, &DeviceId_Type))
    {
        PyErr_SetString(PyExc_TypeError, "\'audiodevice\' must be a _audio.DeviceId or NoneType object");
        return NULL;
    }
    else
    {
        device_id = (ma_device_id *)(device_id_obj + 1);
    }

    if (!isContextInitialized)
    {
        context_config = ma_context_config_init();
        result = ma_context_init(NULL, 0, &context_config, &AUDIO.System.context);
        
        if (result != MA_SUCCESS)
        {
            PyErr_SetString(MiniAudioError, "Failed to initialize MiniAudio context");
            return NULL;
        }
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = device_id;
    config.playback.format = ma_format_f32;
    config.playback.channels = 2;
    config.capture.pDeviceID = NULL;  // NULL for the default capture AUDIO.System.device
    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = 0;
    config.dataCallback = OnSendAudioDataToDevice;
    config.pUserData = NULL;

    result = ma_device_init(&AUDIO.System.context, &config, &AUDIO.System.device);
    if (result != MA_SUCCESS)
    {
        PyErr_SetString(MiniAudioError, "Failed to initialize MiniAudio playback device");
        ma_context_uninit(&AUDIO.System.context);
        return NULL;
    }    

    result = ma_mutex_init(&AUDIO.System.lock);
    if (result != MA_SUCCESS)
    {
        PyErr_SetString(MiniAudioError, "Failed to create MiniAudio mutex for mixing");
        ma_device_uninit(&AUDIO.System.device);
        ma_context_uninit(&AUDIO.System.context);
        return NULL;
    }    

    result = ma_device_start(&AUDIO.System.device);
    if (result != MA_SUCCESS)
    {
        PyErr_SetString(MiniAudioError, "Failed to start playback device");
        ma_device_uninit(&AUDIO.System.device);
        ma_context_uninit(&AUDIO.System.context);
        return NULL;
    }    

    AUDIO.System.isReady = true;
    AUDIO.device_id = NULL;
    
    if (device_id != NULL)
    {
        Py_INCREF(device_id_obj);
        AUDIO.device_id = device_id_obj;
    }    

    Py_RETURN_NONE;
}    

static PyObject *devices = NULL;

static PyObject *AudioDevice_close(PyObject *self, PyObject *args)
{
    if (!AUDIO.System.isReady)
    {
        PyErr_Warn(PyExc_Warning, "AudioDevice was not initialized");
    }
    else
    {
        ma_mutex_uninit(&AUDIO.System.lock);
        ma_device_uninit(&AUDIO.System.device);
        ma_context_uninit(&AUDIO.System.context);

        AUDIO.System.isReady = false;
        free(AUDIO.System.pcmBuffer);
        AUDIO.System.pcmBuffer = NULL;
        AUDIO.System.pcmBufferSize = 0;
        if (AUDIO.device_id != NULL)
        {
            Py_DECREF(AUDIO.device_id);
        }
    }

    if (devices != NULL)
    {
        Py_DECREF(devices);
    }

    Py_RETURN_NONE;
}

static PyObject *AudioDevice_is_ready(PyObject *self, PyObject *args)
{
    return PyBool_FromLong((long)AUDIO.System.isReady);
}

static PyObject *AudioDevice_GetMasterVolume(PyObject *self, void *closure)
{
    float volume;
    ma_device_get_master_volume(&AUDIO.System.device, &volume);
    return PyFloat_FromDouble((double)volume);
}

static int AudioDevice_SetMasterVolume(PyObject *self, PyObject *value, void *closure)
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
        PyErr_SetString(PyExc_TypeError, "master_volume must be float or int object");
        return -1;
    }

    if (volume < 0)
    {
        PyErr_SetString(PyExc_ValueError, "master_volume must be positive");
        return -1;
    }

    ma_device_set_master_volume(&AUDIO.System.device, volume);

    return 0;
}

PyObject *GetDevices(PyObject *m, PyObject *args)
{
    ma_result Device_result;
    ma_device_info *pPlaybackInfos;
    ma_uint32 playbackCount;
    ma_device_info *pUnused;
    ma_uint32 Unused;

    if (devices != NULL)
    {
        Py_INCREF(devices);
        return devices;
    }

    if (!isContextInitialized)
    {
        context_config = ma_context_config_init();
        Device_result = ma_context_init(NULL, 0, &context_config, &AUDIO.System.context);
        
        if (Device_result != MA_SUCCESS)
        {
            PyErr_SetString(MiniAudioError, "Failed to initialize MiniAudio context");
            return NULL;
        }
    }

    Device_result = ma_context_get_devices(&AUDIO.System.context, &pPlaybackInfos, &playbackCount, &pUnused, &Unused);

    if (Device_result != MA_SUCCESS)
    {
        PyErr_SetString(MiniAudioError, "Error on getting devices");
        return NULL;
    }

    devices = PyList_New(playbackCount);

    if (devices == NULL)
    {
        return NULL;
    }

    for (ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice++)
    {
        PyObject *deviceinfo_obj = PyType_GenericNew(&DeviceInfo_Type, NULL, NULL);
        if (deviceinfo_obj == NULL)
        {
            PyList_SetItem(devices, iDevice, Py_None);
            continue;
        }

        ma_device_info *deviceinfo = (ma_device_info *)(deviceinfo_obj + 1);
        (*deviceinfo) = pPlaybackInfos[iDevice];

        PyList_SetItem(devices, iDevice, deviceinfo_obj);
    }

    Py_INCREF(devices);

    return devices;
}

#define GETMETHOD(name) AudioDevice_##name

static PyMethodDef methods[] = {
    METHOD(is_ready, METH_NOARGS),
    METHOD(init, METH_VARARGS),
    METHOD(close, METH_NOARGS),
    {NULL, NULL, 0, NULL}
};

static PyGetSetDef properties[] = {
    {"master_volume", GETMETHOD(GetMasterVolume), GETMETHOD(SetMasterVolume), NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

PyTypeObject AudioDevice_Type = {
    .tp_name = "_audio._AudioDeviceHelper",
    .tp_new = PyType_GenericNew,
    .tp_basicsize = sizeof(PyObject),
    .tp_methods = methods,
    .tp_getset = properties,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE
};
