
// All the bitmasks for a 16 bit processor
#define BIT15		0x8000  // 1000 0000 0000 0000
#define BIT14		0x4000  // 0100 0000 0000 0000
#define BIT13		0x2000  // 0010 0000 0000 0000
#define BIT12		0x1000  // 0001 0000 0000 0000
#define BIT11		0x0800  // 0000 1000 0000 0000
#define BIT10		0x0400  // 0000 0100 0000 0000
#define BIT9		0x0200  // 0000 0010 0000 0000
#define BIT8		0x0100  // 0000 0001 0000 0000
#define BIT7        0x0080  // 0000 0000 1000 0000
#define BIT6        0x0040  // 0000 0000 0100 0000

#define BIT5        0x0020  // 0000 0000 0010 0000
#define BIT4        0x0010  // 0000 0000 0001 0000
#define BIT3        0x0008  // 0000 0000 0000 1000
#define BIT2        0x0004  // 0000 0000 0000 0100
#define BIT1        0x0002  // 0000 0000 0000 0010
#define BIT0        0x0001  // 0000 0000 0000 0001

// Port Select Registers - use these to choose pin functionality
#define P4SEL_REG  	0x022B
#define P4SEL  		(*((uint8_t*)P4SEL_REG))

// Basic Port Control is easily demonstrated with these two registers. P1.0 is the red lead
#define PORT1_OUT_REG      (0x0202)
#define PORT1_DIR_REG	   (0x0204)
#define PORT1_SEL_REG	   (0x0204)
#define PORT1_OUT          (*((uint16_t*)PORT1_OUT_REG))
#define PORT1_DIR		   (*((uint16_t*)PORT1_DIR_REG))
#define PORT1_SEL		   (*((uint16_t*)PORT1_SEL_REG))

// Watchdog Timer
#define WDTPW                  (0x5A00)		  /* WDT - Control Register Password */
#define WDTHOLD_L              (0x0080)       /* WDT - Timer hold */

#define WDTCTL_REG  (0x015C)
#define WDTCTL  (*((uint16_t*)WDTCTL_REG))

// UCS Register Addresses
#define UCS_REG_STRT 0x0160
#define UCS_CTL_REG0	(0x0160 + 0x00)
#define UCS_CTL_REG1	(0x0160 + 0x02)
#define UCS_CTL_REG2	(0x0160 + 0x04)
#define UCS_CTL_REG3	(0x0160 + 0x06)
#define UCS_CTL_REG4	(0x0160 + 0x08)
#define UCS_CTL_REG5	(0x0160 + 0x0A)
#define UCS_CTL_REG6	(0x0160 + 0x0C)
#define UCS_CTL_REG7	(0x0160 + 0x0E)
#define UCS_CTL_REG8	(0x0160 + 0x10)

// UCS Register Pointers
#define UCSCTL0			(*((uint16_t*)UCS_CTL_REG0))
#define UCSCTL1			(*((uint16_t*)UCS_CTL_REG1))
#define UCSCTL2			(*((uint16_t*)UCS_CTL_REG2))
#define UCSCTL3			(*((uint16_t*)UCS_CTL_REG3))
#define UCSCTL4			(*((uint16_t*)UCS_CTL_REG4))
#define UCSCTL5			(*((uint16_t*)UCS_CTL_REG5))
#define UCSCTL6			(*((uint16_t*)UCS_CTL_REG6))
#define UCSCTL7			(*((uint16_t*)UCS_CTL_REG7))
#define UCSCTL8			(*((uint16_t*)UCS_CTL_REG8))

// UCA1 Register Addresses
#define UCA1CTL1_REG 	(0x0600 + 0x00)
#define UCA1CTL0_REG 	(0x0600 + 0x01)
#define UCA1BR0_REG 	(0x0600 + 0x06)
#define UCA1BR1_REG 	(0x0600 + 0x07)
#define UCA1MCTL_REG 	(0x0600 + 0x08)
#define UCA1STAT_REG 	(0x0600 + 0x0A)
#define UCA1RXBUF_REG 	(0x0600 + 0x0C)
#define UCA1TXBUF_REG 	(0x0600 + 0x0E)
#define UCA1ABCTL_REG 	(0x0600 + 0x10)
#define UCA1IRTCTL_REG 	(0x0600 + 0x12)
#define UCA1IRRCTL_REG 	(0x0600 + 0x13)
#define UCA1IE_REG 		(0x0600 + 0x1C)
#define UCA1IFG_REG 	(0x0600 + 0x1D)
#define UCA1IV_REG 		(0x0600 + 0x1E)

