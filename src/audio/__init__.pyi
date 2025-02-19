from typing import Union, final, overload, Iterator, AnyStr
from typing_extensions import Self, SupportsBytes
from _typeshed import GenericPath, SupportsRead

__version__: str

_BufferType = Union[bytes, bytearray, SupportsBytes]
_BytesPath = GenericPath[bytes]
_StrPath = GenericPath[str]

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

@final
class DeviceInfoIterator(Iterator[DeviceInfo]):
    def __getitem__(self, index: int, /) -> DeviceInfo: ...
    def __next__(self) -> DeviceInfo: ...
    def __iter__(self) -> Self: ...
    def __len__(self) -> int: ...

class Wave:
    def unload(self) -> None: ...
    @property
    def is_valid(self) -> bool: ...

    @classmethod
    def fromBytes(cls, filetype: str, buffer: _BufferType) -> Self: ...
    @classmethod
    def fromFile(cls, filepath: _StrPath) -> Self: ...

class Sound:
    @overload
    def __new__(cls, fp: _BytesPath, filetype: Union[str, bytes, None] = None) -> Self: ...
    @overload
    def __new__(cls, fp: _StrPath, filetype: Union[str, bytes, None] = None) -> Self: ...
    @overload
    def __new__(cls, fp: Union[_BufferType, SupportsRead[bytes]], filetype: str) -> Self: ...

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
    def fromFile(cls, filepath: _StrPath) -> Self: ...

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

def get_devices() -> DeviceInfoIterator: ...

__all__ = [
    "DeviceInfoIterator",
    "MiniAudioError",
    "get_devices",
    "DeviceInfo",
    "DeviceId",
    "device",
    "Sound",
    "Wave"
]
