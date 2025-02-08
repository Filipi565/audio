import _audio # type: ignore
from _audio import MiniAudioError # type: ignore
from _audio import DeviceId # type: ignore

MiniAudioError.__module__ = __name__

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
    "AudioDevice",
    "DeviceId"
]

del _AudioDeviceHelper
del _audio