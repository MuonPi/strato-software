
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




while True:
	
	
	fct.failsafe_transfers()
	
	
	#---------------- SIM7000E ---------------------
	
	#fct.write_file("send_raw.csv", "raw", payload, 11)
	
	
	fct.SIM7000E.test()
	#fct.SIM7000E.activate_gnss()
	#scor = fct.SIM7000E.read_scor()
	#if scor[0] != 'fail':
	#	fct.SIM7000E.deactivate_gnss()
	#print(scor)
	#fct.SIM7000E.save_scor(scor)
	#fct.SIM7000E.deactivate_gnss()
	#fct.SIM7000E.send_file("/home/pi/strato2/raw/send_raw.csv")
	
	
	
	
	#----------------- LoRa -----------------------
	
	#message = fct.LoRa.create_message(payload2)
	#print(message)
	#print(fct.LoRa.test())
	#fct.LoRa.send(payload)
	#print('LoRa gesendet')
	
	'''l = []
	l = l + ['l']
	print(l)
	print(len(l))
	print(l[0])
	k = l[0].split(';')
	print(k)
	i = 0
	while(i < 100):
		print(int(random.random() * 10000 + 1))
		i += 1'''
		
	
	lora = fct.LoRa.average_payload()
	print(lora)
	fct.LoRa.save_payload(lora)
	msge = fct.LoRa.create_message(lora)
	print(msge)
	fct.LoRa.save_message(msge)
	fct.LoRa.send_message(msge)
	
	
	
	
	
	
	#----------------- END ------------------------
	
	fct.sleep_until(60)
