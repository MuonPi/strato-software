
import time
from datetime import datetime
import smbus
from python import fct









def init():
	
	#------------- QMC-5883l ----------------------
	
	fct.write_byte(0x0d, 0x09, 0b10010101)		#samplerate=128Hz; fieldrange=8.1Ga; datarate=50Hz; continues-mode
	fct.write_byte(0x0d, 0x0a, 0b00000001)		#disable soft-reset; disable roll-over; disable interrupt
	fct.write_byte(0x0d, 0x0b, 0b00000001)		#set period
	
	
	

######################################################################################################



init()


while 1:
	
	#------------- QMC-5883l ----------------------
	
	magn = [fct.read_2byte_lh(0x0d, 0x00), fct.read_2byte_lh(0x0d, 0x02), fct.read_2byte_lh(0x0d, 0x04)]
	
	
	fct.write_file(fct.filename("magn_raw", "csv", 5), "raw", magn, 2)
	
	
	
	print (magn)
	
	
	
	
	
	time.sleep(1)
