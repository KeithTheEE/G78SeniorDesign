#!/usr/bin/env python
from PIL import Image, ImageFilter
import PIL
import numpy 
import time
import threading
import Queue
import serial
import sys
import rpSerial
import RPi.GPIO as GPIO
import glob
import picamera
from time import sleep

'''
G78 Senior Design Project:
    Raspberry Pi Code, image Processing
payloads for the raspberry pi:
    sudo apt-get install python-dev
    sudo easy_install -U distribute
    sudo apt-get install python-pip
    sudo apt-get update
    sudo apt-get install python-rpi.gpio python3-rpi.gpio
    sudo pip install serial
    X Ignore this, causes error, see below X: sudo pip install PIL

    sudo git init
    sudo git remote add origin https://github.com/CrakeNotSnowman/G78SeniorDesign
    sudo git pull origin master

    sudo usermod -a -G dialout pi

    sudo ./ImageProcessing.py > raspberryPiError.txt 2>&1
    Error removed:
	sudo pip uninstall PIL
    sudo pip install pillow

    sudo shutdown -h now

Possible filters
    im1 = imag.filter(ImageFilter.BLUR)
    im1 = imag.filter(ImageFilter.SMOOTH)
    im2 = imag.filter(ImageFilter.FIND_EDGES)
    im3 = im2.filter(ImageFilter.SMOOTH_MORE)
    im3 = im3.filter(ImageFilter.SHARPEN)
    im4 = im3.filter(ImageFilter.EDGE_ENHANCE)
resize options:
    	NEAREST
    	BILINEAR
    	BICUBIC
    	ANTIALIAS


Updates to team charter
  -more skill sets
  -roughly what you're responsible for
  -Product Spec
Time line
  -Crude timeline and details wanted (it's got a lot of duplicate data from 494
  -More rambling with little info
  -He'll use timeline to negoate 


2 weeks to get board back
Ground and power for each header?
He really doesn't check any of this. He has no idea what's going on with us, or any group
woo finally example electronic entry pdf

02:00:00:fe:03:
02:0b:00:1b:02:00:00:fe:03:
02:0b:00:1b:02:00:00:fe:03:
02:0b:00:1b:02:00:00:fe:03:
02:0b:00:1b:02:00:00:fe:03:
02:0b:00:1b:02:00:00:fe:03:02:0b:00:1b:02:00:00:fe:03

fe:03:
02:0b:00:1b:02:00:00:fe:03:
02:0b:00:1b:02:00:00:fe:03:
02:0b:00:1b:02:00:00:fe:03:
02:0b:00:1b:02:00:00:fe:03

pin setup on PI
1   2
3   4
5   6
7   8
9  10
11 12
13 14
15 16
17 18
19 20
21 22 
23 24
25 26


Pin Numbers	RPi.GPIO	Raspberry Pi Name	BCM2835		USED AS
P1_01		1		3V3	 
P1_02		2		5V0	 
P1_03		3		SDA0			GPIO0
P1_04		4		DNC	 
P1_05		5		SCL0			GPIO1
P1_06		6		GND	 				GND
P1_07		7		GPIO7			GPIO4
P1_08		8		TXD			GPIO14		TXD
P1_09		9		DNC	 
P1_10		10		RXD			GPIO15		RXD
P1_11		11		GPIO0			GPIO17	
P1_12		12		GPIO1			GPIO18
P1_13		13		GPIO2		 	GPIO21
P1_14		14		DNC	 
P1_15		15		GPIO3			GPIO22
P1_16		16		GPIO4			GPIO23
P1_17		17		DNC	 
P1_18		18		GPIO5			GPIO24
P1_19		19		SPI_MOSI		GPIO10
P1_20		20		DNC	 
P1_21		21		SPI_MISO		GPIO9
P1_22		22		GPIO6			GPIO25
P1_23		23		SPI_SCLK		GPIO11
P1_24		24		SPI_CE0_N		GPIO8
P1_25		25		DNC	 
P1_26		26		SPI_CE1_N		GPIO7


pin setup on PI
	1   2 
	3   4
	5   6  --GND
	7   8  --TXD->MSP:RXD
	9  10  --RXD<-MSP:TXD
	11 12  --Pic 
	13 14
	15 16
	17 18
	19 20
	21 22 
	23 24
	25 26
MSP->Pi
4d:03:02:3f:4d:03:02:06:0b:03:
02:4d:03:02:3f:4d:03:02:06:0b:03:
02:4d:03:02:3f:4d:03:

Pi->MSP
02:06:4d:03:02:0b:18:00:40:c6:e2:03:
02:06:4d:03:02:0b:18:00:00:c6:22:03:
02:06:4d:03:



'''

