#!/usr/bin/env python



import RPi.GPIO as GPIO
import time


def main():
    rx = 15
    tx = 14
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    GPIO.setup(rx, GPIO.OUT)
    GPIO.setup(tx, GPIO.OUT)
    while True: 
	GPIO.output(rx, False)
	GPIO.output(tx, False)
	time.sleep(0.001)
	GPIO.output(rx, True)
	GPIO.output(tx, True)
	time.sleep(0.001)
    return

main()
