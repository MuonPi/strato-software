import time
from enum import Enum
from datetime import datetime, timezone
import structlog
import smbus2 as smbus

# import serial
import random
import os
import pynmea2
from ADS1x15 import ADS1115

logger = structlog.get_logger()

data_directory = os.path.dirname(__file__) + "/../"


class subdirectory(Enum):
    RAW = 1


subdirectories = {subdirectory.RAW: "raw"}
for key, value in subdirectories.items():
    os.makedirs(
        os.path.join(data_directory, value), exist_ok=True
    )  # Create directory if not exists

# ---------------- I2C Commands --------------------


def read_byte(
    adr, reg, bus=smbus.SMBus(1)
):  # bus=i2c bus nummer; adr=adresse sensor; reg=register sensor
    return bus.read_byte_data(adr, reg)  # bus=aus smbus SMBus(1)


def read_byte_2c(adr, reg, bus=smbus.SMBus(1)):  # als 2er-komplement auslesen
    tmp = read_byte(adr, reg)
    if tmp >= 0x80:
        return -((255 - tmp) + 1)
    else:
        return tmp


def read_2byte_hl(
    adr, reg, bus=smbus.SMBus(1)
):  # als 1er-komplement auslesen zuerst highbyte dann lowbyte
    h = bus.read_byte_data(adr, reg)
    l = bus.read_byte_data(adr, reg + 1)
    tmp = (h << 8) + l
    return tmp


def read_2byte_hl_2c(adr, reg, bus=smbus.SMBus(1)):  # als 2er-komplement auslesen
    tmp = read_2byte_hl(adr, reg)
    if tmp >= 0x8000:
        return -((65535 - tmp) + 1)
    else:
        return tmp


def read_2byte_lh(
    adr, reg, bus=smbus.SMBus(1)
):  # als 1er-komplement auslesen zuerst highbyte dann lowbyte
    l = bus.read_byte_data(adr, reg)
    h = bus.read_byte_data(adr, reg + 1)
    tmp = (h << 8) + l
    return tmp


def read_2byte_lh_2c(adr, reg, bus=smbus.SMBus(1)):  # als 2er-komplement auslesen
    tmp = read_2byte_lh(adr, reg)
    if tmp >= 0x8000:
        return -((65535 - tmp) + 1)
    else:
        return tmp


def read_3byte_hlx(adr, reg, bus=smbus.SMBus(1)):
    dat = bus.read_i2c_block_data(adr, reg, 3)
    # h = bus.read_byte_data(adr, reg)
    # l = bus.read_byte_data(adr, reg+1)
    # x = bus.read_byte_data(adr, reg+2)
    # tmp = (h << 12) + (l << 4) + (x >> 4)
    tmp = (dat[0] << 12) | (dat[1] << 4) | (dat[2] >> 4)
    return tmp


def read_3byte_hlx_2c(adr, reg, bus=smbus.SMBus(1)):
    tmp = read_3byte_hlx(adr, reg)
    if tmp >= 0x800000:
        return -((16777215 - tmp) + 1)
    else:
        return tmp


def read_word(adr, reg, bus=smbus.SMBus(1)):
    return bus.read_word_data(adr, reg)


def write_byte(adr, reg, cmd, bus=smbus.SMBus(1)):  # cmd=command
    return bus.write_byte_data(adr, reg, cmd)


# ------------------ Sensors ----------------------------