init	= 0x01
startX 	= 0x02
endX 	= 0x03
acknow 	= 0x06
burn 	= 0x0B
endIm	= 0x0F
startIm	= 0x11
esc 	= 0x1B
error	= 0x3f
readyB 	= 0x4d
emerg	= 0x0d


# this is used to make sure the threads stay active as long as 
#   there is still data to transmit
Qdone = False
Sdone = False

# Threading classes:
# Each thread has a few internal funcitons,
# the __init__, run, stop, and stopped
# init sets all needed varibles, and ensures
#   the run class has all needed attributes 
# run executes a function or series of functions
# stop kills the thread
# stopped is used to verify that the thread is done
#   Stop and Stopped are needed to allow ctrl+c to 
#   kill the program, making debugging simpler

class populateEDQueueThread(threading.Thread):
    # Handles the basic 'edge detection' algorithm
    def __init__(self, imagA, q, levels, pq):
	threading.Thread.__init__(self)
	self.name = "EDImagePop"
	self.daemon = True
	self.q = q
	self.imagA = imagA
	self.levels = levels
	self.pq = pq
    def run(self):
	self.pq.put(("M", "Starting To find pixles"))
	edQ(self.imagA, self.q, self.levels, self.pq)
    def stop(self):
	self._stop.set()
    def stopped(self):
	return self._stop.isSet()

class populateRasterQueueThread(threading.Thread):
    # Used in place of basic edge detect,
    #   This does raster image processing
    def __init__(self, imagA, q, levels, pq):
	threading.Thread.__init__(self)
	self.name = "RASTERImagePop"
	self.daemon = True
	self.q = q
	self.imagA = imagA
	self.levels = levels
	self.pq = pq
    def run(self):
	self.pq.put(("M", "Starting To find pixles"))
	rasterQ(self.imagA, self.q, self.levels, self.pq)
    def stop(self):
	self._stop.set()
    def stopped(self):
	return self._stop.isSet()

class dealWithImageThread(threading.Thread):
    # Used in place of basic edge detect,
    #   This does raster image processing
    def __init__(self, q, pq, mode, ser, size):
	threading.Thread.__init__(self)
	self.name = "ImageEverything"
	self.daemon = True
	self.q = q
	self.mode = mode
	self.pq = pq
	self.ser = ser
	self.size = size
    def run(self):
	self.pq.put(("M", "Image Processing Begining"))
	runImageSide(self.mode, self.q, self.pq, self.ser, self.size)
    def stop(self):
	self._stop.set()
    def stopped(self):
	return self._stop.isSet()

class serialManagerThread(threading.Thread):
    # Handles serial communication between Pi and MSP
    def __init__(self, q, ser, pq):
	threading.Thread.__init__(self)
	self.name = "Serial"
	self.daemon = True
	self.q = q
	self.ser = ser
	self.pq = pq
    def run(self):
	self.pq.put(("M", "Starting serial communication"))
	serialManager(self.q, self.ser, self.pq)
    def stop(self):
	self._stop.set()
    def stopped(self):
	return self._stop.isSet()

