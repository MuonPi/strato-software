#!/usr/bin/env python3
import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
import random
import os
#import sched
sys.path.insert(0, '/home/pi/strato2')
from python import fct



print('Strato-Transfer_starting_...')

fct.set_starttime()
	
fct.init_transfers()
time.sleep(1)


while(True):
	
	
	fct.failsafe_transfers()


	# --------------- LoRa ------------------
	
	#fct.LoRa.init()
	
	pld = fct.LoRa.average_payload()
	msg = fct.LoRa.create_message(pld)
	fct.LoRa.save_message(msg)

	print("created msg: ")
	print(" ".join("%02x" % b for b in msg))

	fct.LoRa.send_message(msg)

	print('answer: ')
	for i in range(6):
		asw = fct.LoRa.answer()
		print(asw)
		if(asw == bytearray(b'EV_TXCOMPLETE')):
			break
	else:
		print('answer timeout')
		fct.LoRa.init()

	sys.stdout.flush() # delete this line if you don't want to write output every 60 seconds

	
	
	
	
	#----------------- END ------------------------
	
	sys.stdout.flush()
	fct.sleep_until(60)