class MPU6050:
    def init():
        try:
            write_byte(0x68, 0x6B, 0b00000000)  # countinues-mode
            write_byte(0x68, 0x1B, 0b00001000)  # oversampling gyro
            write_byte(0x68, 0x1C, 0b00000000)  # oversampling acce
            logger.info("MPU6050_init_done")
        except Exception as e:
            logger.error("MPU6050_init_fail " + repr(e))
            global sensorfail
            sensorfail[1] = 1

    def read_gyro():
        try:
            return [
                read_2byte_hl(0x68, 0x3D),
                read_2byte_hl(0x68, 0x3B),
                read_2byte_hl(0x68, 0x3F),
            ]
        except Exception as e:
            logger.error("MPU6050_gyro_read_fail " + repr(e))
            MPU6050.init()
            return ["fail", "fail", "fail"]

    def read_acce():
        try:
            return [
                read_2byte_hl(0x68, 0x43),
                read_2byte_hl(0x68, 0x45),
                read_2byte_hl(0x68, 0x47),
            ]
        except Exception as e:
            logger.error("MPU6050_acce_read_fail " + repr(e))
            MPU6050.init()
            return ["fail", "fail", "fail"]

    def save_gyro(dat):
        try:
            write_file(filename("gyro_raw", "csv", 5), subdirectory.RAW, dat)  # , 3)
        except Exception as e:
            logger.error("MPU6050_gyro_save_fail " + repr(e))

    def save_acce(dat):
        try:
            write_file(filename("acce_raw", "csv", 5), subdirectory.RAW, dat)  # , 3)
        except Exception as e:
            logger.error("MPU6050_acce_save_fail " + repr(e))


