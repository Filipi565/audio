import _audio # type: ignore
from _audio import ( # type: ignore
    DeviceInfoIterator,
    MiniAudioError,
    DeviceInfo,
    DeviceId
)
import os

__version__ = "1.2.1"

MiniAudioError.__module__ = __name__

def _register_deviceinfoiterator():
    from collections.abc import Iterator
    Iterator.register(DeviceInfoIterator)

_register_deviceinfoiterator()
del _register_deviceinfoiterator

def get_devices():
    """
    Retrieve a list of available audio devices as a DeviceInfoIterator object.

    This function returns the result of _audio._get_devices.

    Parameters:
    None

    Returns:
    DeviceInfoIterator: A list-like of `DeviceInfo` objects representing the available audio devices.
    """
    return _audio._get_devices()

class _AudioDeviceHelper(_audio._AudioDeviceHelper):
    is_ready = property(_audio._AudioDeviceHelper.is_ready)

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
            if (isinstance(fp, str)):
                filetype = "." + fp.split(".")[-1]
            else:
                filetype = (b"." + fp.split(b".")[-1]).decode()
        
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
    
class Music(_audio.Music):
    def __new__(cls, fp, filetype = None):
        openfp = False

        if isinstance(fp, str) or hasattr(fp, "__fspath__"):
            openfp = True
            fp = os.fspath(fp)
        
        if filetype is None and openfp:
            if (isinstance(fp, str)):
                filetype = "." + fp.split(".")[-1]
            else:
                filetype = (b"." + fp.split(b".")[-1]).decode()
        
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

    time_length = property(_audio.Music.time_length)
    time_played = property(_audio.Music.time_played)
    is_valid = property(_audio.Music.is_valid)

    @classmethod
    def fromFile(cls, filepath):
        path = os.fspath(filepath)
        if (not isinstance(path, str)):
            raise TypeError("filepath must be StrPath")
        
        filetype = path.split(".")[-1]

        with open(filepath, "rb") as f:
            return cls.fromBytes(buffer=f.read(), filetype=f".{filetype}")

    @classmethod
    def fromBytes(cls, filetype, buffer):
        if (hasattr(buffer, "__bytes__")):
            buffer = type(buffer).__bytes__(buffer)
        elif (isinstance(buffer, bytearray)):
            buffer = bytes(buffer)
        return _audio._music_from_bytes(filetype, cls, buffer)

device = _AudioDeviceHelper()

__all__ = [
    "DeviceInfoIterator",
    "MiniAudioError",
    "get_devices",
    "DeviceInfo",
    "DeviceId",
    "device",
    "Sound",
    "Music",
    "Wave"
]

del _AudioDeviceHelper