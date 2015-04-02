//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : defs.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com
// Date		   : 2014-10-11 (Created), 2015-04-01 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Header file to define macros used for in the project
//============================================================================


#ifndef DEFS_H_
#define DEFS_H_


////////////////////////////////////////////////////////////////////////////////

//============================================================================
// General

#define FALSE               0
#define TRUE                1
//============================================================================



//============================================================================
// Debug

#define DEBUG_LED 	BIT0
#define PCB_LED 	BIT3
#define BUTTON1 	BIT1
#define BUTTON2 	BIT1
//============================================================================



//============================================================================
// Laser Driver

#define LASER_CTL_PIN			BIT2
#define LASER_ENA_PIN			BIT3

#define LASER_INTENSITY_MASK	0x18
#define LASER_INTENSITY_SHIFT	3

#define LASER_DUR_1				15
#define LASER_DUR_2				30
#define LASER_DUR_3				50
#define LASER_DUR_4				100

#define MAX_INTENSITY 			12300	// 100%
#define INTENSITY_3 			9225	// 75%
#define INTENSITY_2 			6150	// 50%
#define INTENSITY_1 			3075	// 25%
#define FOCUS_INTENSITY 		1230	// 10%
//============================================================================



//============================================================================
// Motor Driver

//#define DEBUG
// #define PXL2TCK 	1 		// Pixel to ticks ratio assuming 1:1
#define XHOME 		BIT4 
// #define YHOME 		BIT45	// TODO: check if this was 4 or 5
//============================================================================



//============================================================================
// UART

#define RXD 		BIT1
#define TXD 		BIT2


#define UCA1_OS   	1    // 1 = oversampling mode, 0 = low-freq mode
#define UCA1_BR0  	17   // Value of UCA1BR0 register
#define UCA1_BR1  	0    // Value of UCA1BR1 register
#define UCA1_BRS  	0    // Value of UCBRS field in UCA1MCTL register
#define UCA1_BRF  	6    // Value of UCBRF field in UCA1MCTL register

// Special characters
#define ETX		  	0X03
#define STX			0x02
#define ESC			0x1B
#define CMD_ACK		0x06
#define CMD_NAK		0X3F
#define NEW_CMD		0X00


// Commands
#define CMD_BURN	0x0B
#define CMD_READY	0x4D
#define CMD_STOP	0x0D


#define CMD_BURN_PAYLOAD_SIZE		4
#define CMD_READY_PAYLOAD_SIZE		0

#define CMD_BURN_RESPONSE_SIZE		0
#define CMD_READY_RESPONSE_SIZE		0

#define MAX_DATA_SIZE				4
#define MIN_PACKET_LENGTH			3
#define MAX_PACKET_LENGTH			3 + 2 * ( MAX_DATA_SIZE + 1 )
#define FIFO_SIZE 					128


#define MAX_ATTEMPTS				3
//============================================================================

////////////////////////////////////////////////////////////////////////////////


#endif // DEFS_H_
