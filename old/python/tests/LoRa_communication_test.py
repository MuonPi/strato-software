import serial



lor = serial.Serial('/dev/ttyUSB0', 115200)
time.sleep(.1)
lor.reset_input_buffer()


msg = bytearray()
msg = [f9, 2, ]
lor.write