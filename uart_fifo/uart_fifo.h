#ifndef  UART_FIFO_H
#define  UART_FIFO_H


#define LED BIT0
#define RXD BIT1
#define TXD BIT2


#define UCA1_OS   1    // 1 = oversampling mode, 0 = low-freq mode
#define UCA1_BR0  17   // Value of UCA1BR0 register
#define UCA1_BR1  0    // Value of UCA1BR1 register
#define UCA1_BRS  0    // Value of UCBRS field in UCA1MCTL register
#define UCA1_BRF  6    // Value of UCBRF field in UCA1MCTL register

#define ETX		  	0X03
#define STX			0x02
#define ESC			0x1B
#define CMD_ACK		0x06
#define CMD_NAK		0X3F
#define NEW_CMD		0X00
#define CMD_BURN	0x0B
#define CMD_READY	0x4D

#define CMD_BURN_PAYLOAD_SIZE		4
#define CMD_READY_PAYLOAD_SIZE		0

#define CMD_BURN_RESPONSE_SIZE		0
#define CMD_READY_RESPONSE_SIZE		0

#define MAX_DATA_SIZE		4
#define MIN_PACKET_LENGTH	3
#define MAX_PACKET_LENGTH	3 + 2 * ( MAX_DATA_SIZE + 1 )
#define FIFO_SIZE 			128
#define MAX_ATTEMPTS		3


struct TPacket_Data
{
	unsigned char ack;
	unsigned char command;
	unsigned char data[MAX_DATA_SIZE];
	unsigned char data_size;
};



/*rx_flag
* This flag is to be used by other modules to check and see if a new transmission has happened.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile unsigned int rx_flag;

/*rx_fifo_full
* This flag is to be used by other modules to check and see if the rx fifo is full.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile unsigned int rx_fifo_full;

/*tx_fifo_full
* This flag is to be used by other modules to check and see if the rx fifo is full.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile unsigned int tx_fifo_full;

void setup_clocks( void );
void uart_init( void );
unsigned char uart_getc();
void uart_gets( char* Array, int length );
unsigned int uart_getp( unsigned char* packet, int max_length );
void uart_putc( unsigned char c);
void uart_puts( char *str);
void uart_putp( unsigned char *packet, unsigned int length);

int parse_rx_packet( unsigned char *rx_buff, unsigned int length, struct TPacket_Data * rx_data );
unsigned int pack_tx_packet( struct TPacket_Data tx_data, unsigned char * tx_buff );

unsigned char calc_8bit_mod_checksum( unsigned char *data, unsigned int length );

void check_and_respond_to_msg( struct TPacket_Data * rx_data );
void send_ack( unsigned char command, unsigned char ack );
void respond_to_burn_cmd( unsigned char * burn_cmd_payload );

#endif // UART_FIFO_H
