#!/usr/bin/env python
import zlib
import time
import serial
import threading
import Queue
import sys
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

startXc 	= "0x02"
endXc 		= "0x03"
acknowc 	= "0x06"
burnc 		= "0x0B"
escc		= "0x1B"
errorc		= "0x3f"
readyBc 	= "0x4d"

def hexParse(rawMsg):
    # Parsing
    tempMsg = rawMsg.upper()
    tempMsg = tempMsg.split('0X')
    tempMsg = "".join(tempMsg)
    tempMsg = tempMsg.split('\X')
    tempMsg = "".join(tempMsg)
    hexMsg = 0x00
    if(len(tempMsg) > 2):
	if ((tempMsg[0] == "0") and (tempMsg[1] == "X")):
	    tempMsg = tempMsg[2:]
    hexVals = set('0123456789ABCDEF')
    tempMsg = "".join(c for c in tempMsg if c in hexVals)

    # Varibles used
    hexMsg = ""
    byteHold = 0x00
    byteArr = []

    
    # Start with most sig byte
    sigBytePoint = len(tempMsg)% 2
    if (sigBytePoint == 1):
	# Pack the first Byte
	if (tempMsg[0] == '0'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x00
	elif (tempMsg[0] == '1'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x01
	elif (tempMsg[0] == '2'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x02
	elif (tempMsg[0] == '3'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x03
	elif (tempMsg[0] == '4'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x04
	elif (tempMsg[0] == '5'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x05
	elif (tempMsg[0] == '6'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x06
	elif (tempMsg[0] == '7'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x07
	elif (tempMsg[0] == '8'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x08
	elif (tempMsg[0] == '9'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x09
	elif (tempMsg[0] == 'A'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0A
	elif (tempMsg[0] == 'B'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0B
	elif (tempMsg[0] == 'C'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0C
	elif (tempMsg[0] == 'D'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0D
	elif (tempMsg[0] == 'E'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0E
	elif (tempMsg[0] == 'F'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0F	
    	byteArr.append(byteHold)
    # Rest of sting/bytes
    byteHold = 0x00

    action = 0
    for i in range(sigBytePoint, len(tempMsg)):
	if (tempMsg[i] == '0'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x00
	elif (tempMsg[i] == '1'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x01
	elif (tempMsg[i] == '2'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x02
	elif (tempMsg[i] == '3'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x03
	elif (tempMsg[i] == '4'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x04
	elif (tempMsg[i] == '5'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x05
	elif (tempMsg[i] == '6'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x06
	elif (tempMsg[i] == '7'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x07
	elif (tempMsg[i] == '8'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x08
	elif (tempMsg[i] == '9'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x09
	elif (tempMsg[i] == 'A'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0A
	elif (tempMsg[i] == 'B'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0B
	elif (tempMsg[i] == 'C'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0C
	elif (tempMsg[i] == 'D'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0D
	elif (tempMsg[i] == 'E'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0E
	elif (tempMsg[i] == 'F'):
	    byteHold = byteHold << 4
	    byteHold = byteHold | 0x0F
	action += 1
	if (action == 2):
	    byteArr.append(byteHold)
	    byteHold = 0x00
	    action = 0

	
    hexMsg = "".join(chr(x) for x in byteArr)
    #print hexMsg
    return hexMsg


def sendX(ser, messages):
    #messages = hexParse(str(messages))
    #print str(messages)
    try:
	ser.write(str(messages))
    except:
	print "Failed to send\t", messages, "\trpSerial.sendX"
	return 1
    return 0


def receiveX(ser, expectations):
    i = 0
    msgD = {}
    msgArray = []
    anything = False
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
	    hexM = ":".join("{:02x}".format(ord(c)) for c in str(msg))
	    msgArray.append(hexM)
	    #print "In: \t", (str(msg))
	    msgD[msg] = 1
	    i = 0
	    anything = True
    if (anything == False):
	return 2
    for j in range(len(expectations)):
	if expectations[j] not in msgD:
	    #print msgArray
	    return 1
    #print "MSG ARR", msgArray
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
	msgA.append(esc)
	checkSum = (esc << 8) | checkSum
    msgA.append(checkSum & 0xff)
    #print hex(payload), hex(checkSum), hex(byte4),hex(byte3),hex(byte2),hex(byte1)

    # I hate doing this, it's ugly, but it will get this to work
    #for i in range(len(msgA))

    return msgA

def sendPix(ser, payload):
    checksum = 0x00
    #checksum32 = zlib.adler32(str(payload))
    #checksum = checksum | (checksum32 & 0x000000FF)
    payloadAr = checkSum8(payload)
    #print payload
    sendX(ser, chr(startX))
    sendX(ser, chr(burn))
    hexMsg = "".join(chr(x) for x in payloadAr)
    sendX(ser, hexMsg)
    sendX(ser, chr(endX))
    #reciv = receiveX(ser, [startX, acknow, burn, endX] )
    #print "Recieved\t", reciv

    return 

def logicFlow(ser, payload):
    maxWait = 5
    startTime = time.time()
    anything = False
    print "Payload"
    waited = 0
    while True:
	if (waited < 1):
	    sendPix(ser, payload)
	    waited += 1
	#print payload
	#print (time.time() - startTime)
	reciv = receiveX(ser, [chr(startX), chr(acknow), chr(burn), chr(endX), chr(readyB)] )
	if reciv == 0:
	    break
	if (reciv != 2):
	    #print reciv
	    anything = True
	if ((time.time() - startTime) > maxWait):
	    if (anything == False):
		print "Communication Lost"
		return 1
		#sys.exit()
		break
	    # in the last five seconds we got something, try again
	    anything = False
	    startTime = time.time()
    startTime = time.time()
    print "waiting"
    while True:
	reciv = receiveX(ser, [chr(startX), chr(readyB), chr(endX)] )
	#reciv = 0
	if reciv == 0:
	    break
	if (reciv != 2):
	    anything = True
	if ((time.time() - startTime) > maxWait):
	    if (anything == False):
		print "Communication Lost"
		return 1
		#sys.exit()
		break
	    # in the last five seconds we got something, try again
	    anything = False
	    startTime = time.time()
    sendX(ser, chr(startX))
    sendX(ser, chr(acknow))
    sendX(ser, chr(readyB))
    sendX(ser, chr(endX))
    return 0
    
def phase2(ser):
    maxWait = 5
    reciv = 2 # This is the return val for no serial response
    startTime = time.time()
    while reciv == 2:
	reciv = receiveX(ser, [chr(startX), chr(acknow), chr(burn), chr(endX)] )
	if ((time.time() - startTime) > maxWait):
	    # I've waited for 5 seconds, I give up
	    return 2 
    if reciv == 1:
	# I recieved something, but it wasn't what I wanted
	return 1
    if reciv == 0:
	return 0
    else:
	return 2


def phase3(ser):
    maxWait = 5
    reciv = 2 # This is the return val for no serial response
    startTime = time.time()
    while reciv == 2:
	reciv = receiveX(ser, [chr(startX), chr(endX), chr(readyB)] )
	if ((time.time() - startTime) > maxWait):
	    # I've waited for 5 seconds, I give up
	    return 2 
    if reciv == 1:
	# I recieved something, but it wasn't what I wanted
	return 1
    if reciv == 0:
	return 0
    else:
	return 2

def logicFlow2(ser, payload):
    maxWait = 20
    startTime = time.time()
    
    p2 = 1
    while p2 == 1:
	# Phase 1 0x02PAYCHECK03 Pi->MSP
        sendPix(ser, payload)
	# Phase 2 0x02060b03 MSP<-Pi
	p2 = phase2(ser)
        startTime = time.time()
	if ((time.time() - startTime) > maxWait):
	    print "P2 took too long"
	    break
    if p2 == 2:
	print "Communication Lost"
	return 1

    # Phase 3 0x024d03  MSP<-Pi
    p3 = 1
    while p3 == 1:
	p3 = phase3(ser)
        startTime = time.time()
	if ((time.time() - startTime) > maxWait):
	    print "P2 took too long"
	    break
    if p3 == 2:
	print "Communication Lost"
	return 1

    # Phase 4: 0x02064d03 Pi->MSP
    sendX(ser, chr(startX))
    sendX(ser, chr(acknow))
    sendX(ser, chr(readyB))
    sendX(ser, chr(endX))
    return 0



def rpSerialManager(q, ser):
    pixCount = 0
    i = 0
    connected = False
    while not connected:
	serin = ser.read()
	connected = True
    receiveX(ser, [chr(startX), chr(error), chr(endX)])
    
    #ser.write("HSDF")

    #ser.write("HEN")
    while (i < 5):
        time.sleep(2)
	i += 1
	while not q.empty():
	    payload = q.get()
	    check = logicFlow2(ser, payload)
	    if check == 1:
		# Communictation lost, return error code 1: Comm Lost
		return 1
	    pixCount += 1
	    q.task_done()
	    print pixCount, q.qsize(), q.empty()
	    i = 0
    print "*******************************"
    print "DONE ", pixCount," ", pixCount/1200
    return 0
   


