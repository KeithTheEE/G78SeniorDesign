#!/usr/bin/env python
import serial
import threading
import Queue
import sys
import time
import argparse, textwrap
import glob
import struct

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
    def __init__(self, ser, q, modeW):
	threading.Thread.__init__(self)
	self._stop = threading.Event()
	self.name = "SerialWriter"
	self.q = q
	self.ser = ser
	self.mode = modeW
	self.daemon = True
    def run(self):
	mySerialWrite(self.ser, self.q, self.mode)
    def stop(self):
	self._stop.set()
    def stopped(self):
	return self._stop.isSet()

def interface():
    defPort = serial_ports()
    if (len(defPort) > 0):
	defPort = defPort[-1]
    else:
	defPort = "/dev/ttyACM0"
    args = argparse.ArgumentParser(
	prog='SerialModuleKM.py',
	formatter_class=argparse.RawDescriptionHelpFormatter,
	description='This is a simple python based serial monitor')
    args.add_argument('-p', '--port', default=defPort, help='Serial Port')
    args.add_argument('-b', '--baudrate', type=int, default=9600, help='Baudrate')
    args.add_argument('-t', '--time-out', type=float, default=0.01, help='Time Out')
    args.add_argument('-m', '--mode', default="s", help="Mode for Incoming Data: 's':String, 'b':Binary, 'h':Hex")
    args.add_argument('-w', '--write-mode', default="s", help="Mode for User Input Data: 's':String, 'b':Binary, 'h':Hex")
    args = args.parse_args()
    return args