class SEN0321:
    def init():
        try:
            write_byte(0x73, 0x03, 0x00)  # automatic mode
            logger.info("SEN0321_init_done")
        except Exception as e:
            logger.error("SEN0321_init_fail " + repr(e))
            global sensorfail
            sensorfail[1] = 1

    def read_ozon():
        try:
            return [read_2byte_hl(0x73, 0x09)]
        except Exception as e:
            logger.error("SEN0321_ozon_read_fail " + repr(e))
            SEN0321.init()
            return ["fail"]  #'fail'

    def save_ozon(dat):
        try:
            write_file(filename("ozon_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("SEN0321_ozon_save_fail " + repr(e))


class BME280:
    def init():
        try:
            write_byte(0x76, 0x0E, 0xB6)  # reset
            write_byte(0x76, 0xF2, 0b00000101)  # oversampling humidity)
            write_byte(
                0x76, 0xF4, 0b10110111
            )  # oversampling temperature, oversampling pressure, mode
            logger.info("BME280 ID: " + hex(read_byte(0x76, 0xD0)))  # log chip id
            logger.info(
                "88-a1: " + repr(smbus.SMBus(1).read_i2c_block_data(0x76, 0x88, 26))
            )
            logger.info(
                "e1-f0: " + repr(smbus.SMBus(1).read_i2c_block_data(0x76, 0xE1, 7))
            )
            logger.info("BME280_init_done")
        except Exception as e:
            logger.error("BME280_init_fail " + repr(e))
            global sensorfail
            sensorfail[1] = 1

    def read_pres():
        try:
            return [read_3byte_hlx(0x76, 0xF7)]
        except Exception as e:
            logger.error("BME280_pres_read_fail " + repr(e))
            BME280.init()
            return ["fail"]  #'fail'

    def read_temp():
        try:
            return [read_3byte_hlx(0x76, 0xFA)]
        except Exception as e:
            logger.error("BME280_temp_read_fail " + repr(e))
            BME280.init()
            return ["fail"]  #'fail'

    def read_humi():
        try:
            return [read_2byte_hl(0x76, 0xFD)]
        except Exception as e:
            logger.error("BME280_humi_read_fail " + repr(e))
            BME280.init()
            return ["fail"]  #'fail'

    def save_pres(dat):
        try:
            write_file(filename("pres_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("BME280_pres_save_fail " + repr(e))

    def save_temp(dat):
        try:
            write_file(filename("temp_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("BME280_temp_save_fail " + repr(e))

    def save_humi(dat):
        try:
            write_file(filename("humi_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("BME280_humi_save_fail " + repr(e))


class QMC5883L:
    def init():
        try:
            write_byte(
                0x0D, 0x09, 0b00001101
            )  # samplerate=512Hz; fieldrange=2Ga; datarate=200Hz; continues-mode
            write_byte(
                0x0D, 0x0A, 0b00000001
            )  # disable soft-reset; disable roll-over; disable interrupt
            write_byte(0x0D, 0x0B, 0b00000001)  # set period
            logger.info("QMC5883L_init_done")
        except Exception as e:
            logger.error("QMC5883L_init_fail " + repr(e))
            global sensorfail
            sensorfail[1] = 1

    def read_magn():
        try:
            return [
                read_2byte_lh(0x0D, 0x00),
                read_2byte_lh(0x0D, 0x02),
                read_2byte_lh(0x0D, 0x04),
            ]
        except Exception as e:
            logger.error("QMC5883L_magn_read_fail " + repr(e))
            QMC5883L.init()
            return ["fail", "fail", "fail"]

    def save_magn(dat):
        try:
            write_file(filename("magn_raw", "csv", 5), subdirectory.RAW, dat)  # , 3)
        except Exception as e:
            logger.error("QMC5883L_magn_save_fail " + repr(e))

    def read_tmpi():
        try:
            return [read_2byte_lh(0x0D, 0x07)]
        except Exception as e:
            logger.error("QMC5883L_tmpi_read_fail " + repr(e))
            QMC5883L.init()
            return ["fail"]

    def save_tmpi(dat):
        try:
            write_file(filename("tmpi_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("QMC5883L_tmpi_save_fail " + repr(e))


class VEML6075:
    def init():
        try:
            write_byte(
                0x10, 0x00, 0b00011000
            )  # integrationtime=100ms; high dynamic; no trigger; no force-mode; continues-mode
            logger.info("VEML6075_init_done")
        except Exception as e:
            logger.error("VEML6075_init_fail " + repr(e))
            global sensorfail
            sensorfail[1] = 1

    def read_uvse():
        try:
            return [read_word(0x10, 0x07), read_word(0x10, 0x09)]
        except Exception as e:
            logger.error("VEML6075_uvse_read_fail " + repr(e))
            VEML6075.init()
            return ["fail", "fail"]

    def save_uvse(dat):
        try:
            write_file(filename("uvse_raw", "csv", 5), subdirectory.RAW, dat)  # , 2)
        except Exception as e:
            logger.error("VEML6075_uvse_save_fail " + repr(e))


class ADS_1115:
    def init():
        try:
            ################ fehlt ###################
            ADS1115(0x4A).read_adc(0, 2 / 3, 128)
            logger.info("ADS1115_init_done")
        except Exception as e:
            logger.error("ADS1115_init_fail " + repr(e))
            global sensorfail
            sensorfail[1] = 1

    def read_volt():
        try:
            return [ADS1115(0x4A).read_adc(0, 2 / 3, 128)]
            # adc_read(ADS1115(0x4A), 0, 2/3)
        except Exception as e:
            logger.error("ADS1115_volt_read_fail " + repr(e))
            ADS_1115.init()
            return ["fail"]  #'fail'

    def save_volt(dat):
        try:
            write_file(filename("volt_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("ADS1115_volt_save_fail " + repr(e))


class MuonPi:
    def read_mhas():
        try:
            return [read_logfile(muon_logfile(), "geoHash").strip()]
        except Exception as e:
            logger.error("geoHash_read_fail " + repr(e))
            return ["fail"]  #'fail'

    def save_mhas(dat):
        try:
            write_file(filename("mhas_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("geoHash_save_fail " + repr(e))

    def read_mhei():
        try:
            return [read_logfile(muon_logfile(), "geoHeightMSL")[:-1].strip()]
        except Exception as e:
            logger.error("geoHeightMSL_read_fail " + repr(e))
            return ["fail"]  #'fail'

    def save_mhei(dat):
        try:
            write_file(filename("mhei_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("geoHeightMSL_save_fail " + repr(e))

    def read_mcor():
        try:
            return [
                read_logfile(muon_logfile(), "geoLatitude")[:-3].strip(),
                read_logfile(muon_logfile(), "geoLongitude")[:-3].strip(),
            ]
        except Exception as e:
            logger.error("geoLatitude/geoLongitude_read_fail " + repr(e))
            return ["fail", "fail"]  #'fail'

    def save_mcor(dat):
        try:
            write_file(filename("mcor_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("geoLatitude/geoLongitude_save_fail " + repr(e))

    def read_mand():
        try:
            return [read_logfile(muon_logfile(), "rateAND")[:-2].strip()]
        except Exception as e:
            logger.error("rateAND_read_fail " + repr(e))
            return ["fail"]  #'fail'

    def save_mand(dat):
        try:
            write_file(filename("mand_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("rateAND_save_fail " + repr(e))

    def read_mxor():
        try:
            return [read_logfile(muon_logfile(), "rateXOR")[:-2].strip()]
        except Exception as e:
            logger.error("rateXOR_read_fail " + repr(e))
            return ["fail"]  #'fail'

    def save_mxor(dat):
        try:
            write_file(filename("mxor_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("rateXOR_save_fail " + repr(e))

    def read_coun():
        try:
            return [read_logfile(muon_logfile(), "ubloxCounter").strip()]
        except Exception as e:
            logger.error("ubloxCounter_read_fail " + repr(e))
            return ["fail"]  #'fail'

    def save_mxor(dat):
        try:
            write_file(filename("coun_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("ubloxCounter_save_fail " + repr(e))

    def read_temi():
        try:
            return [read_logfile(muon_logfile(), "temperature")[:-2].strip()]
        except Exception as e:
            logger.error("temperature_read_fail " + repr(e))
            return ["fail"]  #'fail'

    def save_temi(dat):
        try:
            write_file(filename("temi_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("temperature_save_fail " + repr(e))


"""
class SIM7000E:
	sim = None
	def init(prt = 'ttyUSB3'):
		try:
			try:
				SIM7000E.sim.close()
				logger.info('SIM7000E_handle_closed')
			except Exception as e:
				None
			SIM7000E.sim = serial.Serial('/dev/' + prt, 115200)
			time.sleep(.1)
			SIM7000E.sim.reset_input_buffer()
			#sim.write('AT\r'.encode())
			#SIM7000E.answer()
			SIM7000E.sim.write('AT+CGNSHOT\r'.encode())	# hotstart einschalten
			SIM7000E.answer()
			#### fehlt ####################
			logger.info('SIM7000E_init_done')
		except Exception as e:
			try:
				logger.error('SIM7000E_init_fail ' + repr(e))
				global transferfail
				transferfail[1] = 1
				SIM7000E.sim.close()
			except Exception as e:
				logger.error('SIM7000E_close_fail ' + repr(e))
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
						#logger.info(asw)
						return asw #.strip()
					#asw = asw + ';'
			logger.error('SIM7000E_answer_timeout ' + repr(e))
			return ['fail']
		except Exception as e:
			logger.error('SIM7000E_answer_fail ' + repr(e))
			SIM7000E.init()
			return ['fail']
	def test():
		try:
			SIM7000E.sim.write('AT\r'.encode())			# prüfen ob ansprechbar
			return SIM7000E.answer()
		except Exception as e:
			logger.error('SIM7000E_test_fail ' + repr(e))
			SIM7000E.init()
			return ['fail']
	def manual(msg):
		try:
			msg = msg + '\r'
			SIM7000E.sim.write(msg.encode())
			return SIM7000E.answer()
		except Exception as e:
			logger.error('SIM7000E_manual_fail ' + repr(e))
			SIM7000E.init()
			return ['fail']
	def activate_gnss():
		try:
			SIM7000E.sim.write('AT+CGNSHOT\r'.encode())	# hotstart einschalten
			SIM7000E.answer()
			SIM7000E.sim.write('AT+CGNSPWR=1\r'.encode())	# GNSS einschalten
			SIM7000E.answer()
		except Exception as e:
			logger.error('SIM7000E_activate_gnss_fail ' + repr(e))
			SIM7000E.init()
			return ['fail']
	def deactivate_gnss():
		try:
			SIM7000E.sim.write('AT+CGNSPWR=0\r'.encode())	# GNSS ausschalten
			SIM7000E.answer()
		except Exception as e:
			logger.error('SIM7000E_deactivate_gnss_fail ' + repr(e))
			SIM7000E.init()
			return ['fail']
	def read_scor():
		try:
			SIM7000E.sim.write('AT+CGNSINF\r'.encode())
			dat = SIM7000E.answer()[0]
			#logger.info(dat)
			#for i in range(0, len(dat)):
			if dat[0:9] == "+CGNSINF:":
				lat = dat.split(',')[3]
				lon = dat.split(',')[4]
				try:
					lat = float(lat)
					lon = float(lon)
					return [lat, lon]
				except Exception as e:
					logger.error('SIM7000E_gnss_format_fail ' + repr(e))
					return ['fail', 'fail']
			else:
				logger.error('SIM7000E_gnss_format_fail ' + repr(e))
				return ['fail', 'fail']
		except Exception as e:
			logger.error('SIM7000E_read_scor_fail ' + repr(e))
			SIM7000E.init()
			return ['fail', 'fail']
	def save_scor(dat):
		try:
			write_file(filename("scor_raw", "csv", 5), subdirectory.RAW, dat)#, 2)
		except Exception as e:
			logger.error('SIM7000E_save_scor_fail ' + repr(e))
	def create_payload(dat):
		try:
			########## fehlt; auch koordinaten aus sim7000e usw sollen in dieses payload gepackt werden
			return dat
		except Exception as e:
			logger.error('SIM7000E_create_payload_fail ' + repr(e))
	def save_payload(dat):
		try:
			write_file("payl_sim.csv", subdirectory.RAW, dat)#, 18)
		except Exception as e:
			logger.error('SIM7000E_save_payload_fail ' + repr(e))
	def send_file(pat):
		try:
			fil = open(pat, 'w')
			########################## fehlt ##################
			if SIM7000E.http_post(fil) == 0:
				fil.truncate()
			else:
				logger.error('SIM7000E_no_reception ' + repr(e))
			fil.close()
		except Exception as e:
			logger.error('SIM7000E_send_fail ' + repr(e))
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
		except Exception as e:
			logger.error('SIM7000E_http_fail ' + repr(e))
			SIM7000E.init()
			return 1
			"""


class LoRa:
    def average_payload():
        avr = bytearray()  # avr = average

        # -------- 1. Time Stamp -----------
        try:
            tme = datetime.now(timezone.utc).strftime("%H:%M:%S")
            tmestr = tme.split(":")
            sec = int(tmestr[0]) * 3600 + int(tmestr[1]) * 60 + int(float(tmestr[2]))
            avr.append(0x01)  # channel 1
            avr.append(0x02)  # data type 2 (analog input)
            avr.append(sec % (2**16) // (2**8))
            avr.append(sec % (2**8))
        except Exception as e:
            logger.error("append time fail " + repr(e))
            avr.append(0x01)
            avr.append(0x02)
            avr.append(0x00)
            avr.append(0x00)

            # --------- 2. GPS Position ----------
        try:
            mcor = MuonPi.read_mcor()
            mlat = int(float(mcor[0]) * 10000)
            mlon = int(float(mcor[1]) * 10000)
            mhei = int(float(MuonPi.read_mhei()[0]) * 100)
            avr.append(0x02)  # channel 2
            avr.append(0x88)  # gps position
            avr.append((mlat % (2**24)) // (2**16))
            avr.append((mlat % (2**16)) // (2**8))
            avr.append(mlat % (2**8))
            avr.append((mlon % (2**24)) // (2**16))
            avr.append((mlon % (2**16)) // (2**8))
            avr.append(mlon % (2**8))
            avr.append((mhei % (2**24)) // (2**16))
            avr.append((mhei % (2**16)) // (2**8))
            avr.append(mhei % (2**8))
        except Exception as e:
            logger.error("append position fail " + repr(e))
            avr.append(0x02)  # channel 2
            avr.append(0x88)  # gps position
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)

            # -------- 3. Akkuspannung ----------
        try:
            volt = int(
                average(
                    read_file(filename("volt_raw", "csv", 5), subdirectory.RAW, 2, 5)
                )
                / 32768
                * 6.144
                * 100
            )  # in centivolt angegeben
            avr.append(0x03)  # channel 3
            avr.append(0x02)  # analog input
            avr.append((volt % (2**16)) // (2**8))
            avr.append(volt % (2**8))
        except Exception as e:
            logger.error("append voltage fail " + repr(e))
            avr.append(0x03)  # channel 3
            avr.append(0x02)  # analog input
            avr.append(0x00)
            avr.append(0x00)

            # ------- 4. XOR rate ------------
        try:
            mxor = int(float(MuonPi.read_mxor()[0]) * 100)
            avr.append(0x04)  # channel 4
            avr.append(0x02)  # analog input
            avr.append((mxor % (2**16)) // (2**8))
            avr.append(mxor % (2**8))
        except Exception as e:
            logger.error("append xor fail " + repr(e))
            avr.append(0x04)  # channel 4
            avr.append(0x02)  # analog input
            avr.append(0x00)
            avr.append(0x00)

            # ------- 5. AND rate ------------
        try:
            mand = int(float(MuonPi.read_mand()[0]) * 100)
            avr.append(0x05)  # channel 5
            avr.append(0x02)  # analog input
            avr.append((mand % (2**16)) // (2**8))
            avr.append(mand % (2**8))
        except Exception as e:
            logger.error("append and fail " + repr(e))
            avr.append(0x05)  # channel 5
            avr.append(0x02)  # analog input
            avr.append(0x00)
            avr.append(0x00)

            # ------- 6. Counter rate ------------
        try:
            coun = int(float(MuonPi.read_coun()[0]))
            avr.append(0x06)  # channel 6
            avr.append(0x02)  # analog input
            avr.append((coun % (2**16)) // (2**8))
            avr.append(coun % (2**8))
        except Exception as e:
            logger.error("append counter fail " + repr(e))
            avr.append(0x06)  # channel 6
            avr.append(0x02)  # analog input
            avr.append(0x00)
            avr.append(0x00)

            # ---------- 7. Pressure --------------
        try:
            pres = int(
                average(
                    read_file(filename("pres_raw", "csv", 5), subdirectory.RAW, 2, 5)
                )
            )  # in Pa
            avr.append(0x07)  # channel 7
            avr.append(0x71)  # accelerometer
            avr.append((pres % (2**24)) // (2**16))
            avr.append((pres % (2**16)) // (2**8))
            avr.append((pres % (2**8)))
        except Exception as e:
            logger.error("append pressure fail " + repr(e))
            avr.append(0x07)  # channel 7
            avr.append(0x71)  # accelerometer
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)

            # ---------- 8. Temperature Out ----------
        try:
            temo = int(
                average(
                    read_file(filename("temp_raw", "csv", 5), subdirectory.RAW, 2, 5)
                )
            )  # in K
            # avr.append(0x08) # channel 8
            # avr.append(0x71) # accelerometer
            avr.append((temo % (2**24)) // (2**16))
            avr.append((temo % (2**16)) // (2**8))
            avr.append(temo % (2**8))
        except Exception as e:
            logger.error("append temperature fail " + repr(e))
            # avr.append(0x08) # channel 8
            # avr.append(0x71) # accelerometer
            avr.append(0x00)
            avr.append(0x00)
            avr.append(0x00)

        return avr

    def save_payload(dat):
        try:
            write_file(filename("payl_raw", "csv", 5), subdirectory.RAW, dat)
        except Exception as e:
            logger.error("LoRa_save_payload_fail " + repr(e))

    def uplinkSequenceNo():
        file_path = os.path.join(data_directory, "uplinkSequenceNo.txt")
        try:
            usn = 0
            if os.path.exists(file_path):
                fil = open(file_path, "r")
                usn = int(fil.readlines()[0])  # usn = uplinkSequenceNo
                fil.close()
            usn += 1
            # logger.error(usn, repr(e))
            fil = open(file_path, "w")
            fil.write(str(usn))
            fil.close()
            return usn
        except Exception as e:
            logger.error("LoRa_uplinkSequenceNo_fail " + repr(e))
            try:
                fil.close()
                logger.error("LoRa_uplinkSequenceNo_closed " + repr(e))
            except Exception as e:
                logger.error(repr(e))
            try:
                os.remove(os.path.dirname(__file__) + "/../raw/uplinkSequenceNo.txt")
            except Exception as e:
                logger.error(repr(e))
            with open(
                os.path.dirname(__file__) + "/../raw/uplinkSequenceNo.txt", "w"
            ) as fil:
                usn = int(random.random() * 10000 + 1)
                fil.write(str(usn))
            return usn

    def save_lora_payload(pld):
        file_path = os.path.join(data_directory, "lora_payload.txt")
        try:
            with open(file_path, "w") as fil:
                for i in range(len(pld)):
                    fil.write(str(pld[i]))
                    fil.write(";")
            return 0
        except Exception as e:
            logger.error("LoRa_save_lora_payload_fail " + repr(e))
            try:
                fil.close()
                logger.error("LoRa_lora_payload_closed " + repr(e))
            except Exception as e:
                logger.error(repr(e))
            try:
                os.remove(file_path)
            except Exception as e:
                logger.error(repr(e))
            fil = open(file_path, "w")
            fil.write(str(pld))
            fil.close()
            return 0


def init_sensors():
    global sensorfail
    sensorfail = [0, 0]
    ADS_1115.init()
    MPU6050.init()
    SEN0321.init()
    BME280.init()
    QMC5883L.init()
    VEML6075.init()
    logger.info("init_sensors_done")


def init_transfers():
    global transferfail
    transferfail = [0, 0]
    # SIM7000E.init()
    LoRa.init()
    logger.info("init_transfers_done")


def failsafe_sensors():
    global sensorfail
    # logger.info(sensorfail, repr(e))
    if sensorfail[0] == 1 and sensorfail[1] == 0:
        logger.info("sensors_restarting ...")
        init_sensors()
        logger.info("sensors_restarted")
    else:
        sensorfail[0] = sensorfail[1]
        sensorfail[1] = 0


def failsafe_transfers():
    global transferfail
    # logger.error(transferfail, repr(e))
    if transferfail[0] == 1 and transferfail[1] == 0:
        logger.info("transfers_restarting ...")
        init_transfers()
        logger.info("transfers_restarted")
    else:
        transferfail[0] = transferfail[1]
        transferfail[1] = 0


# ------------------ ADC Commands ------------------------


def adc_read(adc, num, snt):  # snt=sensitivity
    return adc.read_adc(num, snt, 128)


# ----------------- File Commands ----------------------


def write_file(
    nam: str, dir: subdirectory, val
) -> None:  # , num = 1):		# name, value, number
    try:
        pat = os.path.join(data_directory, subdirectories.get(dir), nam)  # path
        fil = open(pat, "a")
        fil.write(
            datetime.now(timezone.utc).strftime("     %Y:%m:%d;   %H:%M:%S.%f")[:-3]
        )
        i = 0
        while i < len(val):
            fil.write(";%15s" % (val[i]))
            i += 1
        fil.write(";")
        fil.write("\n")
        fil.close()
    except Exception as e:
        logger.error(str(pat) + " " + repr(e))


def read_file(
    nam: str, dir: subdirectory, num: int = 2, cnt: int = 1
) -> (
    None
):  # , num = 1):		# name, value, number(arraystelle, die ausgelesen weden soll), count(anzahl, die zurück gegeben werden soll)
    pat = os.path.join(data_directory, subdirectories.get(dir), nam)  # path
    val = []
    try:
        with open(pat, "r") as fil:
            txt = fil.readlines()
    except Exception as e:
        logger.error(e)
        return val
    for i in range(cnt):
        val += [txt[len(txt) - 1 - i].split(";")[num].strip()]
    return val


def average(dat):  # dat muss array sein
    res = 0
    n = len(dat)
    for i in range(n):
        try:
            res += float(dat[i])
        except Exception as e:
            n -= 1
    if n == 0:
        return 0
    res = res / n
    return res


def filename(
    nam: str, typ: str, tim: int, dif: int = 0
) -> (
    None
):  # dateiname, dateiendung, time nachdem neue datei erstellt werden soll, differenz um die vor gestellt wird
    min = (
        int(datetime.now(timezone.utc).strftime("%M"))
        + int(datetime.now(timezone.utc).strftime("%H")) * 60
    )  # minute
    min = min + dif
    while min % tim != 0:
        min -= 1
    hou = int(min / 60)
    if int(hou / 10) == 0:
        houstr = "0" + str(hou)  # stunde als string
    else:
        houstr = str(hou)
    min = int(min % 60)
    if int(min / 10) == 0:
        minstr = "0" + str(min)
    else:
        minstr = str(min)
    now = (
        datetime.now(timezone.utc).strftime("%Y%m%d_") + houstr + minstr
    )  # zeit für dateinamen
    namnow = nam + "_" + now + "." + typ
    return namnow


def muon_logfile(
    pat="/var/muondetector/currentWorkingFileInformation.conf",
):  # gibt Pfad der aktuellen Log-File aus
    fil = open(pat, "r")
    ret = fil.readlines()[1][:-1]
    fil.close()
    return ret


def read_logfile(pat, atr):  # dateipfad, attribut welches ausgegeben werden soll
    fil = open(pat, "r")  # liest einen Wert eines Attributs einer Log-File aus
    txt = fil.readlines()
    fil.close()
    for i in range(-1, -50, -1):
        if txt[i][20 : 20 + len(atr)] == atr:
            return txt[i].split(" ")[2].strip()
    return "fail"


# ----------------- Sending Commands ----------------------


def transmit(hdl, trm, dev):  # handle, zu sendender String, Geraet: s/l sim/lora
    trm = dev + ";" + trm + "#"
    trm = bytes(trm, "utf-8")  # transmitting: zu sendende Zeichen
    # logger.info("\n" + str(hdl.write(trm))+" Zeichen wurden gesendet")
    hdl.write(trm)


def receive(hdl):  # hdl=handle
    while hdl.in_waiting:
        rcv = hdl.read_until(b"#").decode("utf-8")  # receiving: empfangene Zeichen
        rcv = rcv[:-1]
        logger.info(rcv)


# ----------------- GPS Commands -------------------------


def read_gps(hdl, tmo):  # hdl=handle, tmo=timeout
    hdl.reset_input_buffer()
    now = time.time()
    while time.time() < now + tmo:
        if hdl.in_waiting:
            dat = hdl.readline().decode("utf-8")
            # logger.info(dat)
            if dat[0:6] == "$GPRMC":
                # logger.info('GPRMC detected')
                cor = pynmea2.parse(dat)  # koordinaten
                lat = round(cor.latitude, 5)
                lon = round(cor.longitude, 5)
                # out = str(lat) + ',' + str(lon)
                return [lat, lon]
        else:
            time.sleep(0.05)


def maps_str(cor):  # cor=koordinaten mit , getrennt
    out = "https://www.google.de/maps?q=" + str(cor[0]) + "," + str(cor[1])
    return out


# -------------------- Time Commands --------------------


def set_starttime():  # Startzeit setzen
    global starttime
    starttime = time.time()


def sleep_until(tme):  # Pause bis Programmlaufzeit bestimmte Zeit ist
    global starttime
    time.sleep(tme - ((time.time() - starttime) % tme))
    return 1


def past_seconds(
    tme,
):  # vergangene Sekunden des Tages # tme = übergebene Zeit in Form hh:mm:ss
    tme = tme.split(":")
    # logger.info(int(tme[0]) * 3600 + int(tme[1]) * 60 + int(float(tme[2])))
    return int(tme[0]) * 3600 + int(tme[1]) * 60 + int(float(tme[2]))
