#!/usr/bin/env python
import serial
import threading
import Queue
import sys
import time

class printQueueThread(threading.Thread):
    def __init__(self, q):
	threading.Thread.__init__(self)
	self.name = "SerialPrinter"
	self.q = q
    def run(self):
	justPrintIt(self.q)

class serialReadThread(threading.Thread):
    def __init__(self, ser, q):
	threading.Thread.__init__(self)
	self.name = "SerialReader"
	self.q = q
	self.ser = ser
    def run(self):
	mySerialRead(self.ser, self.q)

class serialWriteThread(threading.Thread):
    def __init__(self, ser, q):
	threading.Thread.__init__(self)
	self.name = "SerialWriter"
	self.q = q
	self.ser = ser
    def run(self):
	mySerialWrite(self.ser, self.q)

def justPrintIt(q):
    while True:
	if not q.empty():
	    sys.stdout.write(str(q.get()))

def mySerialRead(ser, q):
    msgArray = []
    msgStr = ""
    i = 3
    recentMessage = False
    while True:
	msg = ser.read()
	if (msg != ""):
	    q.put(msg)
	    recentMessage = True
	    i = 0
	else:
	    i += 1
	    if( (recentMessage == True) and (i > 3)):
		recentMessage = False
		i = 0
		q.put("\n")

def mySerialWrite(ser, q):
    while True:
	myMsg = raw_input()
	if (myMsg != ""):
	    try:
		ser.write(myMsg)
		#q.put("\n")
	    except:
		q.put(str("Could not Send: " + str(myMsg)))
    

def initalize():
    q = Queue.Queue()
    # Change this to have defaults, and allow input args
    port = "/dev/ttyACM0"
    myBaud = 9600
    myTimeOut = .01
    # Catch Error of wrong port: display all open ports instead
    ser = serial.Serial(port, baudrate = myBaud, timeout = myTimeOut)
    connected = False 
    print "\t*****************************************"
    print "\t* Opening Serial Port Communication\t*"
    print "\t*\tPort:    \t", port, "\t*"
    print "\t*\tBaudrate:\t", myBaud, "\t\t*"
    print "\t*\tTime Out:\t", myTimeOut, "\t\t*"
    while not connected:
	ser.read()
	connected = True
    print "\t*\tSerial Communication Open\t*"
    print "\t*****************************************"

    printThread = printQueueThread(q)
    printThread.start()
    readThread = serialReadThread(ser, q)
    readThread.start()
    writeThread = serialWriteThread(ser, q)
    writeThread.start()

    printThread.join()
    readThread.join()
    writeThread.join()

    #while True:
	#time.sleep(1)
    return






initalize()

        
