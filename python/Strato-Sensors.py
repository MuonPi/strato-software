
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



print('Strato-Sensors_starting_...')

fct.set_starttime()
	
fct.init_sensors()
time.sleep(1)


while True:
	
	
	fct.failsafe_sensors()
	
	
	#-------------- MuonPi -----------------------
	
	mcor = fct.MuonPi.read_mcor()
	mhei = fct.MuonPi.read_mhei()
	mand = fct.MuonPi.read_mand()
	mxor = fct.MuonPi.read_mxor()
	
	fct.MuonPi.save_mcor(mcor)
	fct.MuonPi.save_mhei(mhei)
	fct.MuonPi.save_mand(mand)
	fct.MuonPi.save_mxor(mxor)
	
	
	
	
	
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
	
	temp = []
	adc_T = float(fct.BME280.read_temp()[0])
	var1 = ((adc_T/16384.0 - float(fct.BME280.dig_T1)/1024.0)) * float(fct.BME280.dig_T2)
	var2 = ((adc_T / 131072.0 - float(fct.BME280.dig_T1) / 8192.0) * (adc_T / 131072.0 - float(fct.BME280.dig_T1) / 8192.0)) * float(fct.BME280.dig_T3)
	t_fine = var1 + var2 # as BME280_S32_t (signed 32bit)
	temp = [(var1 + var2) / 5120.0]

	pres = [0.0]
	adc_P = float(fct.BME280.read_pres()[0])
	var1 = t_fine / 2.0 - 64000.0
	var2 = var1 * var1 * float(fct.BME280.dig_P6) / 32768.0
	var2 = var2 + var1 * float(fct.BME280.dig_P5) * 2.0
	var2 = (var2 / 4.0) + float(fct.BME280.dig_P4) * 65536.0
	var1 = (float(fct.BME280.dig_P3) * var1 * var1 / 524288.0 + float(fct.BME280.dig_P2) * var1 ) / 524288.0
	if var1 != 0.0:
		pres = 1048576.0 - adc_P
		pres = (pres - var2 / 4096.0) * 6250.0 / var1
		var1 = float(fct.BME280.dig_P9) * pres * pres / 2147483648.0
		var2 = pres * fct.BME280.dig_P8 / 32768.0
		pres = pres + (var1 + var2 + float(fct.BME280.dig_P7)) / 16.0
		pres = [pres]

	humi = []
	adc_H = float(fct.BME280.read_humi()[0])
	var_H = t_fine - 76800.0
	var_H = (adc_H - (fct.BME280.dig_H4 * 64.0 + fct.BME280.dig_H5 / 16384.0 * var_H)) * (fct.BME280.dig_H2 / 65536.0 * (1.0 + fct.BME280.dig_H6 / 67108864.0 * var_H * (1.0 + fct.BME280.dig_H3 / 67108864.0 * var_H)))
	var_H = var_H * (1.0 - fct.BME280.dig_H1 * var_H / 524288.0)
	if (var_H > 100.0):
		var_H = 100.0
	elif var_H < 0.0:
		var_H= 0.0
	humi = [var_H]

	fct.BME280.save_pres(pres)
	fct.BME280.save_temp(temp)
	fct.BME280.save_humi(humi)
	
	
	
	
	#------------- QMC-5883L ----------------------
	
	magn = fct.QMC5883L.read_magn()
	
	fct.QMC5883L.save_magn(magn)
	
	
	
	
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
	
	
	
	
	
	#----------------- END ------------------------
	
	fct.sleep_until(1)
	
	
