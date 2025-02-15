import audio

devices = audio.get_devices()

for device in devices:
    print(device)