#!/usr/bin/env python
import serial
import threading
import Queue
import sys
import time

class printQueueThread(threading.Thread):
    def __init__(self, q):
	threading.Thread.__init__(self)
	self._stop = threading.Event()
	self.name = "SerialPrinter"
	self.q = q
	self.daemon = True
    def run(self):
	justPrintIt(self.q)
    def stop(self):
	self._stop.set()
    def stopped(self):
	return self._stop.isSet()


class serialReadThread(threading.Thread):
    def __init__(self, ser, q):
	threading.Thread.__init__(self)
	self._stop = threading.Event()
	self.name = "SerialReader"
	self.q = q
	self.ser = ser
	self.daemon = True
    def run(self):
	mySerialRead(self.ser, self.q)
    def stop(self):
	self._stop.set()
    def stopped(self):
	return self._stop.isSet()


class serialWriteThread(threading.Thread):
    def __init__(self, ser, q):
	threading.Thread.__init__(self)
	self._stop = threading.Event()
	self.name = "SerialWriter"
	self.q = q
	self.ser = ser
	self.daemon = True
    def run(self):
	mySerialWrite(self.ser, self.q)
    def stop(self):
	self._stop.set()
    def stopped(self):
	return self._stop.isSet()

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
    readThread = serialReadThread(ser, q)
    writeThread = serialWriteThread(ser, q)
    
    try: 
	printThread.start()
	readThread.start()
	writeThread.start()
	while True:
	    time.sleep(1)
    except(KeyboardInterrupt, SystemExit):
	writeThread.stop()
	readThread.stop()
	printThread.stop()
	print "\t*****************************************"
	print "\t* Exited Serial Port Communication\t*"
	print "\t*****************************************"


    return






initalize()

        
