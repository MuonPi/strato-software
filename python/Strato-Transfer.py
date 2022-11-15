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
	
	fct.LoRa.init()
	asw = fct.LoRa.answer(5)
	
	pld = fct.LoRa.average_payload()
	msg = fct.LoRa.create_message(pld)
	fct.LoRa.save_message(msg)

	print("created msg: ")
	print(" ".join("%02x" % b for b in msg))

	fct.LoRa.send_message(msg)

	print('answer: ')
	for i in range(3):
		asw = fct.LoRa.answer()
		print(asw)
		if(asw == bytearray(b'EV_TXCOMPLETE')):
			break
	
	# exit(1)

	
	
	
	
	#----------------- END ------------------------
	
	fct.sleep_until(20)