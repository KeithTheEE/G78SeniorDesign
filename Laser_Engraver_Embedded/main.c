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
extern volatile uint8_t pi_init;
extern uint32_t time_ms;

////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	// ------------------------------
	// Variable Declaration
	int32_t i, j;
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
	initWaitTimer();
	init_laser();
    init_uart();
	initMotorIO();

	// homeLaser();
	// ------------------------------
	
	

	// ------------------------------
	// Timing Tests
	/*while( 1 )
	{
		for( i = 0; i < 1; i++ )
		{
			//delay_10us(100);
			delay_ms( 1 );
		}

		#ifdef DEBUG
			P1OUT |= DEBUG_LED;
		#else
			P3OUT |= PCB_LED;
		#endif

		for( i = 0; i < 1; i++ )
		{
			//delay_10us(100);
			delay_ms( 1 );
		}

		#ifdef DEBUG
			P1OUT &= ~DEBUG_LED;
		#else
			P3OUT &= ~PCB_LED;
		#endif
	}*/


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
	while(1)
	{
       moveMotors(6,6);
       delay_ms( 10 );
       moveMotors(0,0);
       delay_ms( 10 );
	}
	// ------------------------------

	
	
	// ------------------------------
	// Test picture values/spacing
	/*enable_laser();
	delay_ms( 10000 );

	for( j = 1; j < 11; j++ )
	{
		for( i = 1; i < 51; i++ )
		{
			moveMotors( i,2*j-1 );

			turn_on_laser_timed( 123 * 2 * (i), 10 * (j+1) );


			// if( i < 10 )
			// {
				// turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_4 );
			// }
			// else if( i < 20 )
			// {
				// turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_3 );
			// }
			// else if( i < 30 )
			// {
				// turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_2 );
			// }
			// else if( i < 40 )
			// {
				// turn_on_laser_timed( MAX_INTENSITY, LASER_DUR_1 );
			// }

			// Simulate communication time
			delay_ms( 1 );
		}

		for( i = 50; i > 0; i-- )
		{
			moveMotors(i,2*j);

			turn_on_laser_timed( 123 * 2 * (i), 10 * (j) );

			delay_ms( 1 );
		}
	}

	disable_laser();*/
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

		if( pi_init != FALSE )
		{
			if( pi_init == JUST_INITIALIZED )
			{
				// Indicate to the Pi that everything has been initialized
				//send_MSP_initialized();

				pi_init = TRUE;	// Pi is initialized, and ping has been sent
			}
			else
			{
				if( picture_ip == TRUE )
				{
					if( burn_ready == TRUE ) { respond_to_burn_cmd( rx_data.data ); }
				}
			}
		}
	}
	// ------------------------------


	return 0;
}
//============================================================================

////////////////////////////////////////////////////////////////////////////////

