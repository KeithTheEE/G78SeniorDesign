//============================================================================
// Project	   : Laser Engraver Embedded
// Name        : main.c
// Author      : Garin Newcomb and Tyler Troyer
// Email       : gpnewcomb@live.com and troyerta@gmail.com
// Date		   : 2014-10-11 (Created), 2015-04-01 (Last Updated)
// Copyright   : Copyright 2014-2015 University of Nebraska-Lincoln
// Description : Laser Engraver Embedded project 'Main' file, including
//				 'main()'
//============================================================================


////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>

#include "msp430f5529.h"
#include "defs.h"
#include "uart_fifo.h"
#include "laser_driver.h"
#include "time.h"
#include "debug.h"
#include "motors.h"

////////////////////////////////////////////////////////////////////////////////


// char test_string[8];

extern volatile uint8_t packet_ready;
extern volatile uint8_t burn_ready;
extern volatile uint8_t picture_ip;
extern uint32_t time_ms;

////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	// ------------------------------
	// Variable Declaration
	uint16_t i, j;
	uint16_t time = 0;
	uint16_t intensity = 0;
	uint16_t button_pressed1 = FALSE;
	uint16_t button_pressed2 = FALSE;
	uint16_t input_pin_high  = FALSE;
	// ------------------------------



	// ------------------------------
	// Setup
	#ifdef DEBUG
		init_button_inputs();
		init_debug_LED();
	#else
		init_pcb_input();
		init_pcb_LED();
		P3OUT |= PCB_LED;	// Turn on the debug LED
	#endif

	init_clocks();
	init_laser();
    init_uart();
	initMotorIO();
	initWaitTimer();
	
	// Indicate to the Pi that everything has been initialized
	send_MSP_initialized();
	// ------------------------------
	
	

	// ------------------------------
	// Simulated Checkerboard burn

	// Wait for input pin to go high
		/*while( input_pin_high == FALSE )
		{
			if( ( P6IN & BIT7 ) != 0 )
			{
				delay_ms( 20 );
				if( ( P6IN & BIT7 ) != 0 )
				{
					input_pin_high = TRUE;
				}
			}
		}*/

/*
	int burn_array[50][50];

	for( i = 0; i < 50; i++ )
	{
		for( j = 0; j < 50; j++ )
		{
			burn_array[i][j] = 0;
		}
	}

	for( i = 0; i < 50; i++ )
	{
		for( j = 0; j < 50; j++ )
		{
			if( ( i < 10 && j < 10 ) ||
				( i < 30 && i >= 20 && j < 10 && j >= 0 ) ||
				( i < 20 && i >= 10 && j < 20 && j >= 10 ) ||
				( i < 10 && i >= 0  && j < 30 && j >= 20 ) ||
				( i < 50 && i >= 40 && j < 30 && j >= 20 ) ||
				( i < 50 && i >= 40 && j < 50 && j >= 40 ) ||
				( i < 40 && i >= 30 && j < 40 && j >= 30 ) ||
				( i < 30 && i >= 20 && j < 50 && j >= 40 ) )
			{
				burn_array[i][j] = 4;
			}


			if( ( i < 10 && i >= 0  && j < 50 && j >= 40 ) ||
				( i < 20 && i >= 10 && j < 40 && j >= 30 ) ||
				( i < 30 && i >= 20 && j < 30 && j >= 20 ) ||
				( i < 40 && i >= 30 && j < 20 && j >= 10 ) ||
				( i < 50 && i >= 40 && j < 10 && j >= 0 ) )
			{
				burn_array[i][j] = 2;
			}
		}
	}


	for( i = 0; i < 50; i++ )
	{
		for( j = 0; j < 50; j++ )
		{
			if( burn_array[i][j] != 0 )
			{
				moveMotors( j, i);

				switch( burn_array[i][j] - 1 )
				{
					case 0:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_1 );
							 break;

					case 1:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_2 );
							 break;

					case 2:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_3 );
							 break;

					case 3:  turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_4 );
							 break;

					default: break;
				}
			}

		}
	}*/
	// ------------------------------



	// ------------------------------
	// Burn Tests
	
	// Wait for Button 1 Press
	/*while( 1 )
	{
		button_pressed1 = FALSE;
		button_pressed2 = FALSE;


		/*while( button_pressed1 == FALSE )
		{
			if( ( P1IN & BUTTON1 ) == 0 )
			{
				delay_ms( 20 );
				if( ( P1IN & BUTTON1 ) == 0 )
				{
					button_pressed1 = TRUE;
				}
			}
		}*/

		/*turn_on_laser_timed( MAX_INTENSITY,  time );
		//turn_on_laser_timed( intensity,  60 );

		//delay_ms( 500 );
		time += 5;

		intensity += ( 123 * 5 );
		if( time > 1000)
		{
			time = 0;
		}
		if( intensity > 12300)
		{
			intensity = 0;
		}
	}*/
	// ------------------------------

	
	
	// ------------------------------
	// Test Motor Drivers
