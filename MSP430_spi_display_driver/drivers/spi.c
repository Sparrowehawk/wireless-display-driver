#include <msp430.h>
#include "drivers/spi.h"

void spi_init()
{

    UCB0CTL1 |= UCSWRST;                  // Put USCI in reset
    UCB0CTL1 |= UCSSEL_2;                 // Select SMCLK as the clock source

    UCB0CTL0 = UCMST + UCMODE_0 + UCSYNC + UCMSB + UCCKPL;

    UCB0BR0 = 8;
    UCB0BR1 = 0;

    // GPIO init
    P1DIR |= CS + RST + DC;
    P1OUT |= CS;
    P1OUT |= RST;
    P1OUT |= DC;

    // SPI init
    P1SEL  |= MOSI + SCLK;
    P1SEL2 |= MOSI + SCLK;

    UCB0CTL1 &= ~UCSWRST;      // Release from reset, enabling SPI
}

void msp430_init()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    BCSCTL1 = CALBC1_1MHZ;      // Set freq to 1MHz
    DCOCTL = CALDCO_1MHZ;

    P2DIR |= BIT1;
    P2OUT &= ~BIT1;
}

void toggle_led()
{
    P2OUT ^= BIT1;
}
