
import time
from datetime import datetime
import smbus
import serial
import pynmea2
import random
import os
#import cayenneLPP
#import schedule
from ADS1x15 import ADS1115


#---------------- I2C Commands --------------------

def read_byte(adr, reg, bus = smbus.SMBus(1)):					#bus=i2c bus nummer; adr=adresse sensor; reg=register sensor
	return bus.read_byte_data(adr, reg)			#bus=aus smbus SMBus(1)

def read_byte_2c(adr, reg, bus = smbus.SMBus(1)):				#als 2er-komplement auslesen
	tmp = read_byte(adr, reg)
	if (tmp >= 0x80):
		return -((255 - tmp) + 1)
	else:
		return tmp

def read_2byte_hl(adr, reg, bus = smbus.SMBus(1)):					#als 1er-komplement auslesen zuerst highbyte dann lowbyte
	h = bus.read_byte_data(adr, reg)
	l = bus.read_byte_data(adr, reg+1)
	tmp = (h << 8) + l
	return tmp

def read_2byte_hl_2c(adr, reg, bus = smbus.SMBus(1)):				#als 2er-komplement auslesen
	tmp = read_2byte_hl(adr, reg)
	if (tmp >= 0x8000):
		return -((65535 - tmp) + 1)
	else:
		return tmp

def read_2byte_lh(adr, reg, bus = smbus.SMBus(1)):					#als 1er-komplement auslesen zuerst highbyte dann lowbyte
	l = bus.read_byte_data(adr, reg)
	h = bus.read_byte_data(adr, reg+1)
	tmp = (h << 8) + l
	return tmp

def read_2byte_lh_2c(adr, reg, bus = smbus.SMBus(1)):				#als 2er-komplement auslesen
	tmp = read_2byte_lh(adr, reg)
	if (tmp >= 0x8000):
		return -((65535 - tmp) + 1)
	else:
		return tmp

def read_3byte_hlx(adr, reg, bus = smbus.SMBus(1)):
	h = bus.read_byte_data(adr, reg)
	l = bus.read_byte_data(adr, reg+1)
	x = bus.read_byte_data(adr, reg+2)
	tmp = (h << 16) + (l << 8) + x
	return tmp

def read_3byte_hlx_2c(adr, reg, bus = smbus.SMBus(1)):
	tmp = read_3byte(adr, reg)
	if (tmp >= 0x800000):
		return -((16777215 - tmp) + 1)
	else:
		return tmp

def read_word(adr, reg, bus = smbus.SMBus(1)):
	return bus.read_word_data(adr, reg)

def write_byte(adr, reg, cmd, bus = smbus.SMBus(1)):            #cmd=command
	return bus.write_byte_data(adr, reg, cmd)



#------------------ Sensors ----------------------------

class MPU6050:
	def init():
		try:
			write_byte(0x68, 0x6b, 0b00000000)  # countinues-mode
			write_byte(0x68, 0x1b, 0b00001000)	# oversampling gyro
			write_byte(0x68, 0x1c, 0b00000000)	# oversampling acce
			print('MPU6050_init_done')
		except:
			print('MPU6050_init_fail')
			global sensorfail
			sensorfail[1] = 1
	def read_gyro():
		try:
			return [read_2byte_hl(0x68, 0x3d), read_2byte_hl(0x68, 0x3b), read_2byte_hl(0x68, 0x3f)]
		except:
			print('MPU6050_gyro_read_fail')
			MPU6050.init()
			return ['fail','fail','fail']
	def read_acce():
		try:
			return [read_2byte_hl(0x68, 0x43), read_2byte_hl(0x68, 0x45), read_2byte_hl(0x68, 0x47)]
		except:
			print('MPU6050_acce_read_fail')
			MPU6050.init()
			return ['fail','fail','fail']
	def save_gyro(dat):
		try:
			write_file(filename("gyro_raw", "csv", 5), "raw", dat)#, 3)
		except:
			print('MPU6050_gyro_save_fail')
	def save_acce(dat):
		try:
			write_file(filename("acce_raw", "csv", 5), "raw", dat)#, 3)
		except:
			print('MPU6050_acce_save_fail')


class SEN0321:
	def init():
		try:
			write_byte(0x73, 0x03, 0x00)    #automatic mode
			print('SEN0321_init_done')
		except:
			print('SEN0321_init_fail')
			global sensorfail
			sensorfail[1] = 1
	def read_ozon():
		try:
			return [read_2byte_hl(0x73, 0x09)]
		except:
			print('SEN0321_ozon_read_fail')
			SEN0321.init()
			return ['fail']#'fail'
	def save_ozon(dat):
		try:
			write_file(filename("ozon_raw", "csv", 5), "raw", dat)
		except:
			print('SEN0321_ozon_save_fail')



