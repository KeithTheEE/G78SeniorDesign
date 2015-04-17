//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : time.c
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com
// Date		   : 2014-03-08 (Created), 2015-04-08 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Source code for setting up clocks, and timers, as well as any
//				 other time-related functionality
//============================================================================


////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "msp430f5529.h"
#include "defs.h"
#include "time.h"

////////////////////////////////////////////////////////////////////////////////


volatile uint32_t time_ms   = 0;
volatile uint32_t time_10us = 0;
volatile uint32_t count = 0;

volatile int timer_flag = FALSE;
volatile uint16_t t=0; //can count 65535 us
volatile int timer=0;

////////////////////////////////////////////////////////////////////////////////


// Set SMCLK to ~12.3 MHz, which is used for most peripherals
void init_clocks( void )
{
	// Setup DCO (will be used as source to SMCLK, which in turn will be used as source to BRCLK)
	WDTCTL   = WDTPW+WDTHOLD;               // Stop WDT
	UCSCTL3 |= SELREF_2;                    // Set DCO FLL reference = REFO
	UCSCTL4 |= SELA_2;                      // Set ACLK = REFO

	__bis_SR_register(SCG0);                // Disable the FLL control loop

	UCSCTL0  = 0x0000;                      // Set lowest possible DCOx, MODx
	UCSCTL1  = DCORSEL_5;                   // Select DCO range 24MHz operation
	UCSCTL2  = FLLD_1 + 374;                // Set DCO Multiplier for 12MHz
											// (N + 1) * FLLRef = Fdco
											// (374 + 1) * 32768 = 12.28MHz
											// Set FLL Div = fDCOCLK/2

	UCSCTL4  |= (0x03 << 4);				// Set SMCLK to DCO (12.28 MHz)
	// UCSCTL6 &= ~BIT8;					// SMCLK - 4 MHz (unresolved problem: XT2 seems to be running at 1 MHz rather than 4 MHz)

	__bic_SR_register(SCG0);                // Enable the FLL control loop

	return;
}
//============================================================================


void init_timer_A0( void )
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
//============================================================================



/*void delay_ms( uint32_t delay_ms )
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
}*/

void delay_ms( uint32_t delay_ms )
{
	uint32_t i;

	for( i = 0; i < delay_ms; i++ )
	{
		delay_10us( 100 );
	}

	return;
}
//============================================================================



void initWaitTimer(void)
{
  TA1CTL = TASSEL_2 + MC_1+TACLR;
  TA1CCR0 =  123; //12.28 MHz/12 ~ 1us

  TA1CCTL0 &= ~CCIE; // disable timer

  return;
}
//============================================================================



void delay_10us( uint32_t time_10us )
{
	TA1R = 60;
	timer_flag = FALSE;
	TA1CCR0 =  123 * time_10us;
	TA1CCTL0 |= CCIE; // enable timer

	while( timer_flag == FALSE );

	TA1CCTL0 &= ~CCIE; // disable timer

	return;
}
//============================================================================

////////////////////////////////////////////////////////////////////////////////


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
//============================================================================



#pragma vector = TIMER1_A0_VECTOR  //CCR0 vector for timerA1
__interrupt void TIMERA1_ISR()
{
	//time_10us += 10;
	timer_flag = TRUE;
}
//============================================================================

////////////////////////////////////////////////////////////////////////////////


