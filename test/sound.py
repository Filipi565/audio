import audio
import time
import os

class FilenameType:
    def __fspath__(self):
        return os.path.relpath(os.path.join(__file__, "..", "audio.wav")).encode()

audio.device.init()

filename = FilenameType()

sound = audio.Sound(filename, ".wav") # uses the ".wav" input as filetype
del sound
sound = audio.Sound(filename) # uses ".wav" automaticly

sound.play()

print(sound.is_valid)

while sound.is_playing:
    time.sleep(.1)

audio.device.close()