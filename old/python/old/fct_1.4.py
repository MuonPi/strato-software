
import time
from datetime import datetime
import smbus
import serial
import pynmea2
#from ADS1x15 import ADS1115


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
		write_byte(0x68, 0x6b, 0b00000000)	# countinues-mode
		write_byte(0x68, 0x1b, 0b00001000)	# oversampling gyro
		write_byte(0x68, 0x1c, 0b00000000)	# oversampling acce
	def read_gyro():
		return [read_2byte_hl(0x68, 0x3d), read_2byte_hl(0x68, 0x3b), read_2byte_hl(0x68, 0x3f)]
	def read_acce():
		return [read_2byte_hl(0x68, 0x43), read_2byte_hl(0x68, 0x45), read_2byte_hl(0x68, 0x47)]
	def save_gyro():
		write_file(filename("gyro_raw", "csv", 5), "raw", MPU6050.read_gyro(), 3)
	def save_acce():
		write_file(filename("acce_raw", "csv", 5), "raw", MPU6050.read_acce(), 3)



#------------------ ADC Commands ------------------------

#def adc_read(adc, num, snt):					#snt=sensitivity
#	return adc.read_adc(num, snt, 128)





#----------------- File Commands ----------------------

def write_file(nam, dir, val, num = 1):		# name, value, number
	pat	= "/home/pi/strato2/"+ dir + "/" + nam	# path
	fil = open(pat,'a')
	fil.write("\n")
	fil.write(datetime.utcnow().strftime('     %Y:%m:%d;   %H:%M:%S.%f')[:-3])
	if (num == 1):
		fil.write(";%15s" % (val))
	else:
		i = 0
		while (i < num):
			fil.write(";%15s" % (val[i]))
			i += 1
	fil.write(";")
	fil.close()
	return 1



def filename(nam, typ, tim):				# dateiname, dateiendung, time nachdem neue datei erstellt werden soll
    min = int(datetime.utcnow().strftime('%M')) + int(datetime.utcnow().strftime('%H')) * 60	# minute
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
	ret = "fail"
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
				
