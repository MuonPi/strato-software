# !!! muss in Verbindung mit Arduino_Decoding.ino auf dem Arduino ausgeführt werden !!!

import serial
import time

usb = serial.Serial('/dev/ttyUSB0', 9600) # Namen ggf. anpassen
time.sleep(5) # der Arduino resettet nach einer Seriellen Verbindung, daher muss kurz gewartet werden
print("Arduino ist bereit")




try:
	while True:
		
		trm = bytes("s;SIM #l;LoRa#l;LoRa#s;SIM #l;LoRa#s;SIM #s;SIM #l;LoRa#nichts#", 'utf-8') # transmitting: zu sendende Zeichen
		print("\n" + str(usb.write(trm))+" Zeichen wurden gesendet")
		
		time.sleep(0.1)		
		
		while usb.in_waiting: # length(usb.read()) != 0
			rcv = str(usb.read_until(b'#')) # receiving: empfangene Zeichen
			print(rcv)
		

except KeyboardInterrupt:
	usb.close()
