#!/usr/bin/env python
from PIL import Image, ImageFilter
import PIL
import numpy 
import time

'''
sudo apt-get install python-dev
sudo easy_install -U distribute
sudo apt-get install python-pip
sudo pip install rpi.gpio
sudo pip install PIL
'''

   
def somethingDifferent(myImg, outputfolder, level):
    imag = Image.open(myImg)
    imag = imag.convert("L")
    #im1 = imag.filter(ImageFilter.BLUR)
    #im1 = imag.filter(ImageFilter.SMOOTH)
    #im1.show()
    #im2 = imag.filter(ImageFilter.FIND_EDGES)
    #im2.show()
    #im3 = im2.filter(ImageFilter.SMOOTH_MORE)
    #im3 = im3.filter(ImageFilter.BLUR)
    #im3 = im3.filter(ImageFilter.SHARPEN)
    #im3.show()
    #im4 = im3.filter(ImageFilter.EDGE_ENHANCE)
    #im4.show()
    #imag = imag.filter(ImageFilter.FIND_EDGES)
    myA = numpy.array(imag)
    myA = myA / int(100)
    myA = myA * 100
    imag = Image.fromarray(myA)
    imag = imag.filter(ImageFilter.FIND_EDGES)
    imag.show()
    
    '''
    myA = imag.convert("L")
    myA = numpy.array(myA)
    myA = myA / int(level)
    myA = myA * level
    myA = 255 - myA
    xSize = len(myA[0])
    ySize = len(myA)
    print "Level: ", level, "nonZeros: ", numpy.count_nonzero(myA), " of ", len(myA)*len(myA[0])

    intFile = open(str(folder)+"intDump"+str(level)+".txt", "w")
    for i in range(ySize):
	quickString = ""
	for j in range(xSize):
	    quickString = quickString + str(myA[i][j]) + "\t"
	intFile.write(quickString)
	intFile.write("\n")
    imag = Image.fromarray(myA)
    imag.save(str(folder)+"Filtered"+str(level)+".png")
    '''
    return
    
def main():
    myImg = "faceAndSuit.png"
    thresholdLevels = [75, 110, 180, 225, 255]
    outputFolder = "faceAndSuit/"
    somethingDifferent(myImg, outputFolder, thresholdLevels)
    





main()









