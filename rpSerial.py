#!/usr/bin/env python
import zlib
import time
import serial
import threading
import Queue
import serial
'''
Raspberry Pi Serial module 

Useful sites:
https://sites.google.com/site/semilleroadt/raspberry-pi-tutorials/gpio

disable the process on the gpio pin

'''
    # Open Serial Communication
    # Send (0,0) coordinate, 
    #   Wait for confimation of recieve
    #   Wait for ready payload
    #   Start Pulling from Queue Loop
    #     ...
    #   Verify Queue is done (check to see that neither
    #		rasterQ nor edQ threads are running)
    # Close Serail Comm

    # PI -> MSP430
    # Startx: 	0x02
    # Command: 	0x0B
    # Escape Char if 03 occurs in payload or 
    # Payload 	0xNNNNNNNN
    # Checksum 	0xNN
    # EndX: 	0x03

    # Recieve MSP430 -> PI
    # Start	0x02
    # Acknow	0x06
    # Command	0x0B
    # ETx	0x03


    # Ready for more MSP430 -> PI
    # Start	0x02
    # SendM	0x1B
    # End	0x03

    # Pi will respond PI -> MSP430
    # Start 	0x02
    # Command	0x1B
    # End	0x03
startX 	= 0x02
endX 	= 0x03
acknow 	= 0x06
burn 	= 0x0B
esc 	= 0x1B
error	= 0x3f
readyB 	= 0x4d


def sendX(ser, messages):
    try:
	ser.write(str(messages))
    except:
	print "Failed to send\t\trpSerial.sendX"
	return 1
    return 0


def receiveX(ser, expectations):
    i = 0
    msgD = {}
    msgArray = []
    while True:
	try:
	    msg = ser.read()
	except serial.serialutil.SerialException:
	    msg = ''
	if (msg == '') :
	    i +=1
	    if (i >= 10):
		break
	else :
	    msgArray.append(msg)
	    print "In: \t", (str(msg))
	    msgD[msg] = 1
	    i = 0
    for j in range(len(expectations)):
	if expectations[j] not in msgD:
	    print msgArray
	    return 1
    print "MSG ARR", msgArray
    return 0


    
def checkSum8(payload):
    specialChar = [startX, endX, esc]
    byte4 = (payload >> 24) & 0xFF
    byte3 = (payload >> 16) & 0xFF
    byte2 = (payload >> 8) & 0xFF
    byte1 = payload & 0xFF
    msgA = []
    payloadEscaped = 0x00
    #print hex(payload)
    if (byte4 in specialChar):
	msgA.append(esc)
    msgA.append(byte4)
    if (byte3 in specialChar):
	msgA.append(esc)
    msgA.append(byte3)
    if (byte2 in specialChar):
	msgA.append(esc)
    msgA.append(byte2)
    if (byte1 in specialChar):
	msgA.append(esc)
    msgA.append(byte1)
    #for i in range(len(msgA)):
	#payloadEscaped = (payloadEscaped << 8) | msgA[i]
    
    checkSum = 256 - ((byte4 + byte3 + byte2 + byte1) & 0x00FF)
    if (checkSum in specialChar):
	checkSum = (esc << 8) | checkSum

    return msgA, checkSum

def sendPix(ser, payload):
    checksum = 0x00
    #checksum32 = zlib.adler32(str(payload))
    #checksum = checksum | (checksum32 & 0x000000FF)
    payloadAr, checksum = checkSum8(payload)
    
    sendX(ser, startX)
    sendX(ser, burn)
    for i in range(len(payloadAr)):
	sendX(ser, payloadAr[i])
    sendX(ser, checksum)
    sendX(ser, endX)
    reciv = receiveX(ser, [startX, acknow, burn, endX] )
    print "Recieved\t", reciv

    return 

def logicFlow(ser, payload):
    maxWait = 5
    ser.write("HELLOAGAIN")
    while True:
	sendPix(ser, payload)
	reciv = receiveX(ser, [startX, acknow, burn, endX] )
	if reciv == 0:
	    break
    startTime = time.clock()
    while True:
	reciv = receiveX(ser, [startX, readyB, endX] )
	if reciv == 0:
	    break
	if ((1000*(time.clock() - startTime)) > maxWait):
	    break
    sendX(ser, startX)
    sendX(ser, readyB)
    sendX(ser, endX)
    return
    



def rpSerialManager(q, ser):
    pixCount = 0
    i = 0
    connected = False
    while not connected:
	serin = ser.read()
	connected = True
    ser.write("HSDF")

    ser.write("HEN")
    while (i < 5):
        time.sleep(2)
	i += 1
	while not q.empty():
	    print "HII"
	    payload = q.get()
	    logicFlow(ser, payload)
	    pixCount += 1
	    q.task_done()
	    i = 0
    print "DONE ", pixCount," ", pixCount/1200

   


