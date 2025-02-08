from typing import Union

class MiniAudioError(Exception): ...
class DeviceId: ...

class DeviceInfo:
    @property
    def name(self) -> str: ...
    @property
    def id(self) -> DeviceId: ...

class _AudioDeviceHelper:
    def init(self, audiodevice: Union[DeviceId, None] = None) -> None: ...
    def close(self) -> None: ...

    @property
    def master_volume(self) -> float: ...
    @master_volume.setter
    def master_volume(self, other) -> None: ...

AudioDevice: _AudioDeviceHelper