from threading import Thread
import audio
import time
import os

def update_music(music): #type: (audio.Music) -> None
    while music.is_playing:
        music.update()
        print("Time played:", music.time_played, end="\r", flush=True)
        time.sleep(1/120)

class FilenameType:
    def __fspath__(self):
        return os.path.relpath(os.path.join(__file__, "..", "audio.wav")).encode()

audio.device.init()

filename = FilenameType()

music = audio.Music(filename, ".wav") # uses the ".wav" input as filetype
del music
music = audio.Music(filename) # uses ".wav" automaticly

music.play()

print("Time length:", music.time_length)
print("Is playing:", music.is_playing)
print("Is valid:", music.is_valid)
print("Looping:", music.looping)

t = Thread(target=update_music, args=[music])
t.start()

music.seek(30)

music.volume = float(input("Type the new volume: "))

t.join()

audio.device.close()