def serial_ports():
    """
	http://stackoverflow.com/questions/12090503
	    Author: http://stackoverflow.com/users/300783/thomas
	Lists serial ports
    :raises EnvironmentError:
        On unsupported or unknown platforms
    :returns:
        A list of available serial ports
    """
    if sys.platform.startswith('win'):
        ports = ['COM' + str(i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this is to exclude your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')
    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def justPrintIt(q, mode):
    i = 3
    recentMessage = False
    while True:
	if not q.empty():
	    msg = q.get()
	    if (msg[0] == 'E'):
		sys.stdout.write(str(msg[1]))
	    else:
		msg = msg[1]
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
	try:
	    msg = ser.read()
	except:
	    msg = ""
	if (msg != ""):
	    q.put(("R", msg))
	    recentMessage = True
	    i = 0
	else:
	    i += 1
	    time.sleep(0.001)
	    if( (recentMessage == True) and (i > 3)):
		recentMessage = False
		i = 0
		#q.put("\n")

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
    return hexMsg

def binParse(rawMsg):
    # Parsing Input string
    tempMsg = rawMsg.upper()
    tempMsg = tempMsg.split('0B')
    tempMsg = "".join(tempMsg)
    if(len(tempMsg) > 2):
	if ((tempMsg[0] == "0") and (tempMsg[1] == "B")):
	    tempMsg = tempMsg[2:]
    binVals = set('01')
    tempMsg = "".join(c for c in tempMsg if c in binVals)

    # Varibles used
    binMsg = ""
    byteHold = 0x00
    byteArr = []

    # Start with most sig byte
    sigBytePoint = len(tempMsg)% 8 
    if (sigBytePoint != 0):
	# first len(tempMsg)% 8 will be stuffed into own byte 
	# Packing the first byte
	for i in range(sigBytePoint):
	    if (tempMsg[i] == '0'):
		byteHold = byteHold << 1
		byteHold = byteHold | 0x00
	    elif (tempMsg[i] == '1'):
		byteHold = byteHold << 1
		byteHold = byteHold | 0x01
	byteArr.append(byteHold)
    # Rest of sting/bytes
    byteHold = 0x00
    for i in range(sigBytePoint, len(tempMsg)):
	if (tempMsg[i] == '0'):
	    byteHold = byteHold << 1
	    byteHold = byteHold | 0x00
	elif (tempMsg[i] == '1'):
	    byteHold = byteHold << 1
	    byteHold = byteHold | 0x01
	if ((i-sigBytePoint+1)%8 == 0):
	    byteArr.append(byteHold)
	    byteHold = 0x00

    binMsg = "".join(chr(x) for x in byteArr)
    #print hex(binMsg)
    return binMsg

def parseAll(rawMsg):
    if(len(rawMsg) > 2):
	if (rawMsg[0] == '0'):
	    if ((rawMsg[1] == 'b') or (rawMsg[1] == 'B')):
		cleanMsg = binParse(rawMsg)
	    elif ((rawMsg[1] == 'x') or (rawMsg[1] == 'X')):
		cleanMsg = hexParse(rawMsg)
	    else:
		cleanMsg = rawMsg
	else:
	    cleanMsg = rawMsg
    else:
	cleanMsg = rawMsg
    return cleanMsg

def mySerialWrite(ser, q, mode):

    while True:
	myMsg = raw_input() # If in python 3, change raw_input() to input()
	if (myMsg != ""):
	    if (mode == 'a'):
		myMsg = parseAll(myMsg)		
		ser.write(myMsg)
	    elif (mode == 'h'):
		myMsg = hexParse(myMsg)		
		ser.write(myMsg)
	    elif (mode == 'b'):
		myMsg = binParse(myMsg)		
		ser.write(myMsg)
	    else:
		ser.write(myMsg)
	    #try:
		#print "KM"#ser.write(myMsg)
	    #except:
		#q.put(("E", str("Could not Send: " + str(myMsg))))
	else:
	    time.sleep(0.001)
    

def initalize(args):
    q = Queue.Queue()
    port = args.port
    valPorts = serial_ports()
    myBaud = args.baudrate
    myTimeOut = args.time_out

    # Verify that the input mode is valid, if not, treat RX as a char
    mode = args.mode
    mode = mode.lower()
    modeDict = {'s': 's', 'str':'s', 'string':'s',\
		'b': 'b', 'bin':'b', 'binary':'b',\
		'h': 'h', 'hex':'h', 'hexadecimal':'h'}
    if (mode not in modeDict):
	mode = 's'
	print "   *** RX is being treated as characters, Your input was invalid                *"
	print "   *** Please use either  's' for String, 'b' forBinary, or 'h' for Hexadecimal *"
    else:
	mode = modeDict[mode]

    wmode = args.write_mode
    wmode = wmode.lower()
    modeDict = {'s': 's', 'str':'s', 'string':'s',\
		'b': 'b', 'bin':'b', 'binary':'b',\
		'h': 'h', 'hex':'h', 'hexadecimal':'h',\
		'a': 'a', 'all':'a', 'any':'a', 'anything':'a'}
    if (wmode not in modeDict):
	wmode = 's'
	print "   *** TX is being treated as characters, Your input was invalid                *"
	print "   *** Please use either  's' for String, 'b' forBinary, or 'h' for Hexadecimal *"
    else:
	wmode = modeDict[wmode]

    # Catch Error of wrong port: display all open ports instead
    try:
	ser = serial.Serial(port, baudrate = myBaud, timeout = myTimeOut)
    except:
	print "Could not connect to Serial Port"
	if (len(valPorts) == 0):
	    print "\tNo valid ports found"
	else:
	    print "\tValid ports:"
	    for i in range(len(valPorts)):
		print "\t\t", valPorts[i]
	return

    # Start up
    connected = False 
    print "\t*****************************************"
    print "\t* Opening Serial Port Communication\t*"
    print "\t*\tPort:      \t", port, "\t*"
    print "\t*\tBaudrate:  \t", myBaud, "\t\t*"
    print "\t*\tTime Out:  \t", myTimeOut, "\t\t*"
    print "\t*\tPrint Mode:\t'", mode, "'\t\t*"
    print "\t*\tWrite Mode:\t'", wmode, "'\t\t*"
    while not connected:
	ser.read()
	connected = True
    print "\t*\tSerial Communication Open\t*"
    print "\t*****************************************"

    printThread = printQueueThread(q, mode)
    readThread = serialReadThread(ser, q)
    writeThread = serialWriteThread(ser, q, wmode)
    
    try: 
	printThread.start()
	readThread.start()
	writeThread.start()
	while True:
	    time.sleep(1)
    except(KeyboardInterrupt, SystemExit):
	# Allows for clean exiting
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

        
