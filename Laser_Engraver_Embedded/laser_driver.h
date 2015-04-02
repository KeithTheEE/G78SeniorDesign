//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : laser_driver.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com
// Date		   : 2015-02-05 (Created), 2015-03-18 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Header file including function prototypes used in driving
//				 the laser driver's analog input
//============================================================================


#ifndef LASER_DRIVER_H_
#define LASER_DRIVER_H_


////////////////////////////////////////////////////////////////////////////////


#include <stdint.h>

#include "msp430f5529.h"

////////////////////////////////////////////////////////////////////////////////


void init_laser( void );
void enable_laser( void );
void disable_laser( void );
void turn_on_laser( uint16_t intensity );
void turn_on_laser_timed( uint16_t intensity, uint16_t duration );
void turn_off_laser( void );

void respond_to_burn_cmd( uint8_t * burn_cmd_payload );
void halt_burn( void );

////////////////////////////////////////////////////////////////////////////////


#endif // LASER_DRIVER_H_
