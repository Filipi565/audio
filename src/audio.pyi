from typing import Union, List

class MiniAudioError(Exception): ...
class DeviceId: ...

class DeviceInfo:
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

class _AudioDeviceHelper:
    def init(self, audiodevice: Union[DeviceId, None] = None) -> None: ...
    def close(self) -> None: ...

    @property
    def master_volume(self) -> float: ...
    @master_volume.setter
    def master_volume(self, other) -> None: ...

AudioDevice: _AudioDeviceHelper

def get_devices() -> List[DeviceInfo]: ...