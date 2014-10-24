#ifndef  uart_fifo_h
#define uart_fifo_h


#define UCA1_OS   1    // 1 = oversampling mode, 0 = low-freq mode
#define UCA1_BR0  17   // Value of UCA1BR0 register
#define UCA1_BR1  0    // Value of UCA1BR1 register
#define UCA1_BRS  0    // Value of UCBRS field in UCA1MCTL register
#define UCA1_BRF  6    // Value of UCBRF field in UCA1MCTL register


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

/*uart_init
* Sets up the UART interface via USCI
* INPUT: None
* RETURN: None
*/
void uart_init(void);

/*uart_getc
* Get a char from the UART. Waits till it gets one
* INPUT: None
* RETURN: Char from UART
*/
unsigned char uart_getc();

/*uart_gets
* Get a string of known length from the UART. Strings terminate when enter is pressed or string buffer fills
* Will return when all the chars are received or a carriage return (\r) is received. Waits for the data.
* INPUT: Array pointer and length
* RETURN: None
*/
void uart_gets(char* Array, int length);

/*uart_putc
* Sends a char to the UART. Will wait if the UART is busy
* INPUT: Char to send
* RETURN: None
*/
void uart_putc(unsigned char c);

/*uart_puts
* Sends a string to the UART. Will wait if the UART is busy
* INPUT: Pointer to String to send
* RETURN: None
*/
void uart_puts(char *str);

#endif
