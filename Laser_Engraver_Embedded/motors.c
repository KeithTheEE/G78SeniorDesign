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

////////////////////////////////////////////////////////////////////////////////


volatile double X = 0;  //Current X pixel assuming 
volatile double Y = 0;  //Current Y pixel assuming
volatile double ticksX = 0; //global ticks
volatile double ticksY = 0; //global ticks
volatile int homeX = 0; //flag for homing x
volatile int homeY = 0; //flag for homing y
const double PXL2TCK = 0.1;

////////////////////////////////////////////////////////////////////////////////


void initMotorIO(void)
{
  //Serial.begin(9600);
  //P1DIR |= (BIT4+BIT5+BIT6) ;       // P1.2 and P1.3 as output 1.2=direction 1.3=steps
  // X-axis
#ifdef DEBUG
  // X-axis
  P4DIR |= BIT3; //P4.3 x step - make output
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


  P4DIR |= BIT6; //P4.6 reset - make output
  P7DIR |= BIT6; //P4.0 enable - make output

  P4OUT |=  BIT6; //P4.6 reset - set high (out of reset)
  P7OUT &= ~BIT6; //P4.0 enable - set low (enabled)

  P5DIR |= BIT6;  //P5.6 stepping mode - make output
  P5DIR |= BIT7;  //P5.7 stepping mode - make output
  P4DIR |= BIT7;  //P4.7 stepping mode - make output

  // Configure for half-stepping
  P5OUT &= ~BIT6;  //P5.6 stepping mode - set low
  P5OUT |=  BIT7;  //P5.6 stepping mode - set high
  P5OUT &= ~BIT6;  //P5.6 stepping mode - set low

  /////////////////////////// Sets up P2.0 as interrupt//////////////////
  P2REN |= BIT0; //pull up resistor
  P2OUT |= BIT0; 
  P2IES |= BIT0; // ISR triggered hi-lo transition 
  P2IFG &= ~BIT0; // P2.0 IFG cleared
  P2IE  |= BIT0;
  ///////////////////////////////////////////////////////////////

  /////////////////////////// Sets up P2.2 as interrupt//////////////////
  P2REN |= BIT2;  // pull up resistor
  P2OUT |= BIT2; 
  P2IES |= BIT2;  // ISR triggered hi-lo transition
  P2IFG &= ~BIT2; // P2.0 IFG cleared
  P2IE  |= BIT2;
  ///////////////////////////////////////////////////////////////  


  _BIS_SR(GIE);          	// interrupts enabled

  return;

}
//============================================================================



#ifdef DEBUG

// Move motors - Launchpad
uint8_t moveMotors( unsigned int Xnew, unsigned int Ynew ){
	if(X<Xnew)
	{
		P4OUT |= BIT0;  //positive direction ONLY FOR LAUNCHPAD TESTING
	}
	else
	{
		P4OUT &= ~BIT0;  //negative direction ONLY FOR LAUNCHPAD TESTING
	}


  while( ( X - Xnew ) > .001 || ( X - Xnew ) < -.001 ){
    if(X<Xnew){

     P4OUT |= BIT3;  //set step pin  ONLY FOR LAUNCHPAD TESTING


      //delayMicroseconds(10);
      delay_ms(1);

      P4OUT &= ~BIT3; //reset step pin

      //delayMicroseconds(5);
      delay_ms(1);
      /*
      P4OUT |= BIT7;  //set step pin
       delay_ms(500);
       P4OUT &= ~BIT7; //reset step pin
       */

      ticksX += PXL2TCK;
    }
    else if(X>Xnew){

      //P4OUT &= ~BIT0; //negative direction ONLY FOR LAUNCHPAD TESTING
      P4OUT |= BIT3;  //set step pin ONLY FOR LAUNCHPAD TESTING

      //delayMicroseconds(10);

      delay_ms(1);

      P4OUT &= ~BIT3; //reset step pin


      //delayMicroseconds(5);
      delay_ms(1);
      ticksX -= PXL2TCK;
    }
    X = ticksX; //move to end of while

  }


  	if(Y<Ynew)
  	{
  		P8OUT |= BIT2;  //positive direction ONLY FOR LAUNCHPAD TESTING
  		P4OUT |= BIT0;  //positive direction
  	}
  	else
  	{
  		P8OUT &= ~BIT2;  //positive direction ONLY FOR LAUNCHPAD TESTING
  		P4OUT &= ~BIT0;  //positive direction
  	}


  while( ( Y - Ynew ) > .001 || ( Y - Ynew ) < -.001 ){
      if(Y<Ynew){

       // P8OUT |= BIT2;  //positive direction ONLY FOR LAUNCHPAD TESTING
        P3OUT |= BIT7;  //set step pin  ONLY FOR LAUNCHPAD TESTING

        //delayMicroseconds(10);
        delay_ms(1);

        P3OUT &= ~BIT7; //reset step pin

        //delayMicroseconds(5);
        delay_ms(1);
        /*
        P4OUT |= BIT7;  //set step pin
         delay_ms(500);
         P4OUT &= ~BIT7; //reset step pin
         */

        ticksY += PXL2TCK;
      }
      else if(Y>Ynew){

       // P8OUT &= ~BIT2;  // Negative direction ONLY FOR LAUNCHPAD TESTING
        P3OUT |= BIT7;   // Set step pin ONLY FOR LAUNCHPAD TESTING

        //delayMicroseconds(10);

        delay_ms(1);

        P3OUT &= ~BIT7; //reset step pin

        //delayMicroseconds(5);
        delay_ms(1);
        ticksY -= PXL2TCK;
      }
      Y = ticksY; //move to end of while
    }

  return 0;
}




