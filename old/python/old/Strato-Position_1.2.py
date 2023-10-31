
import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
sys.path.insert(0, '/home/pi/strato2')
from python import fct








gps = serial.Serial('/dev/ttyAMA0', 9600)
usb = serial.Serial('/dev/ttyUSB0', 9600)

time.sleep(1)
gps.reset_input_buffer()
usb.reset_input_buffer()




######################################################################################################




while True:
	
	
	
	#------------- GPS ----------------------------
	
	coor = fct.read_gps(gps, 1)
	
	
	fct.write_file(fct.filename("coor_raw", "csv", 5), "raw", coor, 2)
	fct.transmit(usb, fct.maps_str(coor), 's')
	fct.receive(usb)
	
	
	
	
	
	
	#time.sleep(.1)
