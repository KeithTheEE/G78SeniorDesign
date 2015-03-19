//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : uart_fifo.c
// Author      : Garin Newcomb, Tyler Troyer, and Chris Haggart
// Email       : gpnewcomb@live.com, troyerta@gmail.com, and chaggart5@gmail.com
// Date		   : 2014-10-11 (Created), 2015-03-08 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Source code for uart communication
//============================================================================


////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>

#include "msp430f5529.h"
#include "defs.h"
#include "uart_fifo.h"
#include "time.h"
#include "laser_driver.h"

////////////////////////////////////////////////////////////////////////////////



volatile uint8_t tx_char;			//This char is the most current char to go into the UART

volatile uint8_t rx_flag;			//Mailbox Flag for the rx_char.
volatile uint8_t rx_char;			//This char is the most current char to come out of the UART

volatile uint8_t tx_fifo[FIFO_SIZE];	//The array for the tx fifo
volatile uint8_t rx_fifo[FIFO_SIZE];  //The array for the rx fifo

volatile uint16_t tx_fifo_ptA;			//Theses pointers keep track where the UART and the Main program are in the Fifos
volatile uint16_t tx_fifo_ptB;
volatile uint16_t rx_fifo_ptA;
volatile uint16_t rx_fifo_ptB;

volatile uint8_t rx_fifo_full;
volatile uint8_t tx_fifo_full;

volatile uint8_t packet_ip;
volatile uint8_t packet_ready;

extern volatile uint8_t burn_ready;

////////////////////////////////////////////////////////////////////////////////


/*init_uart
* Sets up the UART interface via USCI
* INPUT: None
* RETURN: None
*/
void init_uart(void)
{
    P4SEL = (BIT4 | BIT5);				// Setup the port as a peripheral

	UCA1CTL1 |= BIT0;					// Hold UART in reset while modifying settings


										// 12,000,000Hz, 115200Baud, UCBRx=106, UCBRSx=6, UCBRFx=0, no oversampling
	UCA1CTL1 |= ( BIT7 | BIT6 );		// Set UART clock to SMCLK
	UCA1BR0   = 106;
	UCA1BR1   = 0;
	UCA1MCTL  = (0 << 4) | (6 << 1) | (0); 			//UCBRFx=0, UCBRSx=6, UCOS16=0

	UCA1CTL1 &= ~(BIT0); 				//USCI state machine - disable software reset capabilities
	UCA1IE   |= BIT0; 					//Enable USCI_A0 RX interrupt


	// Variable initialization
	rx_flag = 0;						//Set rx_flag to 0

	tx_fifo_ptA = 0;					//Set the fifo pointers to 0
	tx_fifo_ptB = 0;
	rx_fifo_ptA = 0;
	rx_fifo_ptB = 0;

	tx_fifo_full = 0;
	rx_fifo_full = 0;

	packet_ip    = 0;
	packet_ready = 0;

	burn_ready = 0;

	__enable_interrupt();				//Interrupts Enabled

	// Delay (not exactly sure why necessary, but first few bytes are gibberish if not added)
	delay_ms( 20 );


	return;
}
//============================================================================


/*uart_getc
* Get a char from the UART. Waits till it gets one
* INPUT: None
* RETURN: Char from UART
*/
uint8_t uart_getc()					// Waits for a valid char from the UART
{
	uint8_t c;

	while (rx_flag == 0);		 			// Wait for rx_flag to be set

	c = rx_fifo[rx_fifo_ptA];				// Copy the fifo
	rx_fifo_ptA++;							// Increase the fifo pointer

	if(rx_fifo_ptA == FIFO_SIZE)			// If pointer is equal to the max size roll it over
	{
		rx_fifo_ptA = 0;
	}

	if(rx_fifo_ptA == rx_fifo_ptB)			// If the pointers are the same we have no new data
	{
		rx_flag = 0;						// ACK rx_flag
	}
    return c;
}
//============================================================================



