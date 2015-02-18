#/*****************************************************************/
#/*  University of Nebraska-Lincoln                               */
#/*  Department of Electrical Engineering                         */
#/*  G78 Senior Design Project                   		  */
#/*  			and				          */
#/*  Keith "Has joined the chaos" Murray			  */
#/*		     featuring					  */
#/*  		     Magic Code 				  */	
#/*****************************************************************/


all:
	sudo apt-get update
	sudo apt-get install python-dev
	sudo apt-get install python-pip
	sudo apt-get install python-serial
	sudo apt-get install python-rpi.gpio python3-rpi.gpio
	sudo apt-get install picamera
	sudo pip install pillow
	sudo reboot


clean:
	sudo apt-get remove python-rpi.gpio python3-rpi.gpio
	sudo pip uninstall serial
	sudo apt-get remove picamera
	sudo pip uninstall pillow
