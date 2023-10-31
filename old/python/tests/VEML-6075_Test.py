
import time
from datetime import datetime
import smbus
from python import fct









def init():
	
	#------------- VEML-6075 ----------------------
	
	fct.write_byte(0x10, 0x00, 0b00000000)		#integrationtime=50ms; normal dynamic; no trigger; no force-mode; continues-mode
	
	
	

######################################################################################################



init()


while 1:
	
	#------------- VEML-6075 ----------------------
	
	uvse = [fct.read_word(0x10, 0x07), fct.read_word(0x10, 0x09)]
	
	
	fct.write_file(fct.filename("uvse_raw", "csv", 5), "raw", uvse, 2)
	
	
	
	#print(smbus.SMBus(1).read_word_data(0x10, 0x07))
	print (uvse)
	
	
	
	
	
	time.sleep(1)