/*uart_gets
* Get a string of known length from the UART. Strings terminate when enter is pressed or string buffer fills
* Will return when all the chars are received or a carriage return (\r) is received. Waits for the data.
* INPUT: Array pointer and length
* RETURN: None
*/
void uart_gets(char* Array, uint16_t length)
{
	uint16_t i = 0;

	while((i < length))					//Grab data till the array fills
	{
		Array[i] = uart_getc();
		/*if(Array[i] == '\r')				//If we receive a \r the master wants to end
		{
			for( ; i < length ; i++)		//fill the rest of the string with \0 nul. Overwrites the \r with \0
			{
				Array[i] = '\0';
			}
			break;
		}*/
		i++;
	}

    return;
}
//============================================================================



/*uart_getp
* Get a packet of unknown length from the UART. Strings terminate when ETX character is received or string buffer fills
* Will return number of bytes in the packet when all the chars are received. Waits for the data.
* INPUT: Array pointer and length
* RETURN: None
*/
uint16_t uart_getp( uint8_t* packet, uint16_t max_length )
{
	uint16_t i = 0;
	uint8_t temp_char;
	uint8_t message_started = 0;
	// Grab data till the array fills
	for( i = 0; i < max_length; i++ )
	{
		if( message_started == 0 )
		{
			temp_char = uart_getc();
			if( temp_char == STX )
			{
				message_started = 1;
				packet[i] = temp_char;
			}
			else
			{
				i--;
			}
		}
		else
		{
			packet[i] = uart_getc();
		}

		//If we receive an unescaped ETX the master wants to end
		if( packet[i] == ETX )
		{
			uint16_t esc_count = 0;
			while( esc_count < i && packet[ i - esc_count - 1 ] == ESC ) { esc_count++; }

			if( esc_count % 2 == 0 )
			{
				return i;
			}
		}
	}

	printf( "Message Received");

    return max_length;
}
//============================================================================



/*uart_putc
* Sends a char to the UART. Will wait if the UART is busy
* INPUT: Char to send
* RETURN: None
*/
void uart_putc(uint8_t c)
{
	tx_char = c;						//Put the char into the tx_char
	tx_fifo[tx_fifo_ptA] = tx_char;		//Put the tx_char into the fifo
	tx_fifo_ptA++;						//Increase the fifo pointer
	if(tx_fifo_ptA == FIFO_SIZE)		//Check to see if the pointer is max size. If so roll it over
	{
		tx_fifo_ptA = 0;
	}
	if(tx_fifo_ptB == tx_fifo_ptA)		//fifo full
	{
		tx_fifo_full = 1;
	}
	else
	{
		tx_fifo_full = 0;
	}
	UCA1IE |= BIT1; 					//Enable USCI_A0 TX interrupt
	return;
}
//============================================================================



/*uart_puts
* Sends a string to the UART. Will wait if the UART is busy
* INPUT: Pointer to String to send
* RETURN: None
*/
void uart_puts(char *str)				//Sends a String to the UART.
{
     while(*str) uart_putc(*str++);		//Advance though string till end
     return;
}
//============================================================================



/*uart_putp
* Sends a string to the UART. Will wait if the UART is busy
* INPUT: Pointer to String to send
* RETURN: None
*/
void uart_putp( uint8_t *packet, uint16_t length )					//Sends a String to the UART.
{
	uint16_t i;
	for( i = 0; i < length; i++ )
	{
		uart_putc( packet[i] );
	}

     return;
}
//============================================================================



