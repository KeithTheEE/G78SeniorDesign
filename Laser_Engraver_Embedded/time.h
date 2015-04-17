//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : time.h
// Author      : Garin Newcomb and Tyler Troyer
// Email       : gpnewcomb@live.com and troyerta@gmail.com
// Date		   : 2015-03-08 (Created), 2015-04-08 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Header file including function prototypes and macros used for
//				 to setup and control clocks, timers, etc.
//============================================================================


#ifndef TIME_H_
#define TIME_H_


////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////


void init_clocks( void );
void init_timer_A0( void );
void delay_ms( uint32_t time_ms );


/*initWaitTimer

* sets up timer b for use as a delay function to count microseconds

* INPUT: None

* RETURN: None

*/
void initWaitTimer(void);


/*delay_10us

* delays for a given number of microseconds

* need to test with uart code!!!!!

* INPUT: unsigned int that holds time to wait in us

* RETURN: None

*/
void delay_10us( uint32_t time_10us );

////////////////////////////////////////////////////////////////////////////////


#endif // TIME_H_ 

