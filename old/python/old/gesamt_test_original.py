
import time
from datetime import datetime
import smbus
from ADS1x15 import ADS1115
import fct









def init():
	
	#------------ mpu6050 ----------------
	
	# Aktivieren, um das Modul ansprechen zu koennen
	fct.write_byte(smbus.SMBus(1), 0x68, 0x6b, 0x00)
	# Empfindlichkeit Gyro setzen
	fct.write_byte(smbus.SMBus(1), 0x68, 0x1b, 0b00001000)
	# Empfindlichkeit Acce setzen
	fct.write_byte(smbus.SMBus(1), 0x68, 0x1c, 0b00000000)
	
	
	
	
	#------------- dfrobot-sen0321 ---------------
	
	fct.write_byte(smbus.SMBus(1), 0x72, 0x03, 0x00)	# automatic mode
	
	
	
	
	
	#------------- bme280 ----------------------
	
	fct.write_byte(smbus.SMBus(1), 0x76, 0xF2, 0b00000010)		#oversampling humidity
	fct.write_byte(smbus.SMBus(1), 0x76, 0xF4, 0b01001011)		#oversampling temperature, oversampling pressure, mode
	
	
	
	
	
	#------------- hmc5883 ----------------------
	
	fct.write_byte(smbus.SMBus(1), 0x1e, 0x0, 0b01110000) # Rate: 8 samples @ 15Hz
	fct.write_byte(smbus.SMBus(1), 0x1e, 0x1, 0b01100000) # Sensor field range: 8.1 Ga
	fct.write_byte(smbus.SMBus(1), 0x1e, 0x2, 0b00000000) # Mode: Continuous sampling



######################################################################################################



init()


while 1:

	#------------ mpu6050 ----------------
	
	gyro = [fct.read_word_2c(smbus.SMBus(1), 0x68, 0x3d), fct.read_word_2c(smbus.SMBus(1), 0x68, 0x3b), fct.read_word_2c(smbus.SMBus(1), 0x68, 0x3f)]
	acce = [fct.read_word_2c(smbus.SMBus(1), 0x68, 0x43), fct.read_word_2c(smbus.SMBus(1), 0x68, 0x45), fct.read_word_2c(smbus.SMBus(1), 0x68, 0x47)]
	gyro_grad = gyro[1]/16384.0*180
	
	
	fct.write_file(fct.filename("gyro_raw", "csv", 5), "raw", gyro, 3)
	fct.write_file(fct.filename("acce_raw", "csv", 5), "raw", acce, 3)
	
	
	
	
	
	#------------ guva-s12sd ------------------
	
	snt = 2
	uvse = fct.adc_read(ADS1115(0x4a), 0, snt)
	

	fct.write_file(fct.filename("uvse_raw", "csv", 5), "raw", uvse)
	
	
	
	
	
	#------------- dfrobot-sen0321 ---------------
	
	ozon = fct.read_word(smbus.SMBus(1), 0x72, 0x09)


	fct.write_file(fct.filename("ozon_raw", "csv", 5), "raw", ozon)
	
	
	
	
	
	#------------- bme280 ----------------------
	
	pres = fct.read_3byte(smbus.SMBus(1), 0x76, 0xF7)
	temp = fct.read_3byte(smbus.SMBus(1), 0x76, 0xFA)
	humi = fct.read_word(smbus.SMBus(1), 0x76, 0xFD)
	
	
	fct.write_file(fct.filename("pres_raw", "csv", 5), "raw", pres)
	fct.write_file(fct.filename("temp_raw", "csv", 5), "raw", temp)
	fct.write_file(fct.filename("humi_raw", "csv", 5), "raw", humi)
	
	
	
	
	
	#------------- hmc5883 ----------------------
	
	magn = [fct.read_word(smbus.SMBus(1), 0x1E, 0x03), fct.read_word(smbus.SMBus(1), 0x1E, 0x07), fct.read_word(smbus.SMBus(1), 0x1E, 0x05)]
	
	
	fct.write_file(fct.filename("magn_raw", "csv", 5), "raw", magn, 3)
	
	
	
	
	
	
	
	
	
	
	time.sleep(.1)
