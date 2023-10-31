import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
sys.path.insert(0, '/home/pi/strato2/')
from python import fct




while True:

	#-------------- geoHash -----------------------
	
	geoh = fct.read_logfile(fct.muon_logfile(), "geoHash")
	print(geoh)
	
	
	
	
	#-------------- geoHeightMSL -----------------
	
	geoh = fct.read_logfile(fct.muon_logfile(), "geoHeightMSL")
	print(geoh)
	
	
	
	
	
	
	
	
	
	time.sleep(.5)
