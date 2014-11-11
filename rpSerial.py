#!/usr/bin/env python
import zlib
import time
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
    # Payload 	0xNNNNNNNN
    # Checksum 	0xNN
    # EndX: 	0x03

    # Recieve MSP430 -> PI
    # Start	0x02
    # Acknow	0xAA
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
startX = 0x02
burn = 0x0B
endX = 0x03
acknow = 0xAA
readyB = 0x1B


def sendX(ser, messages):
    try:
	ser.write(str(messages))
    except:
	print "Failed to send"
	return 1
    return 0


def receiveX(ser, expectations):
    i = 0
    msgD = {}
    msgArray = []
    while True:
	msg = ser.read()
	if (msg == '') :
	    i +=1
	    if (i >= 3):
		break
	else :
	    msgArray.append(msg)
	    #print "In: \t", (str(msg))
	    msgD[msg] = 1
	    i = 0
    for j in range(len(expectations)):
	if expectations[j] not in msgD:
	    print msgArray
	    return 1
    print msgArray
    return 0


    


def sendPix(ser, payload):
    checksum = 0x00
    checksum32 = zlib.adler32(str(payload))
    checksum = checksum | (checksum32 & 0x000000FF)
    
    sendX(ser, startX)
    time.sleep(.1)
    sendX(ser, burn)
    time.sleep(.1)
    sendX(ser, payload)
    time.sleep(.1)
    sendX(ser, checksum)
    time.sleep(.1)
    sendX(ser, endX)
    time.sleep(.1)

    quality = receiveX(ser, [startX, acknow, burn, endX])
    print "\t\tVal: ", quality

    return 0


