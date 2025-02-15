import audio
import os

filename = os.path.relpath(os.path.join(__file__, "..", "audio.wav"))

wave = audio.Wave.fromFile(filename)

print(wave.is_valid)