class printQueueThread(threading.Thread):
    # Exists to make print statements that don't get
    #   interrupted by multithreading
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

def runImageSide(mode, q, pq, ser, size):
    while True:
	# Take Picture
	#myImg = takePic()
	myImg = "vw.png"
	size = 190 #**********
	# Start Image command
	response = 2
	rpSerial.sendX(ser, chr(startX))
	rpSerial.sendX(ser, chr(startIm))
	rpSerial.sendX(ser, chr(endX))
	while response > 0:
	    rpSerial.sendX(ser, chr(startX))
	    rpSerial.sendX(ser, chr(startIm))
	    rpSerial.sendX(ser, chr(endX))
	    response = rpSerial.receiveX(ser, [chr(0x02), chr(0x06), chr(0x11), chr(0x03)])
	# Process Image and populate in serial Q
	if (mode == 1):
	    myA = edgeDetectImage(myImg, size)
	    thresholdLevs = getThresh(myA)
	    threadPop = edQ(myA, q, thresholdLevels, pq)
	elif (mode == 0):
	    myA = rasterImage(myImg, size)
	    thresholdLevs = getThresh(myA)
	    threadPop = rasterQ(myA, q, thresholdLevels, pq)
	    print "THREAD POP"
   	#time.sleep(.05)
	# Wait for all of the image to be done being processed
	while q.qsize() > 0:
	    time.sleep(1)
	    #print "HEYEYEYEYYEEY", q.qsize()
	time.sleep(1)
	# Send end of image command
	response = 2
	while response > 0:
	    rpSerial.sendX(ser, chr(startX))
	    rpSerial.sendX(ser, chr(endIm))
	    rpSerial.sendX(ser, chr(endX))
	    response = rpSerial.receiveX(ser, [chr(0x02), chr(0x06), chr(0x0F), chr(0x03)])
    return

def getLevel(pixel, levels):
    # takes a pixel input, and returns the laser intensity
    value = len(levels)
    for i in range(len(levels)):
	if (pixel < levels[i]):
	    value = i
	    break
    return value

def buildpayload(value, yLoc, xLoc, keepOnFlag):
    # value: 		2 bits
    # xLoc: 		13 bits
    # yLoc: 		13 bits
    # keepOnFlag:	1 bit
    # total:		29 bits, occupy 32

    # Startx: 0x02
    # Command: 0x0B
    # Payload 0xNNNNNNNN
    # Checksum 0xNN
    # EndX: 0x03
    '''
    # Recieve
Start : 0x02
Acknow	0x06
Command	0x0B
ETx	0x03


    # Ready for more
Start	0x02
SendM	0x1B
End	0x03

    # Pi will respond
	0x02
	0x1B
	0x3
    ''' 
    payload = 0x00000000
    valMask = 3
    xMask = 8191
    yMask = 8191
    onMask = 1
    #print yLoc
    # Clean off variables 
    value = value & valMask
    xLoc = xLoc & xMask
    yLoc = yLoc & yMask
    keepOnFlag = keepOnFlag & onMask
    #print format(value, '02x'), format(xLoc, '02x'), format(yLoc, '02x'), format(keepOnFlag, '02x')

    #Set payload
    payload = payload | value
    payload = payload << 13
    payload = payload | xLoc
    payload = payload << 13
    payload = payload | yLoc
    payload = payload << 1
    payload = payload | keepOnFlag    
    #print format(payload, '02x')
    return payload

def rasterMode(q, printq):
    # Go to take Pic fun, wait for Image to be armed
    # And for user to push button
    myImg = takePic()


