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
#define ACK_MSG		0x06
#define NAK_MSG		0X3F
#define NEW_CMD		0X00


// Commands
#define CMD_BURN		0x0B		// PI  -> MSP : Pi commands the MSP430 to execute a burn pixel operation (payload includes all necessary data)
#define CMD_PIXEL_READY	0x4D		// MSP -> PI  : MSP has finished previous burn pixel operation and is ready for another (no payload)
#define CMD_EMERGENCY	0x0D		// MSP -> PI  : MSP has encountered a problem and needs to stop the burn (payload indicates failure condition)
#define CMD_MSP_INIT	0x01		// MSP -> PI  : MSP is initialized and ready to burn an image (no payload)
#define CMD_START		0x11		// PI  -> MSP : Pi will commence sending burn pixel commands (no payload)
#define CMD_END			0x0F		// PI  -> MSP : Pi indicates to the MSP that the picture is complete (no payload)


#define CMD_BURN_PAYLOAD_SIZE		4
#define CMD_READY_PAYLOAD_SIZE		0
#define CMD_EMERG_PAYLOAD_SIZE		1
#define CMD_INIT_PAYLOAD_SIZE		0
#define CMD_START_PAYLOAD_SIZE		0
#define CMD_END_PAYLOAD_SIZE		0

#define CMD_BURN_RESPONSE_SIZE		0
#define CMD_READY_RESPONSE_SIZE		0
#define CMD_EMERG_RESPONSE_SIZE		0
#define CMD_INIT_RESPONSE_SIZE		0
#define CMD_START_RESPONSE_SIZE		0
#define CMD_END_RESPONSE_SIZE		0

#define MAX_DATA_SIZE				4
#define MIN_PACKET_LENGTH			3
#define MAX_PACKET_LENGTH			3 + 2 * ( MAX_DATA_SIZE + 1 )
#define FIFO_SIZE 					128


#define MAX_ATTEMPTS				3
//============================================================================

////////////////////////////////////////////////////////////////////////////////


#endif // DEFS_H_
