#!/usr/bin/env python
from PIL import Image, ImageFilter
import PIL
import numpy 
import time
import threading
import Queue

'''

Commands for the raspberry pi:
    sudo apt-get install python-dev
    sudo easy_install -U distribute
    sudo apt-get install python-pip
    sudo pip install rpi.gpio
    X Ignore this, causes error, see below X: sudo pip install PIL

    sudo git init
    sudo git remote add origin https://github.com/CrakeNotSnowman/G78SeniorDesign
    sudo git pull origin master

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


'''
class populateEDQueueThread(threading.Thread):
    def __init__(self, imagA, q, levels):
	threading.Thread.__init__(self)
	self.name = "EDImagePop"
	#self.daemon = True
	self.q = q
	self.imagA = imagA
	self.levels = levels
    def run(self):
	print "Starting To find pixles"
	edQ(self.imagA, self.q, self.levels)

class populateRasterQueueThread(threading.Thread):
    def __init__(self, imagA, q, levels):
	threading.Thread.__init__(self)
	self.name = "RASTERImagePop"
	#self.daemon = True
	self.q = q
	self.imagA = imagA
	self.levels = levels
    def run(self):
	print "Starting To find pixles"
	rasterQ(self.imagA, self.q, self.levels)

class serialManagerThread(threading.Thread):
    def __init__(self, q):
	threading.Thread.__init__(self)
	self.name = "Serial"
	#self.daemon = True
	self.q = q
    def run(self):
	print "Starting serial communication"
	serialManager(self.q)

def getLevel(pixel, levels):
    value = len(levels)
    for i in range(len(levels)):
	if (pixel < levels[i]):
	    value = i
	    break
    return value

def buildCommand(value, xLoc, yLoc, keepOnFlag):
    # value: 		2 bits
    # xLoc: 		13 bits
    # yLoc: 		13 bits
    # keepOnFlag:	1 bit
    # total:		29 bits, occupy 32
    command = 0x000000
    valMask = 3
    xMask = 8191
    yMask = 8191
    onMask = 1

    # Clean off variables 
    value = value & valMask
    xLoc = xLoc & xMask
    yLoc = yLoc & yMask
    keepOnFlag = keepOnFlag & onMask

    #Set command
    command = command | value
    command = command << 13
    command = command | xLoc
    command = command << 13
    command = command | yLoc
    command = command << 1
    command = command | keepOnFlag    
    #print int(command)
    return command

def rasterQ(imagA, q, levels):
    print "Temp, rasterQ"
    xSize = len(imagA[0])
    ySize = len(imagA)
    leftToRight = True
    print xSize," ", ySize
    skippedPix = 0
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
		command = buildCommand(value-1, xLoc, yLoc, False)
		q.put(command)
	    else: # FOR ERROR CHECK
		skippedPix += 1
	leftToRight = not leftToRight
    print "Skippied Pix ", skippedPix
    return

def edQ(imagA, q, levels):
    print "Temp, edQ"
    return

def serialManager(q):
    # Open Serial Communication
    # Send (0,0) coordinate, 
    #   Wait for confimation of recieve
    #   Wait for ready command
    #   Start Pulling from Queue Loop
    #     ...
    #   Verify Queue is done (check to see that neither
    #		rasterQ nor edQ threads are running)
    # Close Serail Comm
    print "Starting Serial"
    pixCount = 0
    i = 0
    while (i < 5):
        time.sleep(1)
	i += 1
	while not q.empty():
	    command = q.get()
	    #print int(command)
	    pixCount += 1
	    time.sleep(.0001)
	    q.task_done()
	    i = 0
    print "DONE ", pixCount," ", pixCount/1200

   

def rasterImage(myImg, size):
    imag = Image.open(myImg)
    imag = imag.convert("L")
    # Crop Image here
    # box = (left, upper, right, lower)
    # imag = Image.crop(box)
    # reSize Image here to x and y
    baseWidth = 1200
    wpercent = baseWidth/float(imag.size[0])
    hSize = int((float(imag.size[1])*float(wpercent)))
    size = (baseWidth, hSize)
    imag = imag.resize(size, PIL.Image.ANTIALIAS)
    # **maybe resize before edge filtering **
    #imag.show()
    myA = numpy.array(imag)
    return myA


def edgeDetectImage(myImg, size):
    imag = Image.open(myImg)
    imag = imag.filter(ImageFilter.EDGE_ENHANCE)
    imag = imag.filter(ImageFilter.FIND_EDGES)
    imag = imag.convert("L")

    # Crop Image here
    # box = (left, upper, right, lower)
    # imag = Image.crop(box)
    # reSize Image here to x and y
    baseWidth = 1200
    wpercent = baseWidth/float(imag.size[0])
    hSize = int((float(imag.size[1])*float(wpercent)))
    size = (baseWidth, hSize)
    imag = imag.resize(size, PIL.Image.ANTIALIAS)
    # **maybe resize before edge filtering **
    imag.show()


    myA = numpy.array(imag)
    xSize = len(myA[0])
    ySize = len(myA)
    
    #imag = Image.fromarray(myA)
    #imag.save(str(folder)+"Filtered"+str(level)+".png")
    return myA
    
def main():
    raster = 0
    edgeDetect = 1
    mode = raster
    # Set Thresholds 
    thresholdLevels = [75, 110, 180, 225]
    # Set Size
    size = (128, 128)
    # Set Queue
    q = Queue.Queue()


    # Get image from file or camera 
    myImg = "faceAndSuit.png"
    myImg = "pumpkins01.png"

    # Get matrix for image
    if (mode == 1):
	myA = edgeDetectImage(myImg, size)
	threadPop = populateEDQueueThread(myA, q, thresholdLevels)
	threadSerial = serialManagerThread(q)
	threadPop.start()
	threadSerial.start()
    elif (mode == 0):
	myA = rasterImage(myImg, size)
	threadPop = populateRasterQueueThread(myA, q, thresholdLevels)
	threadSerial = serialManagerThread(q)
	threadPop.start()
	threadSerial.start()


    # begin image processing & serial

    while not q.empty():
	pass
    time.sleep(10)

    
    





main()