class BME280:
	dig_T1 = None
	dig_T2 = None
	dig_T3 = None
	dig_P1 = None
	dig_P2 = None
	dig_P3 = None
	dig_P4 = None
	dig_P5 = None
	dig_P6 = None
	dig_P7 = None
	dig_P8 = None
	dig_P9 = None
	dig_H1 = None
	dig_H2 = None
	dig_H3 = None
	dig_H4 = None
	dig_H5 = None
	def init():
		try:
			write_byte(0x76, 0xF2, 0b00000010)  # oversampling humidity)
			write_byte(0x76, 0xF4, 0b01001011)  # oversampling temperature, oversampling pressure, mode
			BME280.dig_T1 = read_2byte_hl(0x76, 0x88)
			BME280.dig_T2 = read_2byte_hl_2c(0x76, 0x8A)
			BME280.dig_T3 = read_2byte_hl_2c(0x76, 0x8C)
			BME280.dig_P1 = read_2byte_hl(0x76, 0x8E)
			BME280.dig_P2 = read_2byte_hl_2c(0x76, 0x90)
			BME280.dig_P3 = read_2byte_hl_2c(0x76, 0x92)
			BME280.dig_P4 = read_2byte_hl_2c(0x76, 0x94)
			BME280.dig_P5 = read_2byte_hl_2c(0x76, 0x96)
			BME280.dig_P6 = read_2byte_hl_2c(0x76, 0x98)
			BME280.dig_P7 = read_2byte_hl_2c(0x76, 0x9A)
			BME280.dig_P8 = read_2byte_hl_2c(0x76, 0x8C)
			BME280.dig_P9 = read_2byte_hl_2c(0x76, 0x9E)
			BME280.dig_H1 = read_byte(0x76, 0xA1)
			BME280.dig_H2 = read_2byte_hl_2c(0x76, 0xE1)
			BME280.dig_H3 = read_byte(0x76, 0xE3)

			e5 = read_byte(0x76, 0xE5)
			BME280.dig_H4 = (read_byte(0x76, 0xE4) << 4) + (e5 & 0x07)
			if (BME280.dig_H4 >= 0x8000):
				BME280.dig_H4 = -((65535 - BME280.dig_H4) + 1)
			BME280.dig_H5 = (read_2byte_hl(0x76, 0xE6) << 4) + (e5 & 0x70)
			if (BME280.dig_H5 >= 0x8000):
				BME280.dig_H5 = -((65535 - BME280.dig_H5) + 1)

			print('BME280_init_done')
		except:
			print('BME280_init_fail')
			global sensorfail
			sensorfail[1] = 1
	def read_pres():
		try:
			return [read_3byte_hlx(0x76, 0xF7)]
		except:
			print('BME280_pres_read_fail')
			BME280.init()
			return ['fail']#'fail'
	def read_temp():
		try:
			return [read_3byte_hlx(0x76, 0xFA)]
		except:
			print('BME280_temp_read_fail')
			BME280.init()
			return ['fail']#'fail'
	def read_humi():
		try:
			return [read_2byte_hl (0x76, 0xFD)]
		except:
			print('BME280_humi_read_fail')
			BME280.init()
			return ['fail']#'fail'
	def save_pres(dat):
		try:
			write_file(filename("pres_raw", "csv", 5), "raw", dat)
		except:
			print('BME280_pres_save_fail')
	def save_temp(dat):
		try:
			write_file(filename("temp_raw", "csv", 5), "raw", dat)
		except:
			print('BME280_temp_save_fail')
	def save_humi(dat):
		try:
			write_file(filename("humi_raw", "csv", 5), "raw", dat)
		except:
			print('BME280_humi_save_fail')



class QMC5883L:
	def init():
		try:
			write_byte(0x0d, 0x09, 0b10010101)  # samplerate=128Hz; fieldrange=8.1Ga; datarate=50Hz; continues-mode
			write_byte(0x0d, 0x0a, 0b00000001)  # disable soft-reset; disable roll-over; disable interrupt
			write_byte(0x0d, 0x0b, 0b00000001)  # set period
			print('QMC5883L_init_done')
		except:
			print('QMC5883L_init_fail')
			global sensorfail
			sensorfail[1] = 1
	def read_magn():
		try:
			return [read_2byte_lh(0x0d, 0x00), read_2byte_lh(0x0d, 0x02), read_2byte_lh(0x0d, 0x04)]
		except:
			print('QMC5883L_magn_read_fail')
			QMC5883L.init()
			return ['fail', 'fail', 'fail']
	def save_magn(dat):
		try:
			write_file(filename("magn_raw", "csv", 5), "raw", dat)#, 3)
		except:
			print('QMC5883L_magn_save_fail')



