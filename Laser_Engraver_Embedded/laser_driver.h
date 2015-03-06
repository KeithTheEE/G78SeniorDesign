//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : laser_driver.c
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com
// Version     : See "Revision History" below
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Header file including function prototypes used in driving
//				 the laser driver's analog input
//============================================================================
//
//  Revision History
//      v0.0.0 - 2015/02/05 - Garin Newcomb
//          Initial creation of file
//
//    	Appl Version at Last File Update::  v0.0.x - 2015/02/05 - Garin Newcomb
//      	[Note:  until program released, all files tracking with program revision level -- see "version.h" file]
//
//==============================================================================


#ifndef LASER_DRIVER_H_
#define LASER_DRIVER_H_

#include <stdint.h>
#include "msp430f5529.h"


#define LASER_CTL_PIN			BIT2
#define LASER_ENA_PIN			BIT3

#define LASER_INTENSITY_MASK	0x18
#define LASER_INTENSITY_SHIFT	3

#define LASER_DUR_1				5
#define LASER_DUR_2				10
#define LASER_DUR_3				15
#define LASER_DUR_4				20

#define MAX_INTENSITY 			12300	// 100%
#define INTENSITY_3 			9225	// 75%
#define INTENSITY_2 			6150	// 50%
#define INTENSITY_1 			3075	// 25%
#define FOCUS_INTENSITY 		1230	// 10%


void timer_A0_setup( void );
void initialize_laser( void );
void enable_laser( void );
void disable_laser( void );
void turn_on_laser( uint16_t intensity );
void turn_on_laser_timed( uint16_t intensity, uint16_t duration );
void turn_off_laser( void );
void delay( uint32_t time_ms );


#endif /* LASER_DRIVER_H_ */
