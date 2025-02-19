import audio
import os

class FilenameType:
    def __fspath__(self):
        return os.path.relpath(os.path.join(__file__, "..", "audio.wav")).encode()

audio.device.init()

filename = FilenameType()

music = audio.Music(filename, ".wav") # uses the ".wav" input as filetype
del music
music = audio.Music(filename) # uses ".wav" automaticly

music.play()

print(music.is_valid)

music.volume = float(input("Type the new volume: "))

while music.is_playing:
    music.update()

audio.device.close()