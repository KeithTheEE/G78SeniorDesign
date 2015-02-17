#!/usr/bin/env python

'''
Keith Murray
This code is a test of using the raspicam in python

'''
import picamera
from time import sleep

def takePic():
    img = 'template.png'
    camera = picamera.PiCamera()
    camera.start_preview()
    sleep(3)
    camera.stop_preview()
    camera.capture(img)
    return img
    



def main():
    fileName = takePic()

main()
