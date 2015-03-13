//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : motors.h
// Author      : Fernando Urias-Cordero
// Email       : furias.cordero@gmail.com
// Date		   : 2015-01-26 (Created), 2015-03-09 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Header file including function prototypes used in driving
//				 the laser driver's analog input
//============================================================================


#ifndef MOTORS_H_
#define MOTORS_H_


////////////////////////////////////////////////////////////////////////////////


/*initMotorIO

* sets up P1.4 as high to low button ISR
P4.7 is an output (on launchpad this controls an LED)

* INPUT: None

* RETURN: None

*/
void initMotorIO(void);


/*moveMotors

* moves x-axis motor then moves y-axis motors

*Need to add y-axis logic. simply copy x axis but with y coordinate

* INPUT: Xnew and Ynew both unsigned ints

* RETURN: None

*/

void moveMotors(unsigned int Xnew, unsigned int Ynew);


/*homeLaser

* moves laser to the home position 
* uses launchpad pins need to change to Port 7

* INPUT: none

* RETURN: None

*/
void homeLaser(void);

////////////////////////////////////////////////////////////////////////////////


#endif // MOTORS_H_