class VEML6075:
	def init():
		try:
			write_byte(0x10, 0x00, 0b00000000)  # integrationtime=50ms; normal dynamic; no trigger; no force-mode; continues-mode
			print('VEML6075_init_done')
		except:
			print('VEML6075_init_fail')
			global sensorfail
			sensorfail[1] = 1
	def read_uvse():
		try:
			return [read_word(0x10, 0x07), read_word(0x10, 0x09)]
		except:
			print('VEML6075_uvse_read_fail')
			VEML6075.init()
			return ['fail', 'fail']
	def save_uvse(dat):
		try:
			write_file(filename("uvse_raw", "csv", 5), "raw", dat)#, 2)
		except:
			print('VEML6075_uvse_save_fail')




class ADS_1115:
	def init():
		try:
			################ fehlt ###################
			print('ADS1115_init_done')
			ADS1115(0x4A).read_adc(0, 2/3, 128)
		except:
			print('ADS1115_init_fail')
			global sensorfail
			sensorfail[1] = 1
	def read_volt():
		try:
			return [ADS1115(0x4A).read_adc(0, 2/3, 128)]
			#adc_read(ADS1115(0x4A), 0, 2/3)
		except:
			print('ADS1115_volt_read_fail')
			ADS_1115.init()
			return ['fail']#'fail'
	def save_volt(dat):
		try:
			write_file(filename("volt_raw", "csv", 5), "raw", dat)
		except:
			print('ADS1115_volt_save_fail')




class MuonPi:
	def read_mhas():
		try:
			return [read_logfile(muon_logfile(), "geoHash").strip()]
		except:
			print('geoHash_read_fail')
			return ['fail']#'fail'
	def save_mhas(dat):
		try:
			write_file(filename("mhas_raw", "csv", 5), "raw", dat)
		except:
			print('geoHash_save_fail')
	def read_mhei():
		try:
			return [read_logfile(muon_logfile(), "geoHeightMSL")[:-1].strip()]
		except:
			print('geoHeightMSL_read_fail')
			return ['fail']#'fail'
	def save_mhei(dat):
		try:
			write_file(filename("mhei_raw", "csv", 5), "raw", dat)
		except:
			print('geoHeightMSL_save_fail')
	def read_mcor():
		try:
			return [read_logfile(muon_logfile(), "geoLatitude")[:-3].strip(), read_logfile(muon_logfile(), "geoLongitude")[:-3].strip()]
		except:
			print('geoLatitude/geoLongitude_read_fail')
			return ['fail', 'fail']#'fail'
	def save_mcor(dat):
		try:
			write_file(filename("mcor_raw", "csv", 5), "raw", dat)
		except:
			print('geoLatitude/geoLongitude_save_fail')
	def read_mand():
		try:
			return [read_logfile(muon_logfile(), "rateAND")[:-2].strip()]
		except:
			print('rateAND_read_fail')
			return ['fail']#'fail'
	def save_mand(dat):
		try:
			write_file(filename("mand_raw", "csv", 5), "raw", dat)
		except:
			print('rateAND_save_fail')
	def read_mxor():
		try:
			return [read_logfile(muon_logfile(), "rateXOR")[:-2].strip()]
		except:
			print('rateXOR_read_fail')
			return ['fail']#'fail'
	def save_mxor(dat):
		try:
			write_file(filename("mxor_raw", "csv", 5), "raw", dat)
		except:
			print('rateXOR_save_fail')
	def read_coun():
		try:
			return [read_logfile(muon_logfile(), "ubloxCounter")[:-2].strip()]
		except:
			print('ubloxCounter_read_fail')
			return ['fail']#'fail'
	def save_mxor(dat):
		try:
			write_file(filename("coun_raw", "csv", 5), "raw", dat)
		except:
			print('ubloxCounter_save_fail')
	def read_temi():
		try:
			return [read_logfile(muon_logfile(), "temperature")[:-2].strip()]
		except:
			print('temperature_read_fail')
			return ['fail']#'fail'
	def save_temi(dat):
		try:
			write_file(filename("temi_raw", "csv", 5), "raw", dat)
		except:
			print('temperature_save_fail')




