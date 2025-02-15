import audio
import time
import os

filename = os.path.relpath(os.path.join(__file__, "..", "audio.wav"))

audio.device.init()

sound = audio.Sound(filename, b".wav")
sound.play()

print(sound.is_valid)

while sound.is_playing:
    time.sleep(.1)

audio.device.close()