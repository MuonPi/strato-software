
import serial
import time
import pynmea2


gps = serial.Serial('/dev/ttyAMA0', 9600)

time.sleep(1)
gps.reset_input_buffer()


try:
	while True:	
		while gps.in_waiting:
			dat = gps.readline().decode('utf-8')
			#print(dat)
			if dat[0:6] == "$GPRMC":
				cor = pynmea2.parse(dat) # koordinaten
				lat = cor.latitude
				lon = cor.longitude
				#print('Latitude= ' + str(lat))
				#print('Longitude= ' + str(lon))
				#print(cor)
				print('https://www.google.de/maps?q=' + str(lat) + "," + str(lon))

except KeyboardInterrupt:
	gps.close()
