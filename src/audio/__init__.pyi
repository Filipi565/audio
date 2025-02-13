from typing_extensions import Self, SupportsBytes
from typing import Union, List, final, overload
from _typeshed import StrPath, SupportsRead

__version__: str

_BufferType = Union[bytes, bytearray, SupportsBytes]

class MiniAudioError(Exception): ...

@final
class DeviceId: ...

@final
class DeviceInfo:
    def __repr__(self) -> str: ...

    @property
    def name(self) -> str:
        """
        Returns the name of the audio device.

        :return: A string representing the name of the audio device.
        """

    @property
    def id(self) -> DeviceId:
        """
        Returns the unique identifier of the audio device.

        :return: An instance of the DeviceId class representing the unique identifier of the audio device.
        """

class Wave:
    def __del__(self) -> None: ...

    def unload(self) -> None: ...
    @property
    def is_valid(self) -> bool: ...

    @classmethod
    def fromBytes(cls, filetype: str, buffer: _BufferType) -> Self: ...
    @classmethod
    def fromFile(cls, filepath: StrPath) -> Self: ...

class Sound:
    @overload
    def __new__(cls, fp: StrPath, filetype: Union[str, None] = None) -> Self: ...
    @overload
    def __new__(cls, fp: Union[_BufferType, SupportsRead[bytes]], filetype: str) -> Self: ...
    def __del__(self) -> None: ...

    def unload(self) -> None: ...
    def resume(self) -> None: ...
    def pause(self) -> None: ...
    def play(self) -> None: ...
    def stop(self) -> None: ...

    @property
    def is_playing(self) -> bool: ...
    @property
    def is_valid(self) -> bool: ...
    @property
    def volume(self) -> float: ...
    @volume.setter
    def volume(self, other) -> None: ...

    @classmethod
    def fromWave(cls, wave: Wave) -> Self: ...
    @classmethod
    def fromBytes(cls, filetype: str, buffer: _BufferType) -> Self: ...
    @classmethod
    def fromFile(cls, filepath: StrPath) -> Self: ...

class _AudioDeviceHelper:
    def init(self, audiodevice: Union[DeviceId, None] = None) -> None: ...
    def close(self) -> None: ...

    @property
    def is_ready(self) -> bool: ...

    @property
    def master_volume(self) -> float: ...
    @master_volume.setter
    def master_volume(self, other) -> None: ...

device: _AudioDeviceHelper

def get_devices() -> List[DeviceInfo]: ...

__all__ = [
    "MiniAudioError",
    "get_devices",
    "DeviceInfo",
    "DeviceId",
    "device",
    "Sound",
    "Wave"
]
