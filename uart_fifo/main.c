//#include <msp430g2553.h>
#include "uart_fifo.h"
#include <stdio.h>
#include "msp430f5529.h"

char test_string[8];
#define LED BIT0

int test;
extern volatile unsigned char packet_ready;
extern volatile unsigned char burn_ready;

int main(void)
{
	setup_clocks();
    uart_init();						//Initialize the UART connection



	burn_ready = 0;


	while( 1 )
	{
		struct TPacket_Data rx_data;
		check_and_respond_to_msg( &rx_data );

		if( burn_ready == 1 ) { respond_to_burn_cmd( rx_data.data ); }

		/*if( packet_ready == 1 )
		{
			unsigned char rx_packet[MAX_PACKET_LENGTH];

			struct TPacket_Data rx_data;
			rx_data.data_size = 4;
			unsigned int rec_size = uart_getp( rx_packet, MAX_PACKET_LENGTH );
			if( parse_rx_packet( rx_packet, MAX_PACKET_LENGTH, &rx_data ) == 0 )
			{
				struct TPacket_Data tx_data;
				tx_data.command = rx_data.command;
				tx_data.ack = CMD_ACK;
				tx_data.data_size = 0;

				unsigned char tx_buff[MAX_PACKET_LENGTH];
				unsigned int tx_length = pack_tx_packet( tx_data, tx_buff );
				uart_putp( tx_buff, tx_length );
			}
			else
			{
				uart_putc( 'e' );
			}
		}*/
	}

	return 0;
}




// Test uart code
/*
    uart_puts((char *)"This is a Test of the MSP-430\n\r");

    uart_puts((char *)"PRESS any key to start the test... ");


    unsigned char c = uart_getc();

    uart_putc(c);

    uart_puts((char *)"\n\rTest OK!\n\r");

    for( i = 0; i < 256; i++ )
    {
    	//uart_putc(48 + (i %10));
    	//uart_putc('.');
    	//uart_putc(i);
    	//uart_putc('|');

    	//unsigned char send_msg[3];
    	//send_msg[0] = i;
    	//send_msg[1] = ETX;
    	//if( i == ETX || i == ESC )
    	//{
    	//	send_msg[0] = 0x1B;
    	//	send_msg[1] = i;
    	//	send_msg[2] = 0x03;
    	//}


    	unsigned char rec_packet[1024];
    	unsigned int size = uart_getp( rec_packet, 1024 );
    	//uart_putp( &size, 1/*size*//* );
    	uart_putp( rec_packet, size );



    	//c = uart_getc( );
    	//unsigned char c2 = uart_getc( );
    	//unsigned char c3 = uart_getc( );
    	//unsigned char c4 = uart_getc( );
		//uart_putc( c );
		//uart_putc( c2 );
		//uart_putc( c3 );
		//uart_putc( c4 );

		if( c == 0x1B && c2 == 8 && c3 == ETX && c4 == 0x04 ) { P1OUT |= LED; }


    	//unsigned char rec_packet[1024];
    	//unsigned int size = uart_getp( rec_packet, 1024 );
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
