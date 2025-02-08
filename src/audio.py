import _audio # type: ignore
from _audio import ( # type: ignore
    MiniAudioError,
    DeviceInfo,
    DeviceId
)

MiniAudioError.__module__ = __name__

def get_devices():
    return _audio._get_devices()

class _AudioDeviceHelper(_audio._AudioDeviceHelper):
    def init(self, audiodevice = None):
        super().init(audiodevice)

    def close(self):
        super().close()

    @property
    def master_volume(self):
        return super().master_volume(None)
    
    @master_volume.setter
    def master_volume(self, other):
        if (isinstance(other, int) and not type(other) is bool):
            other = float(other)
        super().master_volume(other)

AudioDevice = _AudioDeviceHelper()

__all__ = [
    "MiniAudioError",
    "get_devices",
    "AudioDevice",
    "DeviceInfo",
    "DeviceId",
]

del _AudioDeviceHelper