def rasterQ(imagA, q, levels, printq):
    msg = ("M", "Running RASTER")
    printq.put(msg)
    xSize = len(imagA[0])
    ySize = len(imagA)
    leftToRight = True
    msg = ("M", (str(xSize) +" " + str(ySize)))
    printq.put(msg)
    skippedPix = 0

    # Burn four corners
    payload = buildpayload(3, 0, 0, False)
    q.put(payload)
    payload = buildpayload(3, xSize-1, 0, False)
    q.put(payload)
    payload = buildpayload(3, xSize-1, ySize-1, False)
    q.put(payload)
    payload = buildpayload(3, 0, ySize-1, False)
    q.put(payload)
    payload = buildpayload(3, 0, 0, False)
    q.put(payload)


    for i in range(xSize):
	for j in range(ySize):
	    if (leftToRight == True):
		pixel = imagA[j][i]
		value = getLevel(pixel, levels)
		xLoc = i
		yLoc = j
	    else:
		pixel = imagA[ySize - j - 1][i]
		value = getLevel(pixel, levels)
		xLoc = i
		yLoc = ySize - j - 1	
	    if (value > 0): #skip all blank areas 
		payload = buildpayload(value-1, xLoc, yLoc, False)
		q.put(payload)
		#print "\t[Queue]->", int(payload)
	    else: # FOR ERROR CHECK
		skippedPix += 1
	leftToRight = not leftToRight
    #print "Skippied Pix ", skippedPix
    msg = ("M", "Done Processing Image: Queue fully populated")
    printq.put(msg)
    Qdone = True
    while not q.empty():
	time.sleep(1)

    return

def edQ(imagA, q, levels, printq):
    print "Running EDGE DETECT"
    while not q.empty():
	time.sleep(1)
    while not Sdone:
	time.sleep(1)
    return


# *************************************************************************

# *************************************************************************

def serialManager(q, ser, printq):
    # Open Serial Communication
    # Send (0,0) coordinate, 
    #   Wait for confimation of recieve
    #   Wait for ready payload
    #   Start Pulling from Queue Loop
    #     ...
    #   Verify Queue is done (check to see that neither
    #		rasterQ nor edQ threads are running)
    # Close Serail Comm
    msg = ("M", "Starting Serial")
    printq.put(msg)
    pixCount = 0
    i = 10
    while True:
	if q.qsize() > 0:
	    print "CALLING SER MANAGER"
	    i = rpSerial.rpSerialManager(q, ser)
	else:
	    time.sleep(1)
	print i
    '''
    while not Qdone:
	check = rpSerial.rpSerialManager(q, ser)
	if check == 1:
	    print "HERE"
	    Sdone = True
	    q.queue.clear()
	    #sys.exit('s')
	    break
    print "Sdone Ln 306", Sdone
    if (Sdone != True):
	check = rpSerial.rpSerialManager(q, ser)
    Sdone = True
    print "Sdone Ln 310", Sdone
    '''
    return
def checkerboard():
    # This function is for milestone 2 in semester 2
    #   The function builds a checkerboard pattern based on
    #   'template' and packs in blocks of blockSize pixels
    #   and then returns it. The returned matrix mimics an image
    thresholdLevels = [51, 102, 153, 204, 255]
    template = [[1,0,2,0,3],\
		[0,2,0,3,0],\
		[2,0,3,0,4],\
		[0,3,0,4,0],\
		[3,0,4,0,1]]

    blockSize = 50 #pixels
    cb = []
    for j in range(len(template)*blockSize):
	tempx = []
	for i in range(len(template[0])*blockSize):
	    x = int(i/blockSize)
	    y = int(j/blockSize)
	    tempx.append(thresholdLevels[template[x][y]]-1)
	cb.append(tempx)

    #print cb

    return cb
   
