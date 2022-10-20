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
from python import fct

fct.set_starttime()

while True:
	pld = "testpayload"
    msge = fct.LoRa.create_message(bytearray(pld.encode()))
	fct.sleep_until(60)
