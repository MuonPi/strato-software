#!/usr/bin/env python3
import sys
import time
from datetime import datetime
import smbus
import serial
import pynmea2
import random
import os
#import sched
from lib import fct

# fct.set_starttime()

# while True:

# fct.init_transfers()
# time.sleep(1)

# pld = fct.LoRa.average_payload()
# print(" ".join("%02x" % b for b in pld))

# msg = fct.LoRa.create_message(bytearray(pld))#.encode()))
# print(" ".join("%02x" % b for b in msg))

# print(msg)

# asw = fct.LoRa.send_message(msg)
# print(asw)

fct.BME280.init()

# fct.sleep_until(60)