def probBasedRaster(myImg, size):
    import random
    imag = Image.open(myImg)
    imag = imag.convert("L")
    # Crop Image here
    # box = (left, upper, right, lower)
    # imag = Image.crop(box)
    # reSize Image here to x and y
    baseWidth = size
    wpercent = baseWidth/float(imag.size[0])
    hSize = int((float(imag.size[1])*float(wpercent)))
    size = (baseWidth, hSize)
    imag = imag.resize(size, PIL.Image.ANTIALIAS)
    myA = numpy.array(imag)
    myA = 255 - myA
    # now scan image, each pixel level / 255 = prob it will be shown
    for i in range(myA):
	for j in range(myA[0]):
	    lev = myA[i][j]
	    probLev = random.randint(0, 255)
	    if probLev >= lev:
		myA[i][j] = 0 # After inverted, this will be black
	    else:
		myA[i][j] = 255 # after inv. this will not be drawn

    return myA

def rasterImage(myImg, size):
    imag = Image.open(myImg)
    imag = imag.convert("L")
    # Crop Image here
    # box = (left, upper, right, lower)
    # imag = Image.crop(box)
    # reSize Image here to x and y
    baseWidth = size
    wpercent = baseWidth/float(imag.size[0])
    hSize = int((float(imag.size[1])*float(wpercent)))
    size = (baseWidth, hSize)
    imag = imag.resize(size, PIL.Image.ANTIALIAS)
    myA = numpy.array(imag)
    myA = 255 - myA
    return myA

def getThresh(myImg):
    newArray = myImg
    hist, bins = numpy.histogram(newArray, 256, [0,256])

    mass = sum(hist)
    curMass = 0
    divisions = 5.001
    thresholdLevels = []
    for i in range(len(hist)):
	curMass += hist[i]
	if curMass >= mass/divisions :
	    mass = mass - curMass
	    curMass = 0
	    divisions = divisions - 1
	    thresholdLevels.append(i)
    while True:
	if len(thresholdLevels) < 4:
	    thresholdLevels.append(255)
        elif len(thresholdLevels) > 4:
	    thresholdLevels = thresholdLevels[0:3]
	else:
	    break
	    


    thresholdLevels = [90, 155, 196, 225]
    return thresholdLevels


def edgeDetectImage(myImg, size):
    imag = Image.open(myImg)
    imag = imag.filter(ImageFilter.FIND_EDGES)
    imag = imag.convert("L")

    # Crop Image here
    # box = (left, upper, right, lower)
    # imag = Image.crop(box)
    # reSize Image here to x and y
    baseWidth = size
    wpercent = baseWidth/float(imag.size[0])
    hSize = int((float(imag.size[1])*float(wpercent)))
    size = (baseWidth, hSize)
    imag = imag.resize(size, PIL.Image.ANTIALIAS)
    # **maybe resize before edge filtering ** NOPE
    #imag.show()
    myA = numpy.array(imag)
    return myA

def justPrintIt(q, mode):
    i = 3
    recentMessage = False
    while True:
	if not q.empty():
	    msg = q.get()
	    if ((msg[0] == 'E') or (msg[0] == 'M')):
		sys.stdout.write(str(msg[1]))
		sys.stdout.write("\n")
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
		recentMessage = True
	    q.task_done()
	    i = 0
	else:
	    time.sleep(0.01)
	    i += 1
	    if( (recentMessage == True) and (i > 3)):
		recentMessage = False
		i = 0
		sys.stdout.write("\n")
    while not q.empty():
	time.sleep(1)
    while not Sdone:
	time.sleep(1)
    return

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

    
# ***************** MAIN FLOW CONTROL ***************** #

