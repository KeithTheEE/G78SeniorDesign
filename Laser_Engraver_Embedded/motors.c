//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : motors.c
// Author      : Fernando Urias-Cordero
// Email       : furias.cordero@gmail.com
// Date		   : 2015-01-26 (Created), 2015-03-09 (Last Updated)
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


volatile int X = 0;  //Current X pixel assuming 
volatile int ticksX = 0; //global ticks
volatile int homeX = 0; //flag for homing x
volatile int homeY = 0; //flag for homing y

////////////////////////////////////////////////////////////////////////////////


void initMotorIO(void)
{
  //Serial.begin(9600);
  P1DIR |= (BIT2 + BIT3 +BIT4+BIT5+BIT6) ;       // P1.2 and P1.3 as output 1.2=direction 1.3=steps
  P4DIR |= BIT7; //P4.7 led on launchpad
  P7DIR |= (BIT4 + BIT5); //P7.4 and P7.5 x and y steps respectively make pins outputs
  P7DIR |= (BIT6 + BIT7); //P7.6 and P7.7 x and y step direction respectively make pins outputs

  /////////////////////////// Sets up P1.4 as interrupt//////////////////
  P2REN |= BIT0; //pull up resistor
  P2OUT |= BIT0; 
  P2IES |= BIT0; // ISR triggered hi-lo transition 
  P2IFG &= ~BIT0; // P2.0 IFG cleared
  P2IE  |= BIT0;
  ///////////////////////////////////////////////////////////////

  /////////////////////////// Sets up P1.5 as interrupt//////////////////
  P2REN |= BIT2; //pull up resistor
  P2OUT |= BIT2; 
  P2IES |= BIT2; // ISR triggered hi-lo transition 
  P2IFG &= ~BIT2; // P2.0 IFG cleared
  P2IE  |= BIT2;
  ///////////////////////////////////////////////////////////////  

  P4OUT &= ~BIT7;
  //P4OUT |= BIT7;
  P1OUT &= ~BIT5;


  _BIS_SR(GIE);          	// interrupts enabled

  return;

}
//============================================================================



void moveMotors(unsigned int Xnew, unsigned int Ynew){
  while(X != Xnew){
    if(X<Xnew){
      P1OUT |= BIT2;  //positive direction ONLY FOR LAUNCHPAD TESTING
      P1OUT |= BIT3;  //set step pin  ONLY FOR LAUNCHPAD TESTING
      P7OUT |= BIT6;  //positive direction
      P7OUT |= BIT4;  //set step pin

      //delayMicroseconds(10);
      delay_us(1000);
      P1OUT &= ~BIT3; //reset step pin
      //delayMicroseconds(5);
      delay_us(1000);
      /*
      P4OUT |= BIT7;  //set step pin
       delay_ms(500);
       P4OUT &= ~BIT7; //reset step pin
       */

      ticksX++;
    }
    else if(X>Xnew){
      P1OUT &= ~BIT2; //negative direction ONLY FOR LAUNCHPAD TESTING
      P1OUT |= BIT3;  //set step pin ONLY FOR LAUNCHPAD TESTING
      P7OUT &= ~BIT6; //negative direction
      P7OUT |= BIT4;  //set step pin

      //delayMicroseconds(10);

      delay_us(1000);
      P1OUT &= ~BIT3; //reset step pin
      //delayMicroseconds(5);
      delay_us(1000);
      ticksX--;
    }
    X = ticksX * PXL2TCK; //move to end of while

    return;


  }
}
//============================================================================



void homeLaser(void){
  while (homeX==0){
    P1OUT &= ~BIT2; //negative direction
    P1OUT |= BIT3;  //set step pin
    delay_us(10);

    //delay_us(10);
    P1OUT &= ~BIT3; //reset step pin
    delay_us(5);
    //delay_us(10);
  }
  while (homeY==0){
    P1OUT &= ~BIT2; //negative direction
    P1OUT |= BIT3;  //set step pin
    delay_us(10);

    //delay_us(10);
    P1OUT &= ~BIT3; //reset step pin
    delay_us(5);
    //delay_us(10);
  }
  P1OUT |= BIT4; //disable drivers
  P1OUT &= ~BIT5; //reset drivers
}
//============================================================================

////////////////////////////////////////////////////////////////////////////////


// Port 1.4 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_1(void)

{
  //unsigned int flag = (P1IFG & P1IE);
  if(P2IV_P2IFG0){   //XHOME
    homeX =1;
    P7OUT &= ~BIT4; //stop stepping
    //P4OUT |= BIT7;
    P2IFG &= ~BIT0; // P2.0 IFG cleared
  }
  if(P2IV_P2IFG2){   ///YHOME
    homeY = 1;
    P7OUT &= ~BIT5; //stop stepping
    //P4OUT |= BIT7;
    P2IFG &= ~BIT2; // P2.2 IFG cleared

    //flag ^= BIT0;
    //P4OUT ^= BIT7;
    //P4OUT |= BIT7;
  }
}

////////////////////////////////////////////////////////////////////////////////