/*	while(1)
	{
       moveMotors(100,100);
       delay_ms( 10 );
       moveMotors(0,0);
       delay_ms( 10 );
	}*/
	// ------------------------------


	
	// ------------------------------
	// Test Laser Driver

	// Simulated Payload
	uint8_t burn_cmd_payload[4];
	burn_cmd_payload[3] = 16;
	burn_cmd_payload[2] = 32;
	burn_cmd_payload[1] = 192;
	burn_cmd_payload[0] = 0;

	// Wait for Button 1 Press
	/*while( 1 )
	{
		button_pressed1 = FALSE;
		button_pressed2 = FALSE;

		// Wait till button 1.2 pressed to turn on
		while( button_pressed1 == FALSE )
		{
			if( ( P1IN & BUTTON1 ) == 0 )
			{
				delay_ms( 20 );
				if( ( P1IN & BUTTON1 ) == 0 )
				{
					button_pressed1 = TRUE;
				}
			}
		}*/



		// Start simulating commands (stop if button 2.1 pressed)
		/*while( button_pressed2 == FALSE )
		{
			enable_laser();
			i = 0;


			while( button_pressed2 == FALSE && i < 4 )
			{
				j = 0;
				while( button_pressed2 == FALSE && j < 1 )
				{
					respond_to_burn_cmd( burn_cmd_payload );
					j++;

					if( ( P2IN & BUTTON2 ) == 0 )
					{
						delay_ms( 20 );
						if( ( P2IN & BUTTON2 ) == 0 )
						{
							button_pressed2 = TRUE;
						}
					}

					delay_ms( 300 );
				}

				burn_cmd_payload[3] += 0x08;
				if( ( burn_cmd_payload[3] & 0x07 ) == 0x07 )
				{
					burn_cmd_payload[3] &= 0xF8;
				}
				burn_cmd_payload[3] += 0x01;

				i++;
			}


			/*for( i = 0; i < 100; i++ )
			{
				turn_on_laser_timed( i * 123, 300 );
			}*/

			/*turn_on_laser_timed( MAX_INTENSITY,   1000 );
			turn_on_laser_timed( INTENSITY_3,     1000 );
			turn_on_laser_timed( INTENSITY_2,     1000 );
			turn_on_laser_timed( INTENSITY_1,     1000 );
			turn_on_laser_timed( FOCUS_INTENSITY, 1000 );*//*

			disable_laser();
			//delay_ms( 1000 );
		}
	}*/
	// ------------------------------



	// ------------------------------
	// Main loop
	while( 1 )
	{
		struct TPacket_Data rx_data;
		check_and_respond_to_msg( &rx_data );

		if( picture_ip == TRUE )
		{
			if( burn_ready == TRUE ) { respond_to_burn_cmd( rx_data.data ); }	
		}
		
	}
	// ------------------------------


	return 0;
}
//============================================================================



// Test uart code A
/*if( packet_ready == 1 )
{
	uint8_t rx_packet[MAX_PACKET_LENGTH];

	struct TPacket_Data rx_data;
	rx_data.data_size = 4;
	uint16_t rec_size = uart_getp( rx_packet, MAX_PACKET_LENGTH );
	if( parse_rx_packet( rx_packet, MAX_PACKET_LENGTH, &rx_data ) == 0 )
	{
		struct TPacket_Data tx_data;
		tx_data.command = rx_data.command;
		tx_data.ack = ACK_MSG;
		tx_data.data_size = 0;

		uint8_t tx_buff[MAX_PACKET_LENGTH];
		uint16_t tx_length = pack_tx_packet( tx_data, tx_buff );
		uart_putp( tx_buff, tx_length );
	}
	else
	{
		uart_putc( 'e' );
	}
}*/
		

// Test uart code B
/*
    uart_puts((char *)"This is a Test of the MSP-430\n\r");

    uart_puts((char *)"PRESS any key to start the test... ");


    uint8_t c = uart_getc();

    uart_putc(c);

    uart_puts((char *)"\n\rTest OK!\n\r");

    for( i = 0; i < 256; i++ )
    {
    	//uart_putc(48 + (i %10));
    	//uart_putc('.');
    	//uart_putc(i);
    	//uart_putc('|');

    	//uint8_t send_msg[3];
    	//send_msg[0] = i;
    	//send_msg[1] = ETX;
    	//if( i == ETX || i == ESC )
    	//{
    	//	send_msg[0] = 0x1B;
    	//	send_msg[1] = i;
    	//	send_msg[2] = 0x03;
    	//}


    	uint8_t rec_packet[1024];
    	uint16_t size = uart_getp( rec_packet, 1024 );
    	//uart_putp( &size, 1/*size*//* );
    	uart_putp( rec_packet, size );



    	//c = uart_getc( );
    	//uint8_t c2 = uart_getc( );
    	//uint8_t c3 = uart_getc( );
    	//uint8_t c4 = uart_getc( );
		//uart_putc( c );
		//uart_putc( c2 );
		//uart_putc( c3 );
		//uart_putc( c4 );

		if( c == 0x1B && c2 == 8 && c3 == ETX && c4 == 0x04 ) { P1OUT |= DEBUG_LED; }


    	//uint8_t rec_packet[1024];
    	//uint16_t size = uart_getp( rec_packet, 1024 );
    	//uart_putp( rec_packet, size );
    	//uart_puts((char *)"\n\r");
    }

    //volatile unsigned long i;
    while(1)
    {
    	uart_puts((char *)"\n\rEnter 3 Chars, then a String of 8 Chars...\n\r");

    	i = 500000; // Delay to Test the FIFO

    	while (i != 0)
    	{
    		i--;
    	}
    	uart_putc(uart_getc());
    	uart_putc(uart_getc());
    	uart_putc(uart_getc());
    	uart_gets(test_string,8);
    	uart_putp(test_string, 8);
    }
 */
//============================================================================

////////////////////////////////////////////////////////////////////////////////

