
import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
sys.path.insert(0, '/home/pi/strato2')
from python import fct










#----------- SIM7000E ------------------
	
usb = serial.Serial('/dev/ttyUSB3', 115200)
time.sleep(1)
usb.reset_input_buffer()

def answer(tmo = 10):
	now = time.time()
	while time.time() < now + tmo:
		time.sleep(.1)
		if usb.in_waiting:
			print(usb.readline().decode().strip())
			return
		#time.sleep(.1)
	
def init():
	usb.write('AT\r'.encode())			# prüfen on ansprechbar
	answer()
	usb.write('AT+CGNSHOT\r'.encode())	# hotstart einschalten
	answer()
	usb.write('AT+CGNSPWR=1\r'.encode())	# GNSS einschalten
	answer()
	usb.write('AT+CNMP=2\r'.encode())	# Mode Selection auf automatisch (GSM und LTE)
	answer()
	usb.write('AT+CMNB=3\r'.encode())	# Selection auf CAT-M and NB-IoT
	answer()
	usb.write('AT+CSQ\r'.encode())	# Signalstärke auslesen
	answer()
	usb.write('AT+CBC\r'.encode())	# Akkuspannung auslesen
	answer()
	print('bereit')
	
def end():
	usb.write('AT\r'.encode())
	answer()
	usb.write('AT+CGNSPWR=0\r'.encode())
	answer()
	print('ende')



######################################################################################################




init()

lin = ''

while len(lin) <= 30:
	


	#----------- SIM7000E ------------------
	
	usb.write('AT+CGNSINF\r'.encode())
	while usb.in_waiting:
		dat = usb.readline().decode()
		if dat[0:9] == "+CGNSINF:":
			lat = dat.split(',')[3]
			lon = dat.split(',')[4]
			lin = 'https://www.google.de/maps?q=' + str(lat) + "," + str(lon)
			#print(lin)
		#time.sleep(.1)
	
	
	time.sleep(2)


end()

