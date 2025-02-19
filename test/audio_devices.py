import audio

devices = audio.get_devices()

for _ in range(3):
    for device in devices:
        print(device)