/*parse_rx_packet
* Parses a received packet
* RETURN: 1 if an error occurred in transmission, 0 else
*/
uint16_t parse_rx_packet( uint8_t *rx_buff, uint16_t length, struct TPacket_Data * rx_data )
{
	// Set the command to 'NAK' originally, so if an error occurs before the command is read, the calling function
	//  doesn't get a false command
	rx_data->command   = CMD_NAK;
	rx_data->ack       = CMD_NAK;
	rx_data->data_size = 0;


	// Packet must consist of at least STX, ETX, command, specified data bytes,
	//   and a checksum byte (if data_size > 0)
	if( length < MIN_PACKET_LENGTH )
	{
		return 1;
	}

	uint16_t rx_it = 0;

	// Check to make sure STX was received
	if(  rx_buff[rx_it++] != STX )
	{
		// Start of Text Character (0x02) was not received;
		return 1;
	}


	/*// No address used at present
	// Check to make sure the correct Address was received
	if( rx_buff[rx_it++] != address )
	{
		return 1;
	}
	*/

	// Check if an ACK/NAK was received
	if( rx_buff[rx_it] == CMD_ACK )
	{
		rx_data->ack = CMD_ACK;
		rx_it++;
	}
	else if( rx_buff[rx_it] == CMD_NAK )
	{
		// resend message
		rx_data->ack = CMD_NAK;
		rx_it++;
	}
	else
	{
		rx_data->ack = NEW_CMD;
	}

	rx_data->command = rx_buff[rx_it++];


	if( rx_data->ack == NEW_CMD )
	{
		switch( rx_data->command )
		{
			case CMD_BURN  :	rx_data->data_size = CMD_BURN_PAYLOAD_SIZE;		break;
		  //case CMD_READY :	rx_data->data_size = CMD_READY_PAYLOAD_SIZE;	break;

			// If command not recognized, return an error
			default		   : 	rx_data->command = CMD_NAK;
								return 1;
		}
	}
	else
	{
		switch( rx_data->command )
		{
		  //case CMD_BURN  :	rx_data->data_size = CMD_BURN_RESPONSE_SIZE;	break;
			case CMD_READY :	rx_data->data_size = CMD_READY_RESPONSE_SIZE;	break;

			// If command not recognized, return an error
			default		   : 	rx_data->command = CMD_NAK;
								return 1;
		}
	}


	if( rx_data->data_size != 0 )
	{
		// Add the data bytes to the field of the rx_data structure
		// MSB First
		int16_t j = rx_data->data_size - 1;
		int16_t temp = 0xFF;
		
		while( j >= 0 && rx_it < length )
		{
			// Remove ESC characters from the Response field of the rx_buff
			if( rx_buff[rx_it] == ESC ) { rx_it++; }

			rx_data->data[j] = rx_buff[rx_it];
			temp = rx_data->data[j];
			rx_it++;
			j--;
		}
		
		// MSB Last
		/*int16_t j = 0;
		
		while( j <  rx_data->data_size && rx_it < length )
		{
			// Remove ESC characters from the Response field of the rx_buff
			if( rx_buff[rx_it] == ESC ) { rx_it++; }

			rx_data->data[j] = rx_buff[rx_it];
			rx_it++;
			j++;
		}*/

		// Payload not the correct size
		if( rx_it >= length )
		{
			return 1;
		}

		// rx_buff[i] now points to the cheksum field of the rx_buff

		// Check checksum field for ESC character and remove it if present
		if( rx_buff[rx_it] == ESC ) { rx_it++; }

		uint8_t exp_checksum = calc_8bit_mod_checksum( rx_data->data, rx_data->data_size );

		if( rx_buff[rx_it] != exp_checksum )
		{
			// Error in data transmission
			/*
			uart_putc(exp_checksum);
			uart_putc(rx_buff[rx_it]);
			*/

			return 1;
		}

		rx_it++; // the ith element is now the ETX field of the rx_buff
	}

	// Check if ETX was received and if packet was properly terminated
	if(rx_buff[rx_it] != ETX )
	{
		return 1;
	}

	return 0;
}
//============================================================================



uint16_t pack_tx_packet( struct TPacket_Data tx_data, uint8_t * tx_buff )
{
	uint16_t tx_it = 0;

	tx_buff[tx_it++] = STX;

	if( tx_data.ack == CMD_NAK || tx_data.ack == CMD_ACK )
	{
		tx_buff[tx_it++] = tx_data.ack;

		if( tx_data.command != CMD_NAK )
		{
			tx_buff[tx_it++] = tx_data.command;
		}
	}
	else
	{
		tx_buff[tx_it++] = tx_data.command;

		if( tx_data.data_size > 0 )
		{
			// MSB First
			int16_t i = 0;
			
			for( i = tx_data.data_size - 1; i >= 0; i-- )
			{
				if( tx_data.data[i] == STX || tx_data.data[i] == ETX || tx_data.data[i] == ESC )
				{
					tx_buff[tx_it++] = ESC;
				}

				tx_buff[tx_it++] = tx_data.data[i];
			}
			
			// MSB Last
			/*int16_t i = 0;
			
			for( i = 0; i < tx_data.data_size; i++ )
			{
				if( tx_data.data[i] == STX || tx_data.data[i] == ETX || tx_data.data[i] == ESC )
				{
					tx_buff[tx_it++] = ESC;
				}

				tx_buff[tx_it++] = tx_data.data[i];
			}*/
			

			uint8_t checksum = calc_8bit_mod_checksum( tx_data.data, tx_data.data_size );

			if( checksum == STX || checksum == ETX || checksum == ESC )
			{
				tx_buff[tx_it++] = ESC;
			}

			tx_buff[tx_it++] = checksum;
		}
	}


	tx_buff[tx_it] = ETX;

	// Return the length of the buffer
	return ( tx_it + 1 );
}
//============================================================================



//uint32_t testvalue= 0x802000E;        used these to test this function
//uint8_t * burnpayload = &testvalue;

void parse_burn_cmd_payload( uint8_t * burn_cmd_payload,
							 uint32_t * yLocation,
							 uint32_t * xLocation,
							 uint32_t * laserInt )
{
	uint32_t combinedPacket  = 0;
	uint32_t combinedPacket2 = 0;
	uint16_t i = 0;

	volatile uint32_t tempy;
	volatile uint32_t tempx;
	volatile uint32_t tempint;

	// Data stored with LSB first
	combinedPacket |= (uint32_t)burn_cmd_payload[i];
	i++;

	combinedPacket |= ( (uint32_t)burn_cmd_payload[i] << 8 );
	i++;

	// Had to use two different ints because shifting by 16 wasn't working
	combinedPacket2 |= ( (uint32_t)burn_cmd_payload[i] << 0 );
	i++;

	combinedPacket2 |= ( (uint32_t)burn_cmd_payload[i] << 8 );

	combinedPacket= (combinedPacket | (combinedPacket2 <<16)); // Combine ints

	// Set y coordinate
	*yLocation= combinedPacket;
	*yLocation &= 0x00003FFE;
	*yLocation = (*yLocation >> 1);

	tempy = *yLocation;

	// Set x coordinate
	*xLocation = combinedPacket;
	*xLocation &= 0x07FFC000;
	*xLocation = (*xLocation >> 14);

	tempx = *xLocation;

	// Set Laser intensity
	*laserInt = combinedPacket;
	*laserInt = (*laserInt & 0x18000000);
	*laserInt= (*laserInt >> 27);

	tempint = *laserInt;

	return;
}
//============================================================================



uint8_t calc_8bit_mod_checksum( uint8_t *data, uint16_t length )
{
	uint8_t mod_sum = 0;
	uint16_t i = 0;

	for( i = 0; i < length; i++ )
	{
		mod_sum += data[i];
	}

	return 256 - mod_sum;
}
//============================================================================



void check_and_respond_to_msg( struct TPacket_Data * rx_data )
{
	if( packet_ready == 1 )
	{
		uint8_t rx_packet[MAX_PACKET_LENGTH];

		struct TPacket_Data lrx_data;
		uint16_t rx_size = uart_getp( rx_packet, MAX_PACKET_LENGTH );
		if( parse_rx_packet( rx_packet, rx_size, &lrx_data ) == 0 )
		{
			switch( lrx_data.command )
			{
				case CMD_BURN :  send_ack( lrx_data.command, CMD_ACK );
								 burn_ready = 1;
							     break;

				case CMD_READY : break;

				// Bad commands should be caught in the parsing function
				default		  :  send_ack( lrx_data.command, CMD_NAK );
			}
		}
		else
		{
			send_ack( lrx_data.command, CMD_NAK );
		}

		if( rx_data != 0 ) { *rx_data = lrx_data; }
	}

	return;
}
//============================================================================



