# Audio

This module is a simple way of playing audio on your OS

Example:

```python
import audio
import time

audio.device.init()

sound = audio.Sound("my_sound.wav")
if sound.is_valid:
    sound.play()

while sound.is_playing:
    time.sleep(.1)

audio.device.close()
```