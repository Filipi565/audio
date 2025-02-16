from audio import device

print(device.is_ready)

device.init()

print(device.is_ready)
print(device.master_volume)

device.master_volume = 10

print(device.master_volume)

device.close()