
import time
from datetime import datetime
import smbus
import serial
import pynmea2
import random
import os
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
	def init():
		try:
			write_byte(0x76, 0xF2, 0b00000010)  # oversampling humidity)
			write_byte(0x76, 0xF4, 0b01001011)  # oversampling temperature, oversampling pressure, mode
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
	'''def send_file(pat):
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
			return 1'''




class LoRa:
	lor = None
	def init(prt = 'nanoatmega328'):
		try:
			try:
				LoRa.lor.close()
				print('LoRa_handle_closed')
			except:
				None
			open("/home/pi/strato2/raw/" + filename('payl_raw', 'csv', 5, -5), 'a').close()
			LoRa.lor = serial.Serial('/dev/' + prt, 115200)
			time.sleep(.1)
			LoRa.lor.reset_input_buffer()
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
			asw = bytearray()
			now = time.time()
			while time.time() < now + tmo:
				while LoRa.lor.in_waiting > 0:
					tmp = LoRa.lor.read(100)
					print(tmp)
					# LoRa.lor.readinto(asw)
					# print(asw)
					time.sleep(.2)
					if LoRa.lor.in_waiting == 0:
						return asw
			print('LoRa_answer_timeout')
			return ['fail']
		except:
			print('LoRa_answer_fail')
			LoRa.init()
			return ['fail']
	def average_payload():
		try:
			fil = open("/home/pi/strato2/raw/" + filename('payl_raw', 'csv', 5, -5), 'r')
			txt = fil.readlines()
			#txt = ['datum1;10:10:10;50.514642778351906;zahl12','datum2;10:10:10;50.514642778351906;8.577668325826654;123;757865'] ############################# muss gelöscht werden
			fil.close()
			txtlen = len(txt)
			#print(txt)
			i = 0
			dat = []
			while(i < txtlen):
				dat = dat + [txt[i].split(';')]
				i += 1
			#print(dat)		# als erste stelle eventuell den idcounter für lora senden
			#[datu, uhrz, mcor[0], mcor[1], mhei[0], volt[0], gyro[0], gyro[1], gyro[2], acce[0], acce[1], acce[2], ozon[0], pres[0], temp[0], humi[0], magn[0], magn[1], magn[2], uvse[0], uvse[1], mand[0], mxor[0]]
			avr = []	# avr = average
			avr = avr + [LoRa.uplinkSequenceNo()]
			#print(avr)
			avr = avr + [past_seconds(dat[txtlen - 1][1].strip())]	# letzter zeitstempel aus payload file wird verwendet in sekunden
			#print(avr)
			if(dat[txtlen - 1][2].strip() == 'fail'):				# letzte latitude hinzufügen			###### vielleicht hilft .strip() damit es geht
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
			return avr
		except:
			print('LoRa_average_payload_fail')
			return[0]
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
		i = 0
		while i < len(pld):
			sum1 = (sum1 + pld[i]) % 255
			sum2 = (sum2 + sum1) % 255
			i += 1
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
			return usn
		except:
			print('LoRa_uplinkSequenceNo_fail')
			try:
				fil.close()
				print('LoRa_uplinkSequenceNo_closed')
			except:
				None
			os.remove("/home/pi/strato2/raw/uplinkSequenceNo.txt")
			fil = open("/home/pi/strato2/raw/uplinkSequenceNo.txt", 'w')
			usn = int(random.random() * 10000 + 1)
			#print(usn)
			fil.write(str(usn))
			fil.close()
			return usn
	def create_message(pld): # message die an lora gesendet werden soll
		try:
			if (len(pld)>255):
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
	def test():
		try:
			LoRa.lor.write('test'.encode())
			return LoRa.answer()
		except:
			print('LoRa_test_fail')
			LoRa.init()
			return ['fail']
	def send_message(msg):
		try:
			LoRa.lor.write(msg)
			return LoRa.answer()
		except:
			print('LoRa_send_fail')
			LoRa.init()
			return ['fail']




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
	SIM7000E.init()
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
	pat	= "/home/pi/strato2/"+ dir + "/" + nam	# path
	fil = open(pat,'a')
	#fil.write("\n")
	fil.write(datetime.utcnow().strftime('     %Y:%m:%d;   %H:%M:%S.%f')[:-3])
	#if (num == 1):
	#	fil.write(";%15s" % (val))
	#else:
	i = 0
	while (i < len(val)):#num):
		fil.write(";%15s" % (val[i]))
		i += 1
	fil.write(";")
	fil.write("\n")
	fil.close()
	return 1



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
	for i in range(-1, -50, -1):
		#print(txt[i])
		#time.sleep(0.5)
		#print(txt[i][20:27])
		if txt[i][20:20+len(atr)] == atr:
			#print("detected")
			fil.close()
			ret = txt[i][21+len(atr):-1]
			return ret
	fil.close()
	ret = 'fail'
	return ret









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
