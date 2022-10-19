import time
import sys
from datetime import datetime
import smbus
import serial
import pynmea2
#import schedule
sys.path.insert(0, '/home/pi/strato2')
from ADS1x15 import ADS1115
from python import fct



while 1:

	print(fct.adc_read(ADS1115(0x4a), 0, 2/3)/32768*6.144)	#*1.057268722
	
	time.sleep(1)
