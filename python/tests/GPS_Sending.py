
import serial
import time
import pynmea2


gps = serial.Serial('/dev/ttyAMA0', 9600)
usb = serial.Serial('/dev/ttyUSB0', 9600)

time.sleep(1)
gps.reset_input_buffer()


def Transmit(trm, dev):
	trm = dev + ';' + trm + '#'
	trm = bytes(trm, 'utf-8') # transmitting: zu sendende Zeichen
	print("\n" + str(usb.write(trm))+" Zeichen wurden gesendet")


def Receive():
	while usb.in_waiting:
		rcv = usb.read_until(b'#').decode('utf-8') # receiving: empfangene Zeichen
		rcv = rcv[:-1]
		print(rcv)


########################################################################

try:
	while True:	
		while gps.in_waiting:
			dat = gps.readline().decode('utf-8')
			#print(dat)
			if dat[0:6] == "$GPRMC":
				cor = pynmea2.parse(dat) # koordinaten
				lat = cor.latitude
				lon = cor.longitude
				Transmit('https://www.google.de/maps?q=' + str(lat) + "," + str(lon), 's')
				time.sleep(0.2)
				Receive()

except KeyboardInterrupt:
	gps.close()