#else
// Move motors - PCB
uint8_t moveMotors(unsigned int Xnew, unsigned int Ynew){
	if(X<Xnew)
	{
		P7OUT |= BIT7;  //positive direction
	}
	else
	{
		P7OUT &= ~BIT7;  //negative direction
	}


  while( ( X - Xnew ) > .001 || ( X - Xnew ) < -.001 ){
    if(X<Xnew){

     P7OUT |= BIT5;  //set step pin

      //delayMicroseconds(10);
      delay_ms(1);

      P7OUT &= ~BIT5;

      //delayMicroseconds(5);
      delay_ms(1);
      /*
      P4OUT |= BIT7;  //set step pin
       delay_ms(500);
       P4OUT &= ~BIT7; //reset step pin
       */

      ticksX += PXL2TCK;
    }
    else if(X>Xnew){


    //  P7OUT &= ~BIT7; //negative direction
      P7OUT |= BIT5;  //set step pin

      //delayMicroseconds(10);

      delay_ms(1);

      P7OUT &= ~BIT5;

      //delayMicroseconds(5);
      delay_ms(1);
      ticksX -= PXL2TCK;
    }
    X = ticksX; //move to end of while

  }


  	if(Y<Ynew)
  	{
  		P8OUT |= BIT2;  //positive direction ONLY FOR LAUNCHPAD TESTING
  		P4OUT |= BIT0;  //positive direction
  	}
  	else
  	{
  		P8OUT &= ~BIT2;  //positive direction ONLY FOR LAUNCHPAD TESTING
  		P4OUT &= ~BIT0;  //positive direction
  	}


  while( ( Y - Ynew ) > .001 || ( Y - Ynew ) < -.001 ){
      if(Y<Ynew){

       // P4OUT |= BIT0;  //positive direction
        P3OUT |= BIT6;  //set step pin

        //delayMicroseconds(10);
        delay_ms(1);

        P3OUT &= ~BIT6;

        //delayMicroseconds(5);
        delay_ms(1);
        /*
        P4OUT |= BIT7;  //set step pin
         delay_ms(500);
         P4OUT &= ~BIT7; //reset step pin
         */

        ticksY += PXL2TCK;
      }
      else if(Y>Ynew){

       // P4OUT &= ~BIT0; // Negative direction
        P3OUT |= BIT6;  // Set step pin

        //delayMicroseconds(10);

        delay_ms(1);

        P3OUT &= ~BIT6;

        //delayMicroseconds(5);
        delay_ms(1);
        ticksY -= PXL2TCK;
      }
      Y = ticksY; //move to end of while
    }

  return 0;
}

#endif
//============================================================================



/*void homeLaser(void){
  while (homeX==0){
    P4OUT &= ~BIT0; //negative direction
    P4OUT |= BIT3;  //set step pin
    delay_us(10);

    //delay_us(10);
    P4OUT &= ~BIT3; //reset step pin
    delay_us(5);
    //delay_us(10);
  }
  while (homeY==0){
    P4OUT &= ~BIT0; //negative direction
    P4OUT |= BIT3;  //set step pin
    delay_us(10);

    //delay_us(10);
    P4OUT &= ~BIT3; //reset step pin
    delay_us(5);
    //delay_us(10);
  }
  P1OUT |= BIT4; //disable drivers
  P1OUT &= ~BIT5; //reset drivers
}*/
//============================================================================

////////////////////////////////////////////////////////////////////////////////


// Port 1.4 interrupt service routine

#ifdef DEBUG

#pragma vector=PORT2_VECTOR
__interrupt void Port_1(void)
{
  //unsigned int flag = (P1IFG & P1IE);
  if(P2IV_P2IFG0){   //XHOME
    homeX =1;
    P4OUT &= ~BIT3; //stop stepping
    //P4OUT |= BIT7;
    P2IFG &= ~BIT0; // P2.0 IFG cleared
  }
  if(P2IV_P2IFG2){   ///YHOME
    homeY = 1;
    P3OUT &= ~BIT7; //stop stepping
    //P4OUT |= BIT7;
    P2IFG &= ~BIT2; // P2.2 IFG cleared

    //flag ^= BIT0;
    //P4OUT ^= BIT7;
    //P4OUT |= BIT7;
  }
}


#else

#pragma vector=PORT2_VECTOR
__interrupt void Port_1(void)
{
  //unsigned int flag = (P1IFG & P1IE);
  if(P2IV_P2IFG0){   //XHOME
    homeX =1;
    P7OUT &= ~BIT5; //stop stepping
    //P4OUT |= BIT7;
    P2IFG &= ~BIT0; // P2.0 IFG cleared
  }
  if(P2IV_P2IFG2){   ///YHOME
    homeY = 1;
    P3OUT &= ~BIT6; //stop stepping
    //P4OUT |= BIT7;
    P2IFG &= ~BIT2; // P2.2 IFG cleared

    //flag ^= BIT0;
    //P4OUT ^= BIT7;
    //P4OUT |= BIT7;
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////


