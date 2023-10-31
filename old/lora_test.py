#!/usr/bin/env python3
import sys
import time
from datetime import datetime
import serial
import random
import os
# import sched
# from python import fct

# fct.set_starttime()

# while True:



class LoRa:

    def init(prt = 'ttyUSB0'):
        global lor
        lor = serial.Serial('/dev/' + prt, 115200)
        #print(LoRa.answer())
        time.sleep(.2)
        lor.reset_input_buffer()
        print(LoRa.answer(5))
        #### fehlt ####################
        print('LoRa_init_done')


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


    def send_message(msg):
        lor.write(msg)
        #return LoRa.answer()

    
    def answer(tmo = 10):
        buf = bytearray()
        chk = bytearray()
        now = time.time()
        while time.time() < now + tmo:
            while(lor.in_waiting > 0):
                incomingByte = lor.read()
                buf += incomingByte
            # print(buf)
            if(len(buf) < 4):
                time.sleep(0.1)
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
            time.sleep(0.1)
        return bytearray()







LoRa.init()

# fct.init_transfers()
time.sleep(1)

# pld = bytearray()
# pld.append(0x00)
# pld.append(0x00)
# pld.append(0x00)
# pld.append(0x01)
# pld += b'testmessage'

# print("payload:")
# print(" ".join("%02x" % b for b in pld))

# print("message:")
# msg = LoRa.create_message(bytearray(pld))

msg = bytearray()
msg.append(0xf9)
msg.append(0x2b)
msg.append(0x00)
msg.append(0x00)
msg.append(0x05)
msg.append(0x54)
msg.append(0x01)
msg.append(0x02)
msg.append(0x89)
msg.append(0x1b)
msg.append(0x02)
msg.append(0x88)
msg.append(0x00)
msg.append(0x00)
msg.append(0x00)
msg.append(0x00)
msg.append(0x00)
msg.append(0x00)
msg.append(0xff)
msg.append(0xf9)
msg.append(0x5c)
msg.append(0x03)
msg.append(0x02)
msg.append(0x01)
msg.append(0x80)
msg.append(0x04)
msg.append(0x02)
msg.append(0x00)
msg.append(0xf9)
msg.append(0x05)
msg.append(0x02)
msg.append(0x00)
msg.append(0x0e)
msg.append(0x06)
msg.append(0x02)
msg.append(0x00)
msg.append(0x2e)
msg.append(0x07)
msg.append(0x71)
msg.append(0x04)
msg.append(0xcb)
msg.append(0x66)
msg.append(0x07)
msg.append(0xad)
msg.append(0x37)
msg.append(0x46)
msg.append(0x41)

print("message:")
print(" ".join("%02x" % b for b in msg))
LoRa.send_message(msg) # bytes(msg))

# time.sleep(1)

# while True:
#     print(lor.read())
#     time.sleep(0.001)

for i in range(5):
    asw = LoRa.answer()
    print(asw)
    if(asw == bytearray(b'EV_TXCOMPLETE')):
        print("Sending Completed")
        break

# fct.sleep_until(60)
