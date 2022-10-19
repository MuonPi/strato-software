
import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
sys.path.insert(0, '/home/pi/strato2')
from python import fct









def init_sensors():
	
	#----------- Serial ------------------
	
	#gps = serial.Serial('/dev/ttyAMA0', 9600)
	#usb = serial.Serial('/dev/ttyUSB0', 9600)
	
	#time.sleep(1)
	#gps.reset_input_buffer()
	
	
	
	
	#------------ MPU-6050 ----------------
	
	fct.write_byte(0x68, 0x6b, 0b00000000)	# countinues-mode
	fct.write_byte(0x68, 0x1b, 0b00001000)	# oversampling gyro
	fct.write_byte(0x68, 0x1c, 0b00000000)	# oversampling acce
	
	
	
	
	
	#------------- SEN-0321 ---------------
	
	fct.write_byte(0x73, 0x03, 0x00)	# automatic mode
	
	
	
	
	
	#------------- BME-280 ----------------------
	
	fct.write_byte(0x76, 0xF2, 0b00000010)		# oversampling humidity
	fct.write_byte(0x76, 0xF4, 0b01001011)		# oversampling temperature, oversampling pressure, mode
	
	
	
	
	
	#------------- QMC-5883L ----------------------
	
	fct.write_byte(0x0d, 0x09, 0b10010101)		# samplerate=128Hz; fieldrange=8.1Ga; datarate=50Hz; continues-mode
	fct.write_byte(0x0d, 0x0a, 0b00000001)		# disable soft-reset; disable roll-over; disable interrupt
	fct.write_byte(0x0d, 0x0b, 0b00000001)		# set period
	
	
	
	
	
	#------------- VEML-6075 ----------------------
	
	fct.write_byte(0x10, 0x00, 0b00000000)		# integrationtime=50ms; normal dynamic; no trigger; no force-mode; continues-mode
	
	
	
	
	print('Configs wurden gesetzt')



######################################################################################################


#usb = serial.Serial('/dev/ttyUSB0', 9600)

#time.sleep(1)
#usb.reset_input_buffer()
	
init_sensors()
time.sleep(1)


while True:
	
	#-------------- geoHash -----------------------
	
	geoh = fct.read_logfile(fct.muon_logfile(), "geoHash")
	
	
	
	
	
	#-------------- geoHeightMSL -----------------
	
	high = fct.read_logfile(fct.muon_logfile(), "geoHeightMSL")
	try: 
		high = int(float(high[:-2]))
	except:
		print('no gps from muonpi')
	
	
	
	
	
	#-------------- ADS-1115 -------------------
	
	volt = 0
	
	
	
	

	#------------ MPU-6050 ----------------
	
	fct.MPU6050.init()
	gyro = fct.MPU6050.read_gyro()
	acce = fct.MPU6050.read_acce()
	#print(gyro)
	#print(acce)
	fct.MPU6050.save_gyro()
	fct.MPU6050.save_acce()
	
	
	#gyro = [fct.read_2byte_hl(0x68, 0x3d), fct.read_2byte_hl(0x68, 0x3b), fct.read_2byte_hl(0x68, 0x3f)]
	#acce = [fct.read_2byte_hl(0x68, 0x43), fct.read_2byte_hl(0x68, 0x45), fct.read_2byte_hl(0x68, 0x47)]
	#gyro_grad = gyro[1]/16384.0*180
	
	
	#fct.write_file(fct.filename("gyro_raw", "csv", 5), "raw", gyro, 3)
	#fct.write_file(fct.filename("acce_raw", "csv", 5), "raw", acce, 3)
	
	
	
	
	
	#------------- SEN-0321 ---------------
	
	ozon = fct.read_2byte_hl(0x73, 0x09)


	fct.write_file(fct.filename("ozon_raw", "csv", 5), "raw", ozon)
	
	
	
	
	
	#------------- BME-280 ----------------------
	
	pres = fct.read_3byte_hlx(0x76, 0xF7)
	temp = fct.read_3byte_hlx(0x76, 0xFA)
	humi = fct.read_2byte_hl (0x76, 0xFD)
	
	
	fct.write_file(fct.filename("pres_raw", "csv", 5), "raw", pres)
	fct.write_file(fct.filename("temp_raw", "csv", 5), "raw", temp)
	fct.write_file(fct.filename("humi_raw", "csv", 5), "raw", humi)
	
	
	
	
	
	#------------- QMC-5883L ----------------------
	
	magn = [fct.read_2byte_lh(0x0d, 0x00), fct.read_2byte_lh(0x0d, 0x02), fct.read_2byte_lh(0x0d, 0x04)]
	
	
	fct.write_file(fct.filename("magn_raw", "csv", 5), "raw", magn, 3)
	
	
	
	
	#------------- VEML-6075 ----------------------
	
	uvse = [fct.read_word(0x10, 0x07), fct.read_word(0x10, 0x09)]
	
	
	fct.write_file(fct.filename("uvse_raw", "csv", 5), "raw", uvse, 2)
	
	
	
	
	
	#----------- P A Y L O A D - A R R A Y --------
	
	payload = [geoh, high, volt, gyro, acce, ozon, pres, temp, humi, magn, uvse]
	
	print(payload)
		
	
	
	time.sleep(.5)