// UCA1 Register Pointers
#define UCA1CTL1		(*((uint8_t*)UCA1CTL1_REG))
#define UCA1CTL0		(*((uint8_t*)UCA1CTL0_REG))
#define UCA1BR0			(*((uint8_t*)UCA1BR0_REG))
#define UCA1BR1			(*((uint8_t*)UCA1BR1_REG))
#define UCA1MCTL		(*((uint16_t*)UCA1MCTL_REG))
#define UCA1STAT		(*((uint16_t*)UCA1STAT_REG))
#define UCA1RXBUF		(*((uint16_t*)UCA1RXBUF_REG))
#define UCA1TXBUF		(*((uint16_t*)UCA1TXBUF_REG))
#define UCA1ABCTL		(*((uint16_t*)UCA1ABCTL_REG))
#define UCA1IRTCTL		(*((uint16_t*)UCA1IRTCTL_REG))
#define UCA1IRRCTL		(*((uint16_t*)UCA1IRRCTL_REG))
#define UCA1IE			(*((uint16_t*)UCA1IE_REG))
#define UCA1IFG			(*((uint16_t*)UCA1IFG_REG))
#define UCA1IV			(*((uint16_t*)UCA1IV_REG))

// Timer A Channel 0 Register Addresses
#define TA0CTL_REG	 	(0x0340 + 0x00) 
#define TA0CCTL0_REG 	(0x0340 + 0x02)
#define TA0CCTL1_REG 	(0x0340 + 0x04)
#define TA0CCTL2_REG 	(0x0340 + 0x06)
#define TA0CCTL3_REG 	(0x0340 + 0x08)
#define TA0CCTL4_REG 	(0x0340 + 0x0A)
#define TA0CCTL5_REG 	(0x0340 + 0x0C)
#define TA0CCTL6_REG 	(0x0340 + 0x0E)
#define TA0R_REG 	 	(0x0340 + 0x10)
#define TA0CCR0_REG 	(0x0340 + 0x12)
#define TA0CCR1_REG 	(0x0340 + 0x14)
#define TA0CCR2_REG 	(0x0340 + 0x16)
#define TA0CCR3_REG 	(0x0340 + 0x18)
#define TA0CCR4_REG 	(0x0340 + 0x1A)
#define TA0CCR5_REG 	(0x0340 + 0x1C)
#define TA0CCR6_REG 	(0x0340 + 0x1E)
#define TA0IV_REG 		(0x0340 + 0x2E)
#define TA0EX0_REG 		(0x0340 + 0x20)

// Timer A Channel 0 Pointers
#define TA0CTL	 	(*((uint16_t*)TA0CTL_REG)) 
#define TA0CCTL0 	(*((uint16_t*)TA0CCTL0_REG)) 
#define TA0CCTL1 	(*((uint16_t*)TA0CCTL1_REG)) 
#define TA0CCTL2	(*((uint16_t*)TA0CCTL2_REG)) 
#define TA0CCTL3 	(*((uint16_t*)TA0CCTL3_REG)) 
#define TA0CCTL4 	(*((uint16_t*)TA0CCTL4_REG)) 
#define TA0CCTL5	(*((uint16_t*)TA0CCTL5_REG)) 
#define TA0CCTL6 	(*((uint16_t*)TA0CCTL6_REG)) 
#define TA0R 	 	(*((uint16_t*)TA0R_REG)) 
#define TA0CCR0 	(*((uint16_t*)TA0CCR0_REG)) 
#define TA0CCR1 	(*((uint16_t*)TA0CCR1_REG)) 
#define TA0CCR2 	(*((uint16_t*)TA0CCR2_REG)) 
#define TA0CCR3		(*((uint16_t*)TA0CCR3_REG)) 
#define TA0CCR4		(*((uint16_t*)TA0CCR4_REG)) 
#define TA0CCR5 	(*((uint16_t*)TA0CCR5_REG)) 
#define TA0CCR6 	(*((uint16_t*)TA0CCR6_REG)) 
#define TA0IV		(*((uint16_t*)TA0IV_REG)) 
#define TA0EX0		(*((uint16_t*)TA0EX0_REG)) 


/*
Example Interrupt Handler Syntax
USC_A1_VECTOR = 46
See Page 21 of MSP430F5529 Device SPecific Datasheet
#pragma vector=46
__interrupt void USC_A1(void)    { ISR code    <---- You can call this function whatever you would like to

  Check is this instance is for Reception of Transmission - Reception should take priority if both IRX flags are set
  if(checkRxFlag())
  {
    Handle Byte




  }
  if(checkTxFlag())
  {
    
    
  }

Software Interrupt Enable
#define SFRIE1_REG	(0x0100)
#define SFRIE1		(*((uint16_t*)SFRIE1_REG))
*/
