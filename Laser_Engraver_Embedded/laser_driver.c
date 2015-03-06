//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : laser_driver.c
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com
// Version     : See "Revision History" below
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Source code to drive the analog input to the laser driver via
//				 PWM
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


#include "laser_driver.h"
#include "defs.h"
#include "uart_fifo.h"


uint8_t laser_on = FALSE;
uint32_t time_ms = 0;

void timer_A0_setup( void )
{
	// Turn OFF capture mode (CM = 00b)
	TA0CCTL1 &= ~CM1;
	TA0CCTL1 &= ~CM0;

	// Turn ON compare mode (CAP = 0b)
	TA0CCTL1 &= ~CAP;

	// Set output OFF (OUT = 0b)
	TA0CCTL1 &= ~OUT;

	// Set output mode to 'OUT bit value' (OUTMOD = 000b)
	TA0CCTL1 &= ~OUTMOD2;
	TA0CCTL1 &= ~OUTMOD1;
	TA0CCTL1 &= ~OUTMOD0;


	// Reset timer
	TA0CTL |=  TACLR;

	// Set the timer clock source to 'SMCLK' (TASSEL = 10b)
	TA0CTL |=  TASSEL1;
	TA0CTL &= ~TASSEL0;

	// Set the timer clock input divider to 1 (ID = 00b)
	TA0CTL &= ~ID1;
	TA0CTL &= ~ID0;

	// Set the timer to 'up' mode (MC = 01b)
	TA0CTL &= ~MC1;
	TA0CTL |=  MC0;


	// Enable timer interrupts
	TA0CTL &= ~TAIFG;
	TA0CTL |=  TAIE;


	// Set Compare register (12300 yields an interrupt about every 1 ms)
	 TA0CCR0 = MAX_INTENSITY;

	__enable_interrupt();				//Interrupts Enabled


	return;
}


void initialize_laser( void )
{
	// Set up TimerA_0 for PWM on the laser input
	timer_A0_setup();

	// Set Port 1.1 I/O to periphereal mode (i.e. Timer A0 output 1)
	P1DIR |= LASER_CTL_PIN;
	P1SEL |= LASER_CTL_PIN;


	// Set Port 1.2 I/O to GPIO mode (laser enable pin)
	P1OUT &= ~LASER_ENA_PIN;	// Laser disabled
	P1SEL &= ~LASER_ENA_PIN;	// Select I/O
	P1DIR |=  LASER_ENA_PIN;	// Select output


	return;
}


void enable_laser( void )
{
	P1OUT = LASER_ENA_PIN;		// Laser enabled

	return;
}


void disable_laser( void )
{
	P1OUT &= ~LASER_ENA_PIN;	// Laser disabled

	return;
}


void turn_on_laser( uint16_t intensity )
{
	// Set Compare register 1 (Duty Cycle = TA0CCR1/TA0CCR0)
	TA0CCR1 = intensity;

	// Set output mode to 'Reset/Set' (OUTMOD = 111b)
	TA0CCTL1 |=  OUTMOD2;
	TA0CCTL1 |=  OUTMOD1;
	TA0CCTL1 |=  OUTMOD0;

	return;
}



void turn_on_laser_timed( uint16_t intensity, uint16_t duration )
{
	turn_on_laser( intensity );

	delay( duration );

	turn_off_laser( );

	laser_on = TRUE;

	return;
}


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


void delay( uint32_t delay_ms )
{
	uint32_t initial_count = time_ms;
	uint32_t final_count = initial_count + delay_ms;

	// Wait for overflow if necessary
	if( final_count < initial_count )
	{
		while( time_ms != 0 );
	}

	while( time_ms < final_count );


	return;
}


#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMERA0_ISR(void)
{
	switch( __even_in_range( TA0IV, 14 ) )
	{
		case TA0IV_TAIFG: time_ms++;
				 	 	  break;
		default: 		  break;
	}
}