'''
class SIM7000E:
	sim = None
	def init(prt = 'ttyUSB3'):
		try:
			try:
				SIM7000E.sim.close()
				print('SIM7000E_handle_closed')
			except:
				None
			SIM7000E.sim = serial.Serial('/dev/' + prt, 115200)
			time.sleep(.1)
			SIM7000E.sim.reset_input_buffer()
			#sim.write('AT\r'.encode())
			#SIM7000E.answer()
			SIM7000E.sim.write('AT+CGNSHOT\r'.encode())	# hotstart einschalten
			SIM7000E.answer()
			#### fehlt ####################
			print('SIM7000E_init_done')
		except:
			try:
				print('SIM7000E_init_fail')
				global transferfail
				transferfail[1] = 1
				SIM7000E.sim.close()
			except:
				print('SIM7000E_close_fail')
	def answer(tmo = 10):
		try:
			asw = []
			now = time.time()
			while time.time() < now + tmo:
				while SIM7000E.sim.in_waiting:
					time.sleep(.2)
					#asw += sim.readline().decode().strip()
					tmp = SIM7000E.sim.readline().decode().strip()
					if tmp != '':
						asw = asw + [tmp]
					if SIM7000E.sim.in_waiting == 0:
						#print(asw)
						return asw #.strip()
					#asw = asw + ';'
			print('SIM7000E_answer_timeout')
			return ['fail']
		except:
			print('SIM7000E_answer_fail')
			SIM7000E.init()
			return ['fail']
	def test():
		try:
			SIM7000E.sim.write('AT\r'.encode())			# prüfen ob ansprechbar
			return SIM7000E.answer()
		except:
			print('SIM7000E_test_fail')
			SIM7000E.init()
			return ['fail']
	def manual(msg):
		try:
			msg = msg + '\r'
			SIM7000E.sim.write(msg.encode())
			return SIM7000E.answer()
		except:
			print('SIM7000E_manual_fail')
			SIM7000E.init()
			return ['fail']
	def activate_gnss():
		try:
			SIM7000E.sim.write('AT+CGNSHOT\r'.encode())	# hotstart einschalten
			SIM7000E.answer()
			SIM7000E.sim.write('AT+CGNSPWR=1\r'.encode())	# GNSS einschalten
			SIM7000E.answer()
		except:
			print('SIM7000E_activate_gnss_fail')
			SIM7000E.init()
			return ['fail']
	def deactivate_gnss():
		try:
			SIM7000E.sim.write('AT+CGNSPWR=0\r'.encode())	# GNSS ausschalten
			SIM7000E.answer()
		except:
			print('SIM7000E_deactivate_gnss_fail')
			SIM7000E.init()
			return ['fail']
	def read_scor():
		try:
			SIM7000E.sim.write('AT+CGNSINF\r'.encode())
			dat = SIM7000E.answer()[0]
			#print(dat)
			#for i in range(0, len(dat)):
			if dat[0:9] == "+CGNSINF:":
				lat = dat.split(',')[3]
				lon = dat.split(',')[4]
				try:
					lat = float(lat)
					lon = float(lon)
					return [lat, lon]
				except:
					print('SIM7000E_gnss_format_fail')
					return ['fail', 'fail']
			else:
				print('SIM7000E_gnss_format_fail')
				return ['fail', 'fail']
		except:
			print('SIM7000E_read_scor_fail')
			SIM7000E.init()
			return ['fail', 'fail']
	def save_scor(dat):
		try:
			write_file(filename("scor_raw", "csv", 5), "raw", dat)#, 2)
		except:
			print('SIM7000E_save_scor_fail')
	def create_payload(dat):
		try:
			########## fehlt; auch koordinaten aus sim7000e usw sollen in dieses payload gepackt werden
			return dat
		except:
			print('SIM7000E_create_payload_fail')
	def save_payload(dat):
		try:
			write_file("payl_sim.csv", "raw", dat)#, 18)
		except:
			print('SIM7000E_save_payload_fail')
	def send_file(pat):
		try:
			fil = open(pat, 'w')
			########################## fehlt ##################
			if SIM7000E.http_post(fil) == 0:
				fil.truncate()
			else:
				print('SIM7000E_no_reception')
			fil.close()
		except:
			print('SIM7000E_send_fail')
			SIM7000E.init()
			return ['fail']
	def http_post(fil):
		try:
			#
			#
			# fehlt ###########
			#
			#
			return 0
		except:
			print('SIM7000E_http_fail')
			SIM7000E.init()
			return 1
			'''




