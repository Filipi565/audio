import _audio # type: ignore
from _audio import ( # type: ignore
    MiniAudioError,
    DeviceInfo,
    DeviceId
)
import os

MiniAudioError.__module__ = __name__

def get_devices():
    """
    Retrieve a list of available audio devices.

    This function uses the internal `_get_devices` function from the `_audio` module to fetch the list of audio devices.
    It filters out any None values from the returned list and returns the filtered list.

    Parameters:
    None

    Returns:
    list: A list of `DeviceInfo` objects representing the available audio devices.
    """
    return list(
        filter(lambda obj: obj is not None, _audio._get_devices())
    )

class _AudioDeviceHelper(_audio._AudioDeviceHelper):
    def init(self, audiodevice = None):
        """
        Initialize the audio device helper with the given audio device.

        This method initializes the internal state of the audio device helper using the provided audio device.
        If no audio device is specified, the default audio device will be used.

        Parameters:
        audiodevice (DeviceId, optional): The identifier of the audio device to be used. If not provided, the default audio device will be used.

        Returns:
        None
        """
        super().init(audiodevice)

    def close(self):
        """
        Close the audio device helper.

        This method closes the audio device helper and releases any associated resources.
        After calling this method, the audio device helper should not be used anymore.

        Returns:
        None
        """
        super().close()

class Wave(_audio.Wave):
    def unload(self):
        super().unload()
    
    is_valid = property(_audio.Wave.is_valid)

    @classmethod
    def fromBytes(cls, filetype, buffer):
        if (hasattr(buffer, "__bytes__")):
            buffer = type(buffer).__bytes__(buffer)
        elif (isinstance(buffer, bytearray)):
            buffer = bytes(buffer)
        return _audio._wave_from_bytes(filetype, cls, buffer)

    @classmethod
    def fromFile(cls, filepath):
        path = os.fspath(filepath)
        if (not isinstance(path, str)):
            raise TypeError("filepath must be StrPath")
        
        filetype = path.split(".")[-1]

        with open(filepath, "rb") as f:
            return cls.fromBytes(buffer=f.read(), filetype=f".{filetype}")

class Sound(_audio.Sound):
    def __new__(cls, fp, filetype = None):
        openfp = False

        if isinstance(fp, str) or hasattr(fp, "__fspath__"):
            openfp = True
            fp = os.fspath(fp)
        
        if filetype is None and openfp:
            filetype = fp.split(".")[-1]
        
        if openfp:
            with open(fp, "rb") as f:
                buffer = f.read()
        elif hasattr(fp, "read"):
            buffer = fp.read()
        else:
            buffer = fp

        if filetype is None:
            raise TypeError("filetype is required when loading a sound from bytes")
        
        return cls.fromBytes(filetype, buffer)

    def unload(self):
        super().unload()

    def resume(self):
        super().resume()

    def pause(self):
        super().pause()

    def play(self):
        super().play()

    def stop(self):
        super().stop()
    
    is_playing = property(_audio.Sound.is_playing)
    is_valid = property(_audio.Sound.is_valid)

    @classmethod
    def fromWave(cls, wave):
        return _audio._sound_from_wave(cls, wave)
    
    @classmethod
    def fromFile(cls, filepath):
        return cls.fromWave(Wave.fromFile(filepath))
    
    @classmethod
    def fromBytes(cls, filetype, buffer):
        return cls.fromWave(Wave.fromBytes(filetype, buffer))

AudioDevice = _AudioDeviceHelper()

__all__ = [
    "MiniAudioError",
    "get_devices",
    "AudioDevice",
    "DeviceInfo",
    "DeviceId",
    "Sound",
    "Wave"
]

del _AudioDeviceHelper