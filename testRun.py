#!/usr/bin/env python
from PIL import Image, ImageFilter
import PIL
import numpy 
import glob
import os, sys
#import subprocess
import Tkinter
import Image, ImageTk
from Tkinter import *
from PIL import ImageTk, Image
import RPi.GPIO as GPIO
import time

from matplotlib import pyplot as plt


#root = Tk()
#img = ImageTk.PhotoImage(Image.open("tswift.png"))
#panel = la

def getLevel(pixel, levels):
    # takes a pixel input, and returns the laser intensity
    value = len(levels)
    for i in range(len(levels)):
	if (pixel < levels[i]):
	    value = i
	    break
    return value


def rasterImage(myImg, size):
    imag = Image.open(myImg)
    imag = imag.convert("L")
    #imag = imag.filter(ImageFilter.FIND_EDGES)
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
    for i in range(len(myA)):
	for j in range(len(myA[0])):
	    lev = myA[i][j]
	    probLev = random.randint(0, 255)
	    if probLev+50 >= lev:
		myA[i][j] = myA[i][j] # After inverted, this will be black
	    else:
		myA[i][j] = 255 # after inv. this will not be drawn

    return myA

def getThresh(myImg):
    #imag = Image.open(myImg)
    #imag = imag.convert("L")
    newArray = myImg#numpy.array(imag)
    hist, bins = numpy.histogram(newArray, 256, [0,256])
    plt.hist(hist, bins)
    plt.show()
    print len(hist)
    print len(bins)

    mass = sum(hist)
    curMass = 0
    divisions = 4.9
    thresholdLevels = []
    for i in range(len(hist)):
	curMass += hist[i]
	if curMass >= mass/divisions :
	    curMass = 0
	    thresholdLevels.append(i)
    while True:
	if len(thresholdLevels) < 4:
	    thresholdLevels.append(255)
        elif len(thresholdLevels) > 4:
	    thresholdLevels = thresholdLevels[0:3]
	else:
	    break
    print thresholdLevels
	    

    return thresholdLevels

def main():
    myImg = 'rosenblatt.png'
    thresholdLevels = [75, 90, 200, 225]
    burnThresh = [0, 100, 160, 200, 230]
    size = 200
    modelSize = 4.9/2.0
    scaleup = int(size*modelSize)
	


    newArray = rasterImage(myImg, size)
    thresholdLevs = getThresh(newArray)
    #thresholdLevels = [30, 67, 103, 200]


        # now scan image, each pixel level / 255 = prob it will be shown
    for i in range(len(newArray)):
	for j in range(len(newArray[0])):
	    pix = newArray[i][j]
	    lev = getLevel(pix, thresholdLevels)
	    newArray[i][j] = burnThresh[lev]
    newArray = 255- newArray
    imag = Image.fromarray(newArray)
    baseWidth = scaleup
    wpercent = baseWidth/float(imag.size[0])
    hSize = int((float(imag.size[1])*float(wpercent)))
    sizeT = (baseWidth, hSize)
    imag = imag.resize(sizeT, PIL.Image.ANTIALIAS)
    imag.show()
    return
def testGPIO():
    led = 11
    button = 12
    GPIO.setup(led, GPIO.OUT)
    GPIO.setup(button, GPIO.IN)
    GPIO.output(led, False)
    buttonState = GPIO.input(button)
    while True:
	if buttonState != GPIO.input(button):
	    break
    GPIO.output(led, True)
    time.sleep(3)
    return
#main()


testGPIO()