class LoRa:
	lor = None
	def init(prt = 'ttyUSB0'):
		try:
			try:
				LoRa.lor.close()
				print('LoRa_handle_closed')
			except:
				None
			# open("/home/pi/strato2/raw/" + filename('payl_raw', 'csv', 5), 'a').close()
			LoRa.lor = serial.Serial('/dev/' + prt, 115200)
			#print(LoRa.answer())
			time.sleep(.2)
			LoRa.lor.reset_input_buffer()
			print(LoRa.answer(5))
			#### fehlt ####################
			print('LoRa_init_done')
		except:
			try:
				print('LoRa_init_fail')
				global transferfail
				transferfail[1] = 1
				LoRa.lor.close()
			except:
				print('LoRa_close_fail')
	def answer(tmo = 10):
		try:
			buf = bytearray()
			chk = bytearray()
			now = time.time()
			while time.time() < now + tmo:
				while(LoRa.lor.in_waiting > 0):
					incomingByte = LoRa.lor.read()
					buf += incomingByte
				# print(buf)
				if(len(buf) < 4):
					continue
				for i in range(len(buf)):
					if(buf[i] == 0xf9 and len(buf) >= i + 4): # header, size, data block, chkA, chkB => length >= 5
						asw = bytearray()
						payload_size = int(buf[i+1])
						if(len(buf) >= i + 4 + payload_size):
							for j in range(i + 2, i + 2 + payload_size):
								asw.append(buf[j])
							chk = LoRa.checksum(asw)
							if(chk[0] != buf[i + 2 + payload_size] or chk[1] != (buf[i + 3 + payload_size])):
								continue
							if(len(buf) >= i + 4 + payload_size):
								buf = buf[i + 4 + payload_size:]
							else:
								buf = bytearray()
						return asw
			return bytearray()
		except:
			print('LoRa_answer_fail')
			LoRa.init()
			return bytearray()
	def average_payload():
		try:
			# fil = open("/home/pi/strato2/raw/" + filename('payl_raw', 'csv', 5), 'r')	#aktuelle datei
			# txt = fil.readlines()	#aktueller inhalt
			# fil.close()
			# txtlen = len(txt)
			# print(txt)
			# dat = []
			# for i in range(txtlen):
			#	dat = dat + [txt[i].split(';')]
			
			#[datu, uhrz, mcor[0], mcor[1], mhei[0], volt[0], gyro[0], gyro[1], gyro[2], acce[0], acce[1], acce[2], ozon[0], pres[0], temp[0], humi[0], magn[0], magn[1], magn[2], uvse[0], uvse[1], mand[0], mxor[0]]
			
			avr = bytearray()	# avr = average
			# -------- 0. upLinkSequnceNo -------
			seq_no = LoRa.uplinkSequenceNo()						# uplinkSequenceNo hinzufügen
			for b in seq_no:
				avr.append(b)
			
			# -------- 1. Time Stamp -----------
			tme = datetime.utcnow().strftime('%H:%M:%S')
			tmestr = tme.split(':')
			sec = int(tmestr[0]) * 3600 + int(tmestr[1]) * 60 + int(float(tmestr[2]))
			avr.append(0x01) # channel 1
			avr.append(0x02) # data type 2 (analog input)
			avr.append(sec % (2**16) // (2**8))
			avr.append(sec % (2**8))

			# --------- 2. GPS Position ----------
			'''if(dat[txtlen - 1][2].strip() == 'fail'):				# latitude hinzufügen
				lat = 0
			else:
				lat = int(float(dat[txtlen - 1][2].strip()) * 10000)
			avr.append(lat // (2**16))
			avr.append((lat % (2**16)) // (2**8))
			avr.append(lat % (2**8))
			if(dat[txtlen - 1][3].strip() == 'fail'):				# longitude hinzufügen
				lon = 0
			else:
				lon = int(float(dat[txtlen - 1][3].strip()) * 10000)
			avr.append(0x02) # channel 2
			avr.append(0x88) # gps position
			avr.append(lon // (2**16))
			avr.append((lon % (2**16)) // (2**8))
			avr.append(lon % (2**8))
			if(dat[txtlen - 1][4].strip() == 'fail'):				# höhe hinzufügen
				hei = 0
			else:
				hei = int(float(dat[txtlen - 1][4].strip()) * 100) # in cm angegeben
				if(hei < 0):
					hei = 0'''
			try:
				mcor = MuonPi.read_mcor()
			except:
				print('send_mcor_fail')
				mcor = [0, 0]
			mlat = int(float(mcor[0]) * 10000)
			mlon = int(float(mcor[1]) * 10000)
			try:
				mhei = int(float(MuonPi.read_mhei()[0]) * 100)
			except:
				print('send_mhei_fail')
				mhei = 0
			avr.append(0x02) # channel 2
			avr.append(0x88) # gps position
			avr.append((mlat % (2**24)) // (2**16))
			avr.append((mlat % (2**16)) // (2**8))
			avr.append(mlat % (2**8))
			avr.append((mlon % (2**24)) // (2**16))
			avr.append((mlon % (2**16)) // (2**8))
			avr.append(mlon % (2**8))
			avr.append((mhei % (2**24)) // (2**16))
			avr.append((mhei % (2**16)) // (2**8))
			avr.append(mhei % (2**8))
			
			# -------- 3. Akkuspannung ----------
			'''avr.append(0x03) # channel 3
			avr.append(0x02) # analog output
			if(dat[txtlen - 1][5].strip() == 'fail'):				# akkuspannung hinzufügen
				vol = 0
			else:
				vol = int(float(dat[txtlen - 1][5].strip()) / 32768 * 6.144 * 100) # in centivolt angegeben'''
			

			# vol = int(float(dat[txtlen - 1][5].strip()) / 32768 * 6.144 * 100) # in centivolt angegeben
			try:
				volt = int(average(read_file(filename('volt_raw', 'csv', 5), 'raw', 2, 5)) / 32768 * 6.144 * 100) # in centivolt angegeben
			except:
				print('send_volt_fail')
				volt = 0
			avr.append(0x03) # channel 3
			avr.append(0x02) # analog output
			avr.append((volt % (2**16)) // (2**8))
			avr.append(volt % (2**8))

			# ------- 4. XOR rate ------------
			try:
				mxor = int(float(MuonPi.read_mxor()[0]) * 1000)
			except:
				print('send_mxor_fail')
				mxor = 0
			avr.append(0x04) # channel 4
			avr.append(0x02) # analog input
			avr.append((mxor % (2**16)) // (2**8))
			avr.append(mxor % (2**8))

			# ------- 5. AND rate ------------
			try:
				mand = int(float(MuonPi.read_mand()[0]) * 1000)
			except:
				print('send_mand_fail')
				mand = 0
			avr.append(0x05) # channel 5
			avr.append(0x02) # analog input
			avr.append((mand % (2**16)) // (2**8))
			avr.append(mand % (2**8))

			# ------- 6. Counter rate ------------
			try:
				coun = int(float(MuonPi.read_coun()[0]) * 1000) # ist *1000 sinnvoll?
			except:
				print('send_coun_fail')
				coun = 0
			avr.append(0x06) # channel 6
			avr.append(0x02) # analog input
			avr.append((coun % (2**16)) // (2**8))
			avr.append(coun % (2**8)) # counter rate existiert noch nicht

			# ---------- 7. Pressure --------------
			try:
				pres = int(average(read_file(filename('pres_raw', 'csv', 5), 'raw', 2, 5)) * 100) # in Pa
			except:
				print('send_pres_fail')
				pres = 0
			avr.append(0x07) # channel 7
			avr.append(0x02) # analog input
			avr.append((pres % (2**16)) // (2**8))
			avr.append(pres % (2**8))

			# ---------- 8. Temperature Out ----------
			try:
				temo = int(average(read_file(filename('temp_raw', 'csv', 5), 'raw', 2, 5)) * 100) # in K
			except:
				print('send_temo_fail')
				temo = 0
			avr.append(0x08) # channel 8
			avr.append(0x02) # analog input
			avr.append((temo % (2**16)) // (2**8))
			avr.append(temo % (2**8))

			# --------- 9. Temperature In ------------
			'''try:
				temi = int(float(MuonPi.read_temi()) * 1000) # ist *1000 sinnvoll?
			avr.append(0x09) # channel 9
			avr.append(0x02) # analog input
			avr.append((temi % (2**16)) // (2**8))
			avr.append(temi % (2**8))'''
			
			print(avr)

			'''
			avr.append(past_seconds(dat[txtlen - 1][1].strip()))	# letzter zeitstempel aus payload file wird verwendet in sekunden
			#print(avr)
			if(dat[txtlen - 1][2].strip() == 'fail'):				# letzte latitude hinzufügen
				lat = 0
			else:
				lat = int(float(dat[txtlen - 1][2].strip()) * 100000)
			avr = avr + [lat]
			#print(avr)
			if(dat[txtlen - 1][3].strip() == 'fail'):				# letzte longitude hinzufügen
				lon = 0
			else:
				lon = int(float(dat[txtlen - 1][3].strip()) * 100000)
			avr = avr + [lon]
			#print(avr)
			if(dat[txtlen - 1][4].strip() == 'fail'):				# letzte höhe hinzufügen
				hei = 0
			else:
				hei = int(float(dat[txtlen - 1][4].strip()))
			avr = avr + [hei]
			#print(avr)
			if(dat[txtlen - 1][5].strip() == 'fail'):				# letzte akkuspannung hinzufügen
				vol = 0
			else:
				vol = int(dat[txtlen - 1][5].strip())
			avr = avr + [vol]
			#print(avr)
			#if(dat[txtlen - 1][6].strip() == 'fail'):				# test falls irgendeinein anderer wert = 'fail' ist (hier gyro[0])
			#	tst = 0
			#else:
			#	tst = int(dat[txtlen - 1][6].strip())
			#avr = avr + [tst]
			######################################## als nächstes müssen mittelwerte und beträge berechnet und hinzugefügt werden
			#print(avr)
			'''
			return avr
		except:
			print('LoRa_average_payload_fail')
			return bytearray()
	'''def average_payload():
		try:
	def read_payload():
		try:
			fil1 = open("/home/pi/strato2/raw/payl_raw.csv",'r')
			fil2 = open("/home/pi/strato2/raw/lora_cnt.csv",'r')	# line index of last taken data from payload file is saved in lora_cnt.csv
			cnt = int(fil2.read().strip())
			print(cnt)
			dat = fil1.readlines()
			print(dat)
			dat = dat[cnt:]
			print(dat)
			fil1.close()
			fil2.close()
		except:
			print('LoRa_read_payload_fail')'''
	'''def create_payload(dat):
		try:
			########## fehlt; was soll noch in dieses payload gepackt werden? nur das wichtigste, zb keine akkuspannung
			return dat
		except:
			print('LoRa_create_payload_fail')'''
	def save_payload(dat):
		try:
			write_file("payl_lor.csv", "raw", dat)#, 18)
		except:
			print('LoRa_save_payload_fail')
	def checksum(pld):
		sum1 = 0
		sum2 = 0
		for b in pld:
			sum1 += b
			sum1 = sum1 % 256
			sum2 += sum1
			sum2 = sum2 % 256
		rlt = bytearray()
		rlt.append(sum1)
		rlt.append(sum2)
		return rlt
	def uplinkSequenceNo():
		try:
			fil = open("/home/pi/strato2/raw/uplinkSequenceNo.txt", 'r')
			usn = int(fil.readlines()[0])						# usn = uplinkSequenceNo
			fil.close()
			usn += 1
			#print(usn)
			fil = open("/home/pi/strato2/raw/uplinkSequenceNo.txt", 'w')
			fil.write(str(usn))
			fil.close()
			seq_no = bytearray()
			seq_no.append(usn // (2**24))
			seq_no.append((usn % (2**24)) // (2**16))
			seq_no.append((usn % (2**16)) // (2**8))
			seq_no.append(usn % (2**8))
			return seq_no
		except:
			print('LoRa_uplinkSequenceNo_fail')
			try:
				fil.close()
				print('LoRa_uplinkSequenceNo_closed')
			except:
				None
			try:
				os.remove("/home/pi/strato2/raw/uplinkSequenceNo.txt")
			except:
				None
			fil = open("/home/pi/strato2/raw/uplinkSequenceNo.txt", 'w')
			usn = int(random.random() * 10000 + 1)
			fil.write(str(usn))
			fil.close()
			seq_no = bytearray()
			seq_no.append(usn // (2**24))
			seq_no.append((usn % (2**24)) // (2**16))
			seq_no.append((usn % (2**16)) // (2**8))
			seq_no.append(usn % (2**8))
			return seq_no
	def create_message(pld): # message die an lora gesendet werden soll
		try:
			if(len(pld) > 255):
				raise ValueError('payload larger than 255 bytes')
			msg = bytearray()
			msg.append(0xf9) # append msg header byte
			msg.append(len(pld))
			for b in pld:
				msg.append(b)
			rlt = LoRa.checksum(pld)
			for b in rlt:
				msg.append(b)
			return msg
		except:
			print('LoRa_create_message_fail')
			return bytearray(b'\xf9\x00\x00\x00')
	def save_message(dat):
		try:
			write_file(filename("lora_raw", "csv", 5), "raw", dat)#, 18)
		except:
			print('LoRa_message_save_fail')
	'''def create_average():
		None
		############## fehlt ##################'''
	def send_message(msg):
		try:
			# test = open('test.txt', 'a')
			# test.writelines(msg)
			#txt = []
			#for i in range(len(msg)):
				#LoRa.lor.write(msg[i])
				#test.write(str(hex(msg[i])))
				#print(hex(msg[i]))#,'utf-8'))
				#		txt[i] += str(int(msg[i]))
			# test.close()
			#test = str(int(msg))
			#print(test.encode())
			#		LoRa.lor.write(txt.encode())
			LoRa.lor.write(bytes(msg))
			#return LoRa.answer()
		except:
			print('LoRa_send_fail')
			LoRa.init()
			# return ['fail']




def init_sensors():
	global sensorfail
	sensorfail = [0, 0]
	ADS_1115.init()
	MPU6050.init()
	SEN0321.init()
	BME280.init()
	QMC5883L.init()
	VEML6075.init()
	print('init_sensors_done')




def init_transfers():
	global transferfail
	transferfail = [0, 0]
	# SIM7000E.init()
	LoRa.init()
	print('init_transfers_done')




def failsafe_sensors():
	global sensorfail
	#print(sensorfail)
	if sensorfail[0] == 1 and sensorfail[1] == 0:
		print('sensors_restarting ...')
		init_sensors()
		print('sensors_restarted')
	else:
		sensorfail[0] = sensorfail[1]
		sensorfail[1] = 0




def failsafe_transfers():
	global transferfail
	#print(transferfail)
	if transferfail[0] == 1 and transferfail[1] == 0:
		print('transfers_restarting ...')
		init_transfers()
		print('transfers_restarted')
	else:
		transferfail[0] = transferfail[1]
		transferfail[1] = 0




#------------------ ADC Commands ------------------------

def adc_read(adc, num, snt):					#snt=sensitivity
	return adc.read_adc(num, snt, 128)





#----------------- File Commands ----------------------

def write_file(nam, dir, val):#, num = 1):		# name, value, number
	try:
		pat	= "/home/pi/strato2/" + dir + "/" + nam	# path
		fil = open(pat, 'a')
		fil.write(datetime.utcnow().strftime('     %Y:%m:%d;   %H:%M:%S.%f')[:-3])
		i = 0
		while (i < len(val)):
			fil.write(";%15s" % (val[i]))
			i += 1
		fil.write(";")
		fil.write("\n")
		fil.close()
	except:
		print('write_file_fail')



def read_file(nam, dir, num = 2, cnt = 1):#, num = 1):		# name, value, number(arraystelle, die ausgelesen weden soll), count(anzahl, die zurück gegeben werden soll)
	pat	= "/home/pi/strato2/" + dir + "/" + nam	# path
	fil = open(pat, 'r')
	txt = fil.readlines()
	fil.close()
	val = []
	for i in range(cnt):
		val += [txt[len(txt) - 1 - i].split(';')[num].strip()]
	return val



def average(dat): # dat muss array sein
	res = 0
	n = len(dat)
	for i in range(n):
		try:
			res += float(dat[i])
		except:
			n -= 1
	if(n == 0):
		return 0
	res = res / n
	return res



def filename(nam, typ, tim, dif = 0):				# dateiname, dateiendung, time nachdem neue datei erstellt werden soll, differenz um die vor gestellt wird
    min = int(datetime.utcnow().strftime('%M')) + int(datetime.utcnow().strftime('%H')) * 60	# minute
    min = min + dif
    while (min % tim != 0):
        min -= 1
    hou = int(min / 60)
    if(int(hou / 10) == 0):
        houstr = '0' + str(hou)				# stunde als string
    else:
        houstr = str(hou)
    min = int(min % 60)
    if(int(min / 10) == 0):
        minstr = '0' + str(min)
    else:
        minstr = str(min)
    now = datetime.utcnow().strftime('%Y%m%d_') + houstr + minstr	# zeit für dateinamen
    namnow = nam + '_'+ now + '.' + typ
    return namnow



def muon_logfile(pat = '/var/muondetector/currentWorkingFileInformation.conf'): # gibt Pfad der aktuellen Log-File aus
	fil = open(pat, 'r')
	ret = fil.readlines()[1][:-1]
	fil.close()
	return ret



def read_logfile(pat, atr):			# dateipfad, attribut welches ausgegeben werden soll
	fil = open(pat, 'r')			# liest einen Wert eines Attributs einer Log-File aus
	txt = fil.readlines()
	fil.close()
	for i in range(-1, -50, -1):
		if txt[i][20:20 + len(atr)] == atr:
			return txt[i].split(' ')[2].strip()
	return 'fail'







#----------------- Sending Commands ----------------------



def transmit(hdl, trm, dev):           # handle, zu sendender String, Geraet: s/l sim/lora
	trm = dev + ';' + trm + '#'
	trm = bytes(trm, 'utf-8') # transmitting: zu sendende Zeichen
	#print("\n" + str(hdl.write(trm))+" Zeichen wurden gesendet")
	hdl.write(trm)



def receive(hdl):     # hdl=handle
	while hdl.in_waiting:
		rcv = hdl.read_until(b'#').decode('utf-8') # receiving: empfangene Zeichen
		rcv = rcv[:-1]
		print(rcv)




#----------------- GPS Commands -------------------------



def read_gps(hdl, tmo):        # hdl=handle, tmo=timeout
	hdl.reset_input_buffer()
	now = time.time()
	while time.time() < now + tmo:
		if hdl.in_waiting:
			dat = hdl.readline().decode('utf-8')
			#print(dat)
			if dat[0:6] == "$GPRMC":
				#print('GPRMC detected')
				cor = pynmea2.parse(dat) # koordinaten
				lat = round(cor.latitude, 5)
				lon = round(cor.longitude, 5)
				#out = str(lat) + ',' + str(lon)
				return [lat, lon]
		else:
			time.sleep(.05)



def maps_str(cor):      # cor=koordinaten mit , getrennt
	out = 'https://www.google.de/maps?q=' + str(cor[0]) + ',' + str(cor[1])
	return out





#-------------------- Time Commands --------------------



def set_starttime():		# Startzeit setzen
	global starttime
	starttime = time.time()



def sleep_until(tme):		# Pause bis Programmlaufzeit bestimmte Zeit ist
	global starttime
	time.sleep(tme - ((time.time() - starttime) % tme))
	return 1



def past_seconds(tme):		# vergangene Sekunden des Tages # tme = übergebene Zeit in Form hh:mm:ss
	tme = tme.split(':')
	#print(int(tme[0]) * 3600 + int(tme[1]) * 60 + int(float(tme[2])))
	return int(tme[0]) * 3600 + int(tme[1]) * 60 + int(float(tme[2]))
