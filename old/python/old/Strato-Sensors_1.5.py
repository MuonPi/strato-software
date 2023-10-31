
import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
sys.path.insert(0, '/home/pi/strato2')
from python import fct









#usb = serial.Serial('/dev/ttyUSB0', 9600)

#time.sleep(1)
#usb.reset_input_buffer()
	
fct.init_sensors()
time.sleep(1)


while True:
	
	
	fct.failsafe_sensors()
	
	
	#-------------- MuonPi -----------------------
	
	mcor = fct.MuonPi.read_mcor()
	mhei = fct.MuonPi.read_mhei()
	
	fct.MuonPi.save_mcor()
	fct.MuonPi.save_mhei()
	
	
	
	
	
	#-------------- ADS-1115 -------------------
	
	volt = 0
	
	
	
	
	
	#------------ MPU-6050 ----------------
	
	gyro = fct.MPU6050.read_gyro()
	acce = fct.MPU6050.read_acce()
	
	fct.MPU6050.save_gyro()
	fct.MPU6050.save_acce()
	
	
	
	
	#------------- SEN-0321 ---------------
	
	ozon = fct.SEN0321.read_ozon()
	
	fct.SEN0321.save_ozon()
	
	
	
	
	#------------- BME-280 ----------------------
	
	pres = fct.BME280.read_pres()
	temp = fct.BME280.read_temp()
	humi = fct.BME280.read_humi()
	
	fct.BME280.save_pres()
	fct.BME280.save_temp()
	fct.BME280.save_humi()
	
	
	
	
	#------------- QMC-5883L ----------------------
	
	magn = fct.QMC5883L.read_magn()
	
	fct.QMC5883L.save_magn()
	
	
	
	
	#------------- VEML-6075 ----------------------
    
	uvse = fct.VEML6075.read_uvse()
	
	fct.VEML6075.save_uvse()
	
	#uvse = [fct.read_word(0x10, 0x07), fct.read_word(0x10, 0x09)]
	
	
	#fct.write_file(fct.filename("uvse_raw", "csv", 5), "raw", uvse, 2)
	
	
	
	
	
	#----------- P A Y L O A D - A R R A Y --------
	
	payload = [mcor, mhei, volt, gyro, acce, ozon, pres, temp, humi, magn, uvse]
	
	print(payload)
	
	time.sleep(.5)
