//#include <msp430g2553.h>
#include "uart_fifo.h"
#include <stdio.h>
#include "msp430f5529.h"

char test_string[8];
#define LED BIT0

int test;

int main(void)
{
	// Setup DCO (will be used as source to SMCLK, which in turn will be used as source to BRCLK)
	// TODO: move into own function (perhaps in clock file?)
	WDTCTL = WDTPW+WDTHOLD;                 // Stop WDT
	UCSCTL3 |= SELREF_2;                    // Set DCO FLL reference = REFO
	UCSCTL4 |= SELA_2;                      // Set ACLK = REFO
	__bis_SR_register(SCG0);                // Disable the FLL control loop
	UCSCTL0 = 0x0000;                       // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_5;                    // Select DCO range 24MHz operation
	UCSCTL2 = FLLD_1 + 374;                	// Set DCO Multiplier for 12MHz
											// (N + 1) * FLLRef = Fdco
											// (374 + 1) * 32768 = 12.28MHz
											// Set FLL Div = fDCOCLK/2
	__bic_SR_register(SCG0);                  // Enable the FLL control loop



    uart_init();						//Initialize the UART connection

    __enable_interrupt();				//Interrupts Enabled

    // Delay (not exactly sure why necessary, but first few bytes are gibberish if not added)
    // TODO: timer delay
    volatile int i = 50000; // Delay to Test the FIFO
	while (i != 0)
	{
		i--;
	}

    uart_puts((char *)"This is a Test of the MSP-430\n\r");

    uart_puts((char *)"PRESS any key to start the test... ");


    unsigned char c = uart_getc();

    uart_putc(c);

    uart_puts((char *)"\n\rTest OK!\n\r");

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
    	uart_puts(test_string);
    }
}
