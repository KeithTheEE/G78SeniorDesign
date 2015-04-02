//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : debug.c
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com
// Date		   : 2014-03-08 (Created), 2015-04-01 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Source code for functionality used for debugging (via the 
//				 MSP430 launchpad)
//============================================================================


////////////////////////////////////////////////////////////////////////////////

#include "msp430f5529.h"
#include "defs.h"
#include "debug.h"


////////////////////////////////////////////////////////////////////////////////


void init_debug_LED( void )
{
	P1SEL &= ~DEBUG_LED;				// Select I/O
	P1DIR |=  DEBUG_LED; 				// Set P1.0 to output
	P1OUT &= ~DEBUG_LED; 				// LED off

	return;
}
//============================================================================



void init_pcb_LED( void )
{
	P3DIR &= ~PCB_LED; 					// LED off
	P3DIR &= ~PCB_LED;					// Select I/O
	P3DIR |=  PCB_LED; 					// Set P1.0 to output

	return;
}
//============================================================================



void init_button_inputs( void )
{
	P1SEL &= ~BUTTON1;					// Select I/O
	P1DIR &= ~BUTTON1; 					// Set P1.1 button to input

	P2SEL &= ~BUTTON2;					// Select I/O
	P2DIR &= ~BUTTON2; 					// Set P1.2 button to input
	
	return;
}
//============================================================================



void init_pcb_input( void )
{
	P6SEL &= ~BIT7;						// Select I/O
	P6DIR &= ~BIT7; 					// Set P1.1 button to input

	return;
}
//============================================================================

////////////////////////////////////////////////////////////////////////////////


