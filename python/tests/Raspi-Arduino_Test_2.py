# !!! muss in Verbindung mit Raspi-Arduino_Test.ino auf dem Arduino ausgeführt werden !!!

import serial
import time

s = serial.Serial('/dev/ttyUSB0', 9600) # Namen ggf. anpassen
#s.open()
time.sleep(5) # der Arduino resettet nach einer Seriellen Verbindung, daher muss kurz gewartet werden


print("Arduino ist bereit")
zeichen = bytes("hallo", 'utf-8')
print(str(s.write(zeichen))+" Zeichen wurden gesendet")

try:
	while True:
		response = str(s.read_until())
		print(response)

except KeyboardInterrupt:
	s.close()
