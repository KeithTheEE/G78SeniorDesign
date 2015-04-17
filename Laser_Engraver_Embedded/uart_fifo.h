//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : uart_fifo.h
// Author      : Garin Newcomb, Tyler Troyer, and Chris Haggart
// Email       : gpnewcomb@live.com, troyerta@gmail.com, and chaggart5@gmail.com
// Date		   : 2014-10-11 (Created), 2015-04-16 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Header file including function prototypes and macros used for
//				 uart communication
//============================================================================


#ifndef  UART_FIFO_H_
#define  UART_FIFO_H_


////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "defs.h"

////////////////////////////////////////////////////////////////////////////////


struct TPacket_Data
{
	uint8_t ack;
	uint8_t command;
	uint8_t data[MAX_DATA_SIZE];
	uint8_t data_size;
};

////////////////////////////////////////////////////////////////////////////////


/*rx_flag
* This flag is to be used by other modules to check and see if a new transmission has happened.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile uint8_t rx_flag;

/*rx_fifo_full
* This flag is to be used by other modules to check and see if the rx fifo is full.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile uint8_t rx_fifo_full;

/*tx_fifo_full
* This flag is to be used by other modules to check and see if the rx fifo is full.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile uint8_t tx_fifo_full;

////////////////////////////////////////////////////////////////////////////////


void init_uart( void );

uint8_t uart_getc();
void uart_gets( char* Array, uint16_t length );
uint16_t uart_getp( uint8_t* packet, uint16_t max_length );
void uart_putc( uint8_t c);
void uart_puts( char *str);
void uart_putp( uint8_t *packet, uint16_t length);

uint16_t parse_rx_packet( uint8_t *rx_buff, uint16_t length, struct TPacket_Data * rx_data );
uint16_t pack_tx_packet( struct TPacket_Data tx_data, uint8_t * tx_buff );
void parse_burn_cmd_payload( uint8_t * burn_cmd_payload,
							 uint32_t * yLocation,
							 uint32_t * xLocation,
							 uint32_t * laserInt );

uint8_t calc_8bit_mod_checksum( uint8_t *data, uint16_t length );

void check_and_respond_to_msg( struct TPacket_Data * rx_data );
void send_ready_for_pixel( void );
void send_MSP_initialized( void );
void send_burn_stop      ( void );

void send_ack( uint8_t command, uint8_t ack );

////////////////////////////////////////////////////////////////////////////////


#endif // UART_FIFO_H_