void send_ready( void )
{
	struct TPacket_Data tx_data;
	tx_data.command = CMD_READY;
	tx_data.ack = NEW_CMD;
	tx_data.data_size = 0;

	uint8_t tx_buff[MIN_PACKET_LENGTH];
	uint16_t tx_length = pack_tx_packet( tx_data, tx_buff );

	uint16_t i = 0;
	struct TPacket_Data rx_data;
	rx_data.ack = 0;
	rx_data.command = 0;

	// Attempt to send the message several times, waiting for the correct response
	//   (acknowledgement and ready command)
	while( i < MAX_ATTEMPTS && !( rx_data.ack == CMD_ACK && rx_data.command == CMD_READY ) )
	{
		uart_putp( tx_buff, tx_length );
		check_and_respond_to_msg( &rx_data );

		i++;
	}

	// If connection with the Pi is lost, halt the burn
	if( !( rx_data.ack == CMD_ACK && rx_data.command == CMD_READY ) )
	{
		halt_burn();
	}


	return;
}
//============================================================================



void send_burn_stop( void )
{
	struct TPacket_Data tx_data;
	tx_data.command = CMD_STOP;
	tx_data.ack = NEW_CMD;
	tx_data.data_size = 0;					// No payload

	uint8_t tx_buff[MIN_PACKET_LENGTH];		// Minimum packet length (STX, CMD, ETX)
	uint16_t tx_length = pack_tx_packet( tx_data, tx_buff );

	struct TPacket_Data rx_data;
	rx_data.ack = 0;
	rx_data.command = 0;

	// Attempt to send the message, waiting for the correct response
	//   (acknowledgement and burn stop command)
	//	 Since this implies failure, keep sending indefinitely
	while( !( rx_data.ack == CMD_ACK && rx_data.command == CMD_STOP ) )
	{
		uart_putp( tx_buff, tx_length );
		check_and_respond_to_msg( &rx_data );
	}

	return;
}
//============================================================================



void send_ack( uint8_t command, uint8_t ack )
{
	struct TPacket_Data tx_data;
	tx_data.command = command;
	tx_data.ack = ack;
	tx_data.data_size = 0;

	uint8_t tx_buff[MIN_PACKET_LENGTH + 1];
	uint16_t tx_length = pack_tx_packet( tx_data, tx_buff );
	uart_putp( tx_buff, tx_length );

	return;
}
//============================================================================

////////////////////////////////////////////////////////////////////////////////


//UART RX/TX USCI Interrupt. This triggers when the USCI receives a char or when we try to send one.
#pragma vector = USCI_A1_VECTOR		
__interrupt void USCI0RXTX_ISR(void)
{
	uint8_t UCA1IV_temp = UCA1IV;

	if(UCA1IV_temp & BIT1)
	{
		rx_char = UCA1RXBUF;				//Copy from RX buffer, in doing so we ACK the interrupt as well
		rx_flag = 1;						//Set the rx_flag to 1

		rx_fifo[rx_fifo_ptB] = rx_char;		//Copy the rx_char into the fifo
		rx_fifo_ptB++;

		if(rx_fifo_ptB == FIFO_SIZE)		//Roll the fifo pointer over
		{
			rx_fifo_ptB = 0;
		}

		if( rx_fifo_ptA == rx_fifo_ptB )		//fifo full
		{
			rx_fifo_full = 1;
		}
		else
		{
			rx_fifo_full = 0;
		}

		if( rx_char == STX && packet_ip == 0 )
		{
			packet_ip = 1;

			if( packet_ready == 1 )
			{
				// TODO: Account for case where two messages received at once
			}
		}
		else if( rx_char == ETX && packet_ip == 1)
		{
			uint16_t esc_count = 0;
			while( rx_fifo[ ( rx_fifo_ptB - 1 ) - esc_count - 1 ] == ESC ) { esc_count++; }

			if( esc_count % 2 == 0 )
			{
				packet_ip 	 = 0;
				packet_ready = 1;
			}
		}
	}
	else if(UCA1IV_temp & BIT2)
	{
		UCA1TXBUF = tx_fifo[tx_fifo_ptB];		//Copy the fifo into the TX buffer
		tx_fifo_ptB++;

		if(tx_fifo_ptB == FIFO_SIZE)			//Roll the fifo pointer over
		{
			tx_fifo_ptB = 0;
		}
		if(tx_fifo_ptB == tx_fifo_ptA)			//Fifo pointers the same no new data to transmit
		{
			UCA1IE &= ~(BIT1); 					//Turn off the interrupt to save CPU
		}
	}
}
//============================================================================

////////////////////////////////////////////////////////////////////////////////


