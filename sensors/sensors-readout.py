
import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
import random
import os
# sys.path.insert(0, '/home/pi/strato2')
import fct

# warnings.filterwarnings('ignore')

print('Strato-Sensors_starting_...')

fct.set_starttime()

NUMBER = 0
	
fct.init_sensors()
time.sleep(1)


while True:
	
	
	fct.failsafe_sensors()
	
	
	#-------------- MuonPi -----------------------
	
	# mcor = fct.MuonPi.read_mcor()
	# mhei = fct.MuonPi.read_mhei()
	# mand = fct.MuonPi.read_mand()
	# mxor = fct.MuonPi.read_mxor()
	
	# fct.MuonPi.save_mcor(mcor)
	# fct.MuonPi.save_mhei(mhei)
	# fct.MuonPi.save_mand(mand)
	# fct.MuonPi.save_mxor(mxor)
	
	
	
	
	
	#-------------- ADS-1115 -------------------
	
	volt = fct.ADS_1115.read_volt()
	
	fct.ADS_1115.save_volt(volt)
	
	
	
	
	#------------ MPU-6050 ----------------
	
	gyro = fct.MPU6050.read_gyro()
	acce = fct.MPU6050.read_acce()
	
	fct.MPU6050.save_gyro(gyro)
	fct.MPU6050.save_acce(acce)
	
	
	
	
	#------------- SEN-0321 ---------------
	
	ozon = fct.SEN0321.read_ozon()
	
	fct.SEN0321.save_ozon(ozon)
	
	
	
	
	#------------- BME-280 ----------------------
	
	# --- raw
	temp = fct.BME280.read_temp()
	pres = fct.BME280.read_pres()
	humi = fct.BME280.read_humi()

	fct.BME280.save_pres(pres)
	fct.BME280.save_temp(temp)
	fct.BME280.save_humi(humi)
	
	
	
	
	#------------- QMC-5883L ----------------------
	
	magn = fct.QMC5883L.read_magn()
	tmpi = fct.QMC5883L.read_tmpi()
	
	fct.QMC5883L.save_magn(magn)
	fct.QMC5883L.save_tmpi(tmpi)
	
	
	
	
	#------------- VEML-6075 ----------------------
    
	uvse = fct.VEML6075.read_uvse()
	
	fct.VEML6075.save_uvse(uvse)
	
	
	
	
	#----------- P A Y L O A D - A R R A Y --------
	
	# payload = [mcor, mhei, volt, gyro, acce, ozon, pres, temp, humi, magn, uvse]
	# payload = [mcor[0], mcor[1], mhei[0], volt[0], gyro[0], gyro[1], gyro[2], acce[0], acce[1], acce[2], ozon[0], pres[0], temp[0], humi[0], magn[0], magn[1], magn[2], uvse[0], uvse[1], mand[0], mxor[0]]
	
	# print(payload)
	
	# fct.write_file(fct.filename("payl_raw", "csv", 5), "raw", payload)#, 18)
	# fct.write_file("payl_raw.csv", "raw", payload)#, 18)
	
	
	
	
	#---------------- SIM7000E ---------------------
	
	#fct.SIM7000E.save_payload(fct.SIM7000E.create_payload(payload))
	
	
	
	
	
	#----------------- LoRa -----------------------
	
	#fct.LoRa.save_payload(fct.LoRa.create_payload(payload))
	
	usn = fct.LoRa.uplinkSequenceNo()
	
	pld = fct.LoRa.average_payload()
	fct.LoRa.save_payload(pld)
	fct.LoRa.save_lora_payload(pld)

	# print("created msg: ")
	# print(" ".join("%02x" % b for b in pld))

	# fct.LoRa.send_message(msg)

	# print('answer: ')
	# for i in range(7):
	# 	asw = fct.LoRa.answer(10)
	# 	print(asw)
	# 	if(asw == bytearray(b'EV_TXCOMPLETE')):
	# 		break
	# 	if(i == 5):
	# 		print('answer timeout')
	# 		fct.LoRa.init()

	# sys.stdout.flush() # delete this line if you don't want to write output every 60 seconds

	
	
	
	
	
	#----------------- END ------------------------
	
	NUMBER += 1
	if(NUMBER >= 60):
		sys.stdout.flush()
		NUMBER = 0

	fct.sleep_until(1)
	
	
