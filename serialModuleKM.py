#!/usr/bin/env python
import serial
import threading
import Queue
import sys
import time
import argparse, textwrap

class printQueueThread(threading.Thread):
    def __init__(self, q, mode):
	threading.Thread.__init__(self)
	self._stop = threading.Event()
	self.name = "SerialPrinter"
	self.q = q
	self.daemon = True
	self.mode = mode
    def run(self):
	justPrintIt(self.q, self.mode)
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

def interface():
    args = argparse.ArgumentParser(
	prog='SerialModuleKM.py',
	formatter_class=argparse.RawDescriptionHelpFormatter,
	description='This is a simple python based serial monitor')
    args.add_argument('-p', '--port', default="/dev/ttyACM0", help='Serial Port')
    args.add_argument('-b', '--baudrate', type=int, default=9600, help='Baudrate')
    args.add_argument('-t', '--time-out', type=float, default=0.01, help='Time Out')
    args.add_argument('-m', '--mode', default="s", help="Mode for Inputs: 's':String, 'b':Binary, 'h':Hex")
    args = args.parse_args()
    return args

def justPrintIt(q, mode):
    i = 3
    recentMessage = False
    while True:
	if not q.empty():
	    msg = q.get()
	    if (mode == 's'):
		sys.stdout.write(str(msg))
	    elif (mode == 'b'):
		binMsg = "".join("{:08b}".format(ord(c), 'b') for c in str(msg))
		sys.stdout.write(str(binMsg))
		sys.stdout.write(":")
	    elif (mode == 'h'):
		hexMsg = ":".join("{:02x}".format(ord(c)) for c in str(msg))
		sys.stdout.write(str(hexMsg))
		sys.stdout.write(":")
	    q.task_done()
	    recentMessage = True
	    i = 0
	else:
	    time.sleep(0.01)
	    i += 1
	    if( (recentMessage == True) and (i > 3)):
		recentMessage = False
		i = 0
		sys.stdout.write("\n")

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
		#q.put("\n")

def mySerialWrite(ser, q):
    while True:
	myMsg = raw_input()
	if (myMsg != ""):
	    try:
		ser.write(myMsg)
	    except:
		q.put(str("Could not Send: " + str(myMsg)))
    

def initalize(args):
    q = Queue.Queue()
    # Change this to have defaults, and allow input args
    port = args.port
    myBaud = args.baudrate
    myTimeOut = args.time_out
    mode = args.mode
    mode = mode.lower()
    modeDict = {'s': 's', 'str':'s', 'string':'s',\
		'b': 'b', 'bin':'b', 'binary':'s',\
		'h': 'h', 'hex':'h', 'hexadecimal':'h'}
    if (mode not in modeDict):
	mode = 's'
	print "   *** RX is being treated as characters, Your input was invalid                *"
	print "   *** Please use either  's' for String, 'b' forBinary, or 'h' for Hexadecimal *"
    # Catch Error of wrong port: display all open ports instead
    ser = serial.Serial(port, baudrate = myBaud, timeout = myTimeOut)
    connected = False 
    print "\t*****************************************"
    print "\t* Opening Serial Port Communication\t*"
    print "\t*\tPort:      \t", port, "\t*"
    print "\t*\tBaudrate:  \t", myBaud, "\t\t*"
    print "\t*\tTime Out:  \t", myTimeOut, "\t\t*"
    print "\t*\tPrint Mode:\t'", mode, "'\t\t*"
    while not connected:
	ser.read()
	connected = True
    print "\t*\tSerial Communication Open\t*"
    print "\t*****************************************"

    printThread = printQueueThread(q, mode)
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
	ser.close()
	time.sleep(0.001)
	print "\t*****************************************"
	print "\t* Exited Serial Port Communication\t*"
	print "\t*****************************************"


    return






initalize(interface())

        
