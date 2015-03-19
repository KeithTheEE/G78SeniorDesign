//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : laser_driver.c
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com
// Date		   : 2015-02-05 (Created), 2015-03-08 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Source code to drive the analog input to the laser driver via
//				 PWM
//============================================================================


////////////////////////////////////////////////////////////////////////////////


#include "msp430f5529.h"
#include "defs.h"
#include "laser_driver.h"
#include "uart_fifo.h"
#include "time.h"
#include "motors.h"

////////////////////////////////////////////////////////////////////////////////


uint8_t laser_on = FALSE;
uint32_t intensity_buffer[100];
uint32_t x_pos_buffer[100];
<<<<<<< HEAD
uint32_t y_pos_buffer[750];
=======
uint32_t y_pos_buffer[100];
>>>>>>> origin/master

uint16_t buffer_it = 0;

volatile uint8_t burn_ready = 0;

////////////////////////////////////////////////////////////////////////////////


void init_laser( void )
{
	// Set up TimerA_0 for PWM on the laser input
	init_timer_A0();

	// Set Port 1.1 I/O to periphereal mode (i.e. Timer A0 output 1)
	P1DIR |= LASER_CTL_PIN;
	P1SEL |= LASER_CTL_PIN;


	// Set Port 1.2 I/O to GPIO mode (laser enable pin)
	P1OUT &= ~LASER_ENA_PIN;	// Laser disabled
	P1SEL &= ~LASER_ENA_PIN;	// Select I/O
	P1DIR |=  LASER_ENA_PIN;	// Select output


	return;
}
//============================================================================



void enable_laser( void )
{
	P1OUT = LASER_ENA_PIN;		// Laser enabled

	return;
}
//============================================================================



void disable_laser( void )
{
	P1OUT &= ~LASER_ENA_PIN;	// Laser disabled

	return;
}
//============================================================================



void turn_on_laser( uint16_t intensity )
{
	// Set Compare register 1 (Duty Cycle = TA0CCR1/TA0CCR0)
	TA0CCR1 = intensity;

	// Set output mode to 'Reset/Set' (OUTMOD = 111b)
	TA0CCTL1 |= OUTMOD2;
	TA0CCTL1 |= OUTMOD1;
	TA0CCTL1 |= OUTMOD0;
	
	laser_on = TRUE;

	return;
}
//============================================================================



void turn_on_laser_timed( uint16_t intensity, uint16_t duration )
{
	turn_on_laser( intensity );

	delay_ms( duration );

	turn_off_laser( );
	
	return;
}
//============================================================================



void turn_off_laser( void )
{
	// Set Compare register 1 to 0 (turns off laser)
	TA0CCR1 = 0;

	// Set output mode to 'OUT bit value' (OUTMOD = 000b)
	// Since the OUT bit value is always 0, this will turn it off
	TA0CCTL1 &= ~OUTMOD2;
	TA0CCTL1 &= ~OUTMOD1;
	TA0CCTL1 &= ~OUTMOD0;

	laser_on = FALSE;

	return;
}
//============================================================================



void respond_to_burn_cmd( uint8_t * burn_cmd_payload )
{
	// Perform a burn (move laser to position, turn on laser)
	uint32_t y_pos;
	uint32_t x_pos;
	uint32_t laser_intensity;
	volatile uint16_t temp0 = burn_cmd_payload[0];
	volatile uint16_t temp1 = burn_cmd_payload[1];
	volatile uint16_t temp2 = burn_cmd_payload[2];
	volatile uint16_t temp3 = burn_cmd_payload[3];



	parse_burn_cmd_payload( burn_cmd_payload,
							&y_pos,
							&x_pos,
							&laser_intensity );

<<<<<<< HEAD
	if( buffer_it < 750  )
	{
		//intensity_buffer[buffer_it] = laser_intensity;
		//x_pos_buffer[buffer_it] = x_pos;
		y_pos_buffer[buffer_it] = y_pos;
		buffer_it++;
	}
	//buffer_it++;
=======
	if( buffer_it < 100 )
	{
		intensity_buffer[buffer_it] = laser_intensity;
		x_pos_buffer[buffer_it] = x_pos;
		y_pos_buffer[buffer_it] = y_pos;
		buffer_it++;
	}
>>>>>>> origin/master

	// Move Laser
	if( moveMotors( x_pos, y_pos ) == 1 )
	{
		halt_burn();
	}

	
	//uint8_t laser_intensity = ( burn_cmd_payload[0] & LASER_INTENSITY_MASK ) >> LASER_INTENSITY_SHIFT;

<<<<<<< HEAD
	// delay_ms( 300 );
=======
	delay_ms( 300 );
>>>>>>> origin/master

	switch( laser_intensity )
	{
		case 0:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_1 );
				 break;

		case 1:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_2 );
				 break;

		case 2:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_3 );
				 break;

		case 3:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_4 );
				 break;

		/*case 0:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_4 );
			  	 break;

		case 1:  turn_on_laser_timed( INTENSITY_3, LASER_DUR_4 );
				 break;

		case 2:  turn_on_laser_timed( INTENSITY_2, LASER_DUR_4 );
				 break;

		case 3:  turn_on_laser_timed( INTENSITY_1, LASER_DUR_4 );
				 break;*/

		default: break;
	}


	// Reset the tracking variable
	burn_ready = 0;

	// When done executing the burn, request another command
	send_ready();

	return;
}
//============================================================================



void halt_burn( void )
{
	// First disable the laser and make sure the PWM input is off
	disable_laser();
	turn_off_laser();
	
	// Tell the Pi the burn is ending
	send_burn_stop();
	
	return;
}

////////////////////////////////////////////////////////////////////////////////