def main():
    # Function defaults:
    raster = 0
    edgeDetect = 1
    q = Queue.Queue() # Pixel queue
    pq = Queue.Queue() #print queue

    # Runtime Settings:
    #   Set Size and basics
    size = 250 # pixels
    #thresholdLevels = [75, 110, 180, 225]
    #myImg = "template.png"
    mode = raster
    #   Set Serial Ports
    myBaud = 115200
    myTimeO = 0

    # Rasp Pi: /dev/ttyAMA0
    # Laptop: check ports: currently ACM0
    defPort = serial_ports()
    if (len(defPort) > 1):
	defPort = defPort[1]
    elif (len(defPort) > 0):
	defPort = defPort[0]
    else:
	defPort = "/dev/ttyACM0"
	print "Couldn't find a valid port"
    ser = serial.Serial(defPort, baudrate = myBaud, timeout = myTimeO)
    connected = False 
    print "Opening Serial Port Communication"
    while not connected:
	serin = ser.read()
	connected = True
    print "\tSerial Communication Open"
    # Now we wait until the micro is initalized:
    # this
    # THIS IS ONLY COMMENTED TO TEST, FIX FOR FULL COMM SUPPORT
    # myImg = takePic()


    response = 2
    time.sleep(1)
    print "waiting for Garin"
    while response > 0:
	rpSerial.sendX(ser, chr(startX))
	rpSerial.sendX(ser, chr(init))
	rpSerial.sendX(ser, chr(endX))
	response = rpSerial.receiveX(ser, [chr(0x02), chr(0x06), chr(0x01), chr(0x03)])
	
	time.sleep(0.001)

    # Get image from file or camera =>MOVE IN WITHIN IMAGING FUNCTIONS
    #myImg = takePic()
    


    # Start all threads: Printing thread, Serail com thread, and Edge or Raster Thread
    printThread = printQueueThread(pq, "h")
    threadSerial = serialManagerThread(q, ser, pq)
    imageThread =  dealWithImageThread(q, pq, mode, ser, size)
    # Get matrix for image
    '''
    if (mode == 1):
	#myA = edgeDetectImage(myImg, size)
	threadPop = populateEDQueueThread(myA, q, thresholdLevels, pq)
    elif (mode == 0):
	#myA = rasterImage(myImg, size)
	#thresholdLevs = getThresh(myA)
	threadPop = populateRasterQueueThread(myA, q, thresholdLevels, pq)
    '''
    try:
	imageThread.start()
	printThread.start()
	#threadPop.start()
	threadSerial.start()
    except(KeyboardInterrupt, SystemExit):
	print "Shutting Down"
	printThread.stop()
	imageThread.stop()
	threadSerial.stop()
	ser.close()

   # print "EHEHH"
    while not Sdone:
	time.sleep(1)
	#print "LN 498", Sdone
	# Turns out, when com fails, it sits here.
	#   Right here. FOREVER.
	# DON"T BOTHER PRINTING FROM HERE
	# ...Also, I'm hungry.\n import food

   # print "EHEHsdfH"
    #while not q.empty():
	#time.sleep(1)
	#pass
    print "EHEH245H"
    #time.sleep(10)
    while True:
	time.sleep(10)
    ser.close()


    return


# ************************** START PROG ******************* #

thresholdLevels = [75, 110, 180, 225]
thresholdLevels = [51, 102, 153, 204]
myImg = "template.png"

# Ok, so we're starting the package on the 'top' frame
#

def takePic():
    img = 'template.png'
    camera = picamera.PiCamera()
    camera.start_preview()
    button = 12
    #GPIO.setmode(GPIO.BOARD)
    #GPIO.setwarnings(False)
    #GPIO.setup(button, GPIO.IN)
    #buttonState = GPIO.input(button)
    #while True:
    #	if buttonState != GPIO.input(button):
    #	    break
    raw_input()
    camera.capture(img)
    camera.stop_preview()
    camera.close()
    return img
    
# raspi still -o "filename.jpg"
#img = takePic()
main()


'''
Burn Checker board
blocks of different intensities
system schem 

turn sys on
	msp locate self
	check power
	check laser
	safety is good, look at pi
pi starts
runs startup
waits for GPIO trigger for "Start"
Start Picture Take
	Take Pic
	Approve
Set the Levels
Approve
*******************
End User 
Start [already built prog]
End laser burn
Repeat .... all? or just take pic


make note of version of software
RSA Encryption

Spring Semester 2015
UNL Electrical and Computer Engeineering

'''






