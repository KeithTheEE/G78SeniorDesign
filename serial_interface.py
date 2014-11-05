#!/usr/bin/env python

import termios, sys, fcntl, struct, os

class serialInterface:

	def __init__(self, serialFD, serialDevice, serialBaudrate):
		
		# Open device
		serialFD = os.open(serialDevice, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
		if(serialFD < 0):
			
			print('Error opening serial device')
		
		# Clear buffers
		termios.tcflush(serialFD, termios.TCIOFLUSH)
		
		# Switch to blocking mode
		fcntl.fcntl(serialFD, fcntl.F_SETFL, fcntl.fcntl(serialFD, fcntl.F_GETFL) & ~os.O_NONBLOCK)

		# Set device parameters
		serialParams = termios.tcgetattr(serialFD)
		if(serialParams):
			print('Error getting serial_Device parameters.')
		
		# TODO: Set c_cflag
		print(serialParams[6])

		termios.tcflush(serialFD, TCIFLUSH)
		if(termios.tcsetattr(serialFD, termios.TCSANOW, serialParams)):
		
			print('Error setting serial device parameters.')
			return -1
		return 0

	def sendSerial(self, serialFD, dataArray, dataSize):
		# Write data to serial device
		sentBytes = os.write(serialFD, dataArray)
		if(sentBytes < 0):
			print('Error writing serial buffer')
			return -1
		termios.tcflsuh(serialFD, TCOFLUSH)
		return sentBytes
	
	def readSerial(serialFD, dataArray, dataSize):
		receivedBytes = os.read(serialFD, sataSize)
		if(receivedBytes < 0):
			print('Error reading serial buffer')
			return -1
		termios.tcflush(serialFD, TCIFLUSH)
		return receivedBytes

# Test main
def main():
	print 'hello'
	serialInt = serialInterface(0,"/dev/tty1",115200)

main()
