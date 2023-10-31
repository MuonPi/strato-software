
import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
import sched
sys.path.insert(0, '/home/pi/strato2')
from python import fct


starttime = time.time()


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
	
	fct.MuonPi.save_mcor(mcor)
	fct.MuonPi.save_mhei(mhei)
	
	
	
	
	
	#-------------- ADS-1115 -------------------
	
	volt = 0
	
	
	
	
	
	#------------ MPU-6050 ----------------
	
	gyro = fct.MPU6050.read_gyro()
	acce = fct.MPU6050.read_acce()
	
	fct.MPU6050.save_gyro(gyro)
	fct.MPU6050.save_acce(acce)
	
	
	
	
	#------------- SEN-0321 ---------------
	
	ozon = fct.SEN0321.read_ozon()
	
	fct.SEN0321.save_ozon(ozon)
	
	
	
	
	#------------- BME-280 ----------------------
	
	pres = fct.BME280.read_pres()
	temp = fct.BME280.read_temp()
	humi = fct.BME280.read_humi()
	
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
	
	payload = [mcor, mhei, volt, gyro, acce, ozon, pres, temp, humi, magn, uvse]
	
	print(payload)
	
	fct.write_file(fct.filename("payl_raw", "csv", 1440), "raw", payload, 11)
	
	
	
	
	#---------------- SIM7000E ---------------------
	
	fct.write_file("send_raw.csv", "raw", payload, 11)
	
	
	#fct.SIM7000E.test()
	fct.SIM7000E.activate_gnss()
	scor = fct.SIM7000E.read_scor()
	#if scor[0] != 'fail':
	#	fct.SIM7000E.deactivate_gnss()
	print(scor)
	#fct.SIM7000E.save_scor(scor)
	#fct.SIM7000E.deactivate_gnss()
	#fct.SIM7000E.send_file("/home/pi/strato2/raw/send_raw.csv")
	
	
	
	time.sleep(1 - ((time.time() - starttime) % 1))
	#time.sleep(1)
	#print(datetime.utcnow().strftime('%H:%M:%S.%f')[:-3])
