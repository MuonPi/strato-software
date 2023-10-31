
import time
from datetime import datetime
import smbus
from ADS1x15 import ADS1115


#---------------- i2c Befehle --------------------

def read_byte(bus, adr, reg):					#bus=i2c bus nummer; adr=adresse sensor; reg=register sensor
	return bus.read_byte_data(adr, reg)			#bus=aus smbus SMBus(1)
    
def read_byte_2c(bus, adr, reg):				#als 2er-komplement auslesen
	tmp = read_byte(bus, adr, reg)
	if (tmp >= 0x80):
		return -((255 - tmp) + 1)
	else:
		return tmp
 
def read_word(bus, adr, reg):					#als 1er-komplement auslesen
	h = bus.read_byte_data(adr, reg)
	l = bus.read_byte_data(adr, reg+1)
	tmp = (h << 8) + l
	return tmp
 
def read_word_2c(bus, adr, reg):				#als 2er-komplement auslesen
	tmp = read_word(bus, adr, reg)
	if (tmp >= 0x8000):
		return -((65535 - tmp) + 1)
	else:
		return tmp

def read_3byte(bus, adr, reg):
	h = bus.read_byte_data(adr, reg)
	l = bus.read_byte_data(adr, reg+1)
	x = bus.read_byte_data(adr, reg+2)
	tmp = (h << 16) + (l << 8) + x
	return tmp

def read_3byte_2c(bus, adr, reg):
	tmp = read_3byte(bus, adr, reg)
	if (tmp >= 0x800000):
		return -((16777215 - tmp) + 1)
	else:
		return tmp
    
def write_byte(bus, adr, reg, cmd):            #cmd=command
	return bus.write_byte_data(adr, reg, cmd)







#------------------ adc Befehle ------------------------

def adc_read(adc, num, snt):					#snt=sensitivity
	return adc.read_adc(num, snt, 128)





#----------------- Datei Befehle ----------------------

def write_file(nam, dir, val, num = 1):		# name, value, number
	pat	= "/home/pi/strato/"+ dir + "/" + nam	# path
	datei = open(pat,'a')
	datei.write("\n")
	datei.write(datetime.utcnow().strftime('     %Y:%m:%d;   %H:%M:%S.%f')[:-3])
	if (num == 1):
		datei.write(";%15d" % (val))
	else:
		i = 0
		while (i < num):
			datei.write(";%15d" % (val[i]))
			i += 1
	datei.write(";")
	datei.close()
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