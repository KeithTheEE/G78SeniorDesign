//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : motors.c
// Author      : Fernando Urias-Cordero
// Email       : furias.cordero@gmail.com
// Date		   : 2015-01-26 (Created), 2015-04-01 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Source code to drive the on-board DRV8825PWPR stepper motor
//				 drivers
//============================================================================


////////////////////////////////////////////////////////////////////////////////


#include <stdint.h>

#include "msp430f5529.h"
#include "defs.h"
#include "motors.h"
#include "time.h"
#include "laser_driver.h"

////////////////////////////////////////////////////////////////////////////////


volatile double X = 0;  		//Current X pixel assuming
volatile double Y = 0;  		//Current Y pixel assuming
volatile double ticksX = 0; 	//global ticks
volatile double ticksY = 0; 	//global ticks
volatile int homeX = 1; 		//flag for homing x
volatile int homeY = 1; 		//flag for homing y
volatile int lid = 1; 			//flag for lid
volatile int skipStep = 1; 		//flag for homing y

uint32_t accel_delay[ACCEL_SIZE];

extern volatile uint8_t picture_ip;


////////////////////////////////////////////////////////////////////////////////


void initMotorIO(void)
{
	uint16_t i;

	// enable/reset drivers

	#ifdef DEBUG
		P6DIR |= BIT0; //P6.0 reset drivers - make output
		P6DIR |= BIT1; //P6.1 enable drivers - make output
	#else
		P4DIR |= BIT6; //P4.6 reset drivers - make output
		P7DIR |= BIT6; //P7.6 enable drivers - make output
	#endif


	#ifdef DEBUG
		// X-axis
		P4DIR |= BIT3; //P4.3 x step - make output
		P4SEL |= BIT3;
		P4DIR |= BIT0; //P4.0 x step direction - make output

		// Y-axis
		P3DIR |= BIT7; //P3.3 y step - make output
		P8DIR |= BIT2; //P8.2 y step direction - make output

	#else
		// X-axis
		P7DIR |= BIT5; //P7.5 x step - make output
		P7DIR |= BIT7; //P7.7 x step direction - make output

		// Y-axis
		P3DIR |= BIT6; //P3.6 y step - make output
		P4DIR |= BIT0; //P4.0 y step direction - make output
	#endif


	P4OUT |=  BIT6; //P4.6 reset - set high (out of reset)
	P7OUT &= ~BIT6; //P4.0 enable - set low (enabled)

	P5DIR |= BIT7;  // P5.7 stepping mode (MODE0) - make output
	P5DIR |= BIT6;  // P5.6 stepping mode (MODE1) - make output
	P4DIR |= BIT7;  // P4.7 stepping mode (MODE2) - make output


	// Configure for full-stepping
	if( TCK2STEP == (double)1 )
	{
		P5OUT &= ~BIT7;  // P5.7 stepping mode (MODE0) - set low
		P5DIR &= ~BIT6;  // P5.6 stepping mode (MODE1) - set low
		P4DIR &= ~BIT7;  // P4.7 stepping mode (MODE2) - set low
	}

	// Configure for half-stepping
	else if( TCK2STEP == (double)2 )
	{
		P5OUT |=  BIT7;  // P5.7 stepping mode (MODE0) - set high
		P5DIR &= ~BIT6;  // P5.6 stepping mode (MODE1) - set low
		P4DIR &= ~BIT7;  // P4.7 stepping mode (MODE2) - set low
	}

	// Configure for quarter-stepping
	else if( TCK2STEP == (double)4 )
	{
		P5OUT &= ~BIT7;  // P5.7 stepping mode (MODE0) - set low
		P5DIR |=  BIT6;  // P5.6 stepping mode (MODE1) - set high
		P4DIR &= ~BIT7;  // P4.7 stepping mode (MODE2) - set low
	}

	// Configure for quarter-stepping
	else if( TCK2STEP == (double)8 )
	{
		P5OUT |=  BIT7;  // P5.7 stepping mode (MODE0) - set high
		P5DIR |=  BIT6;  // P5.6 stepping mode (MODE1) - set high
		P4DIR &= ~BIT7;  // P4.7 stepping mode (MODE2) - set low
	}

	// Configure for quarter-stepping
	else if( TCK2STEP == (double)16 )
	{
		P5OUT &= ~BIT7;  // P5.7 stepping mode (MODE0) - set low
		P5DIR &= ~BIT6;  // P5.6 stepping mode (MODE1) - set low
		P4DIR |=  BIT7;  // P4.7 stepping mode (MODE2) - set high
	}

	// Configure for quarter-stepping
	else if( TCK2STEP == (double)32 )
	{
		P5OUT |=  BIT7;  // P5.7 stepping mode (MODE0) - set high
		P5DIR &= ~BIT6;  // P5.6 stepping mode (MODE1) - set low
		P4DIR |=  BIT7;  // P4.7 stepping mode (MODE2) - set high
	}


	/////////////////////////// Sets up P2.0 as interrupt//////////////////
	P2REN |= BIT0; //pull up resistor
	P2OUT |= BIT0;
	P2IES |= BIT0; // ISR triggered hi-lo transition
	P2IFG &= ~BIT0; // P2.0 IFG cleared
	P2IE  |= BIT0;
	///////////////////////////////////////////////////////////////

	/////////////////////////// Sets up P2.1 as interrupt//////////////////
	P2REN |= BIT1; //pull up resistor
	P2OUT |= BIT1;
	P2IES |= BIT1; // ISR triggered hi-lo transition
	P2IFG &= ~BIT1; // P2.1 IFG cleared
	P2IE  |= BIT1;
	///////////////////////////////////////////////////////////////

	/////////////////////////// Sets up P2.2 as interrupt//////////////////
	P2REN |= BIT2;  // pull up resistor
	P2OUT |= BIT2;
	P2IES |= BIT2;  // ISR triggered hi-lo transition
	P2IFG &= ~BIT2; // P2.2 IFG cleared
	P2IE  |= BIT2;
	///////////////////////////////////////////////////////////////

	///////////////////////////Home Pin Inputs////////////////////////
	#ifdef DEBUG
		P4DIR &= ~BIT3; //P4.3 X home - make input
		P2DIR &= ~BIT3; //P2.3 Y home - make input

	#else
		P4DIR &= ~BIT3; //P4.3 X home - make input
		P2DIR &= ~BIT3; //P2.3 Y home - make input
	#endif
	/////////////////////////////////////////////////////////////////////



	// Setup acceleration vector
	double accel_factors[] = ACCEL_FACTORS;

	for( i = 0; i < ACCEL_SIZE; i++ )
	{
		accel_delay[i] = MIN_TCK_DELAY * accel_factors[i];
	}

	_BIS_SR(GIE);          	// interrupts enabled

	return;

}
//============================================================================



#ifdef DEBUG

// Move motors - Launchpad
uint8_t moveMotors( unsigned int Xnew, unsigned int Ynew ){
	volatile uint32_t temp3 = TCK_DELAY;
	/////////////enable drivers//////////////////////////
	P6OUT |= BIT0;   //unreset drivers LAUNCHPAD
	P6OUT &= ~BIT1;  //enable drivers LAUNCHPAD


	////////////////Set X Direction//////////////////////////
	if(X<Xnew)
	{
		P4OUT |= BIT0;  //positive direction ONLY FOR LAUNCHPAD TESTING
	}
	else
	{
		P4OUT &= ~BIT0;  //negative direction ONLY FOR LAUNCHPAD TESTING
	}
	/////////////////////////////////////////////////////////


	while( ( X - Xnew ) > .001 || ( X - Xnew ) < -.001 ){

		if(X<Xnew){

			P4OUT |= BIT3;  //set step pin  ONLY FOR LAUNCHPAD TESTING
			P1OUT |= DEBUG_LED;

			delay_10us( TCK_DELAY );

			P4OUT &= ~BIT3; //reset step pin
			P1OUT &= ~DEBUG_LED;

			delay_10us( TCK_DELAY );

			ticksX += PXL2TCK;
		}
		else if(X>Xnew){

			P4OUT |= BIT3;  //set step pin ONLY FOR LAUNCHPAD TESTING

			delay_10us( TCK_DELAY );
			

			P4OUT &= ~BIT3; //reset step pin

			delay_10us( TCK_DELAY );
			ticksX -= PXL2TCK;
		}

		X = ticksX; //move to end of while

	}


	////////////////Set Y Direction//////////////////////////
	if(Y<Ynew)
	{
		P8OUT |= BIT2;  //positive direction ONLY FOR LAUNCHPAD TESTING
	}
	else
	{
		P8OUT &= ~BIT2;  //positive direction ONLY FOR LAUNCHPAD TESTING
	}


	while( ( Y - Ynew ) > .001 || ( Y - Ynew ) < -.001 ){

		if(Y<Ynew){

			P3OUT |= BIT7;  //set step pin  ONLY FOR LAUNCHPAD TESTING

			delay_10us( TCK_DELAY );

			P3OUT &= ~BIT7; //reset step pin

			delay_ms(1);

			ticksY += PXL2TCK;
		}
		else if(Y>Ynew){

			P3OUT |= BIT7;   // Set step pin ONLY FOR LAUNCHPAD TESTING

			delay_10us( TCK_DELAY );

			P3OUT &= ~BIT7; //reset step pin

			delay_10us( TCK_DELAY );
			ticksY -= PXL2TCK;
		}

		Y = ticksY; //move to end of while
	  }

	return 0;
}




#else
// Move motors - PCB
uint8_t moveMotors(unsigned int Xnew, unsigned int Ynew){

	uint16_t xDiff;
	uint16_t yDiff;
	uint16_t i = 0;
	uint16_t it_skip = 1;
	uint16_t repeat_it = 1;
	uint16_t accel_it = 0;
	uint16_t num_accel_it = ACCEL_SIZE;

	/////////////enable drivers//////////////////////////
	P4OUT |= BIT6;  //unreset drivers
	P7OUT &= ~BIT6;  //enable drivers


	////////////////Set X Direction//////////////////////////
	if(X<Xnew)
	{
		P7OUT |= BIT7;  //positive direction
		xDiff = ( Xnew - X ) * TCK2PXL;
	}
	else
	{
		P7OUT &= ~BIT7;  //negative direction
		xDiff = ( X - Xnew ) * TCK2PXL;
	}


	if( xDiff > ( 2 * ACCEL_SIZE ) )
	{
		repeat_it = xDiff / ( 2 * ACCEL_SIZE );
		num_accel_it = ACCEL_SIZE * repeat_it;
	}
	else if( xDiff < ACCEL_SIZE )
	{
		it_skip = ACCEL_SIZE / ( xDiff / 2 );
		accel_it = ACCEL_SIZE - 1 - ( it_skip * ( xDiff / 2 - 1 ) );
		num_accel_it = xDiff / 2;
	}
	else if( xDiff < ( 2 * ACCEL_SIZE ) )
	{
		accel_it = ACCEL_SIZE - ( xDiff / 2 );
		num_accel_it = ACCEL_SIZE - accel_it;
	}

	// Why did I think this was needed?
	//num_accel_it /= it_skip;


	/////////////////////////////////////////////////////////


	/*while( ( X - Xnew ) > .001 || ( X - Xnew ) < -.001 ){
		if(X<Xnew){

			P7OUT |= BIT5;  //set step pin

			delay_10us( TCK_DELAY );

			P7OUT &= ~BIT5;

			delay_10us( TCK_DELAY );

			ticksX += PXL2TCK;
		}
		else if(X>Xnew){

			//  P7OUT &= ~BIT7; //negative direction
			P7OUT |= BIT5;  //set step pin

			delay_10us( TCK_DELAY );

			P7OUT &= ~BIT5;

			delay_10us( TCK_DELAY );
			ticksX -= PXL2TCK;
		}

		X = ticksX; //move to end of while
	}*/

	for( i = 0; i < xDiff; i++ )
	{
		volatile uint16_t temp4 = accel_delay[accel_it];
		P7OUT |= BIT5;  //set step pin

		delay_10us( accel_delay[accel_it] );

		P7OUT &= ~BIT5;

		delay_10us( accel_delay[accel_it] );


		if(X<Xnew)
		{
			X += PXL2TCK;
		}
		else
		{
			X -= PXL2TCK;
		}

		if( ( i % repeat_it ) == ( repeat_it - 1 ) )
		{
			if( i < ( num_accel_it - 1 ) )
			{
				accel_it += it_skip;
			}
			else if( i > ( xDiff - num_accel_it - 1 ) )
			{
				accel_it -= it_skip;
			}
		}
	}

	X = Xnew;
	/////////////////////////////////////////////////////////


	////////////////Set Y Direction//////////////////////////
	it_skip = 1;
	repeat_it = 1;
	accel_it = 0;
	num_accel_it = ACCEL_SIZE;



	if(Y<Ynew)
	{
		P4OUT |= BIT0;  //positive direction
		yDiff = ( Ynew - Y ) * TCK2PXL;
	}
	else
	{
		P4OUT &= ~BIT0;  //positive direction
		yDiff = ( Ynew - Y ) * TCK2PXL;
	}


	if( xDiff > ( 2 * ACCEL_SIZE ) )
	{
		repeat_it = xDiff / ( 2 * ACCEL_SIZE );
		num_accel_it = ACCEL_SIZE * repeat_it;
	}
	else if( xDiff < ACCEL_SIZE )
	{
		it_skip = ACCEL_SIZE / ( xDiff / 2 );
		accel_it = ACCEL_SIZE - 1 - ( it_skip * ( xDiff / 2 - 1 ) );
		num_accel_it = xDiff / 2;
	}
	else if( xDiff < ( 2 * ACCEL_SIZE ) )
	{
		accel_it = ACCEL_SIZE - ( xDiff / 2 );
		num_accel_it = ACCEL_SIZE - accel_it;
	}



	/*while( ( Y - Ynew ) > .001 || ( Y - Ynew ) < -.001 ){

		if(Y<Ynew){

			// P4OUT |= BIT0;  //positive direction
			P3OUT |= BIT6;  //set step pin

			delay_10us( TCK_DELAY );

			P3OUT &= ~BIT6;

			delay_10us( TCK_DELAY );
			ticksY += PXL2TCK;
		}
		else if(Y>Ynew){

			// P4OUT &= ~BIT0; // Negative direction
			P3OUT |= BIT6;  // Set step pin

			delay_10us( TCK_DELAY );

			P3OUT &= ~BIT6;

			delay_10us( TCK_DELAY );
			ticksY -= PXL2TCK;
		}

		Y = ticksY; //move to end of while
	}*/

	/*for( i = 0; i < yDiff; i++ )
	{
		P3OUT |= BIT6;  //set step pin

		delay_10us( accel_delay[accel_it] );

		P3OUT &= ~BIT6;

		delay_10us( accel_delay[accel_it] );


		if(Y<Ynew)
		{
			Y += PXL2TCK;
		}
		else
		{
			Y -= PXL2TCK;
		}
	}*/


	for( i = 0; i < yDiff; i++ )
	{
		volatile uint16_t temp5 = accel_delay[accel_it];
		P3OUT |= BIT6;  //set step pin

		delay_10us( accel_delay[accel_it] );

		P3OUT &= ~BIT6;

		delay_10us( accel_delay[accel_it] );


		if(Y<Ynew)
		{
			Y += PXL2TCK;
		}
		else
		{
			Y -= PXL2TCK;
		}

		if( ( i % repeat_it ) == ( repeat_it - 1 ) )
		{
			if( i < ( num_accel_it - 1 ) )
			{
				accel_it += it_skip;
			}
			else if( i > ( yDiff - num_accel_it - 1 ) )
			{
				accel_it -= it_skip;
			}
		}
	}

	Y = Ynew;
	/////////////////////////////////////////////////////////


	return 0;
}
#endif
//============================================================================



#ifdef DEBUG
void homeLaser(void){

	///////////////Set Direction Negative//////////
	P4OUT &= ~BIT0;  //negative X direction
    P8OUT &= ~BIT2;  //negative Y direction
	/////////////////////////////////////////////////


	while (homeX==1){

		//P4OUT &= ~BIT0; //negative direction ONLY FOR LAUNCHPAD TESTING
		P4OUT |= BIT3;  //set step pin ONLY FOR LAUNCHPAD TESTING

		delay_10us( TCK_DELAY );

		P4OUT &= ~BIT3; //reset step pin

		delay_10us( TCK_DELAY );
	}



	while (homeY==1){

		// P8OUT &= ~BIT2;  // Negative direction ONLY FOR LAUNCHPAD TESTING
		P3OUT |= BIT7;   // Set step pin ONLY FOR LAUNCHPAD TESTING

		delay_10us( TCK_DELAY );

		P3OUT &= ~BIT7; //reset step pin

		delay_10us( TCK_DELAY );
	}

	P6OUT &= ~BIT0;  //reset drivers LAUNCHPAD
	P6OUT |= BIT1;  //disable drivers LAUNCHPAD
}


#else
void homeLaser(void){

	///////////////Set Direction Negative//////////
	P7OUT &= ~BIT7;  //negative X direction
    P4OUT &= ~BIT0;  //negative Y direction
	/////////////////////////////////////////////////


	while (homeX==1){

		//P7OUT &= ~BIT7; //negative direction
		P7OUT |= BIT5;  //set step pin

		delay_10us( HOME_TCK_DELAY );

		P7OUT &= ~BIT5;

		delay_10us( HOME_TCK_DELAY );
	}


	while (homeY==1){

		// P4OUT &= ~BIT0; // Negative direction
		P3OUT |= BIT6;  // Set step pin

		delay_10us( HOME_TCK_DELAY );

		P3OUT &= ~BIT7; //reset step pin

		delay_10us( HOME_TCK_DELAY );
	}

	P4OUT &= ~BIT6;  //reset drivers
	P7OUT |= BIT6;  //disable drivers
}
#endif
//============================================================================

////////////////////////////////////////////////////////////////////////////////




// Port 2 interrupt service routine
#ifdef DEBUG
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	//unsigned int flag = (P1IFG & P1IE);
	if(P2IV_P2IFG0){   //XHOME P2.0 interrupt

		P4OUT &= ~BIT3; //stop stepping
		homeX = 0;
		X = 0;
		P2IFG &= ~BIT0; // P2.0 IFG cleared
	}

	if(P2IV_P2IFG2){   ///YHOME P2.2 interrupt

		P3OUT &= ~BIT7; //stop stepping
		homeY = 0;
		Y = 0;
		P2IFG &= ~BIT2; // P2.2 IFG cleared
	}

	if(P2IV_P2IFG6){   ///Lid P2.6 interrupt
		lid = 0;

		P2IFG &= ~BIT6; // P2.2 IFG cleared
	}
}


#else
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	//unsigned int flag = (P1IFG & P1IE);
	if(P2IV_P2IFG0){   //XHOME P2.0 interrupt

		P7OUT &= ~BIT5; //stop stepping
		homeX = 0;
		X=0;
		P2IFG &= ~BIT0; // P2.0 IFG cleared

		P3OUT |= PCB_LED;	// Turn on the debug LED
	}
	if(P2IV_P2IFG1){   //YHOME P2.1 interrupt

		P3OUT &= ~BIT6; //stop stepping
		homeY = 0;
		Y =0;
		P2IFG &= ~BIT1; // P2.1 IFG cleared

		P3OUT &= ~PCB_LED;	// Turn on the debug LED
	}
	if(P2IV_P2IFG2){   // Lid P2.2 interrupt
		lid = 0;

		// need to shut down laser here
		if( picture_ip == TRUE )
		{
			halt_burn();
		}

		P2IFG &= ~BIT2; // P2.2 IFG cleared
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////


