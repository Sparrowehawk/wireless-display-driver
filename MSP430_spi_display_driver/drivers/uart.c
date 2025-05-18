//#include <msp430.h>
//
//void uart_init()
//{
//    P1SEL  |= BIT1 + BIT2;  // P1.1 = RXD, P1.2 = TXD
//    P1SEL2 |= BIT1 + BIT2;
//
//    UCA0CTL1 |= UCSSEL_2;   // SMCLK
//    UCA0BR0 = 8;            // 1MHz 115200 baud rate (see UG)
//    UCA0BR1 = 0;
//    UCA0MCTL = UCBRS2 + UCBRS0; // Modulation
//    UCA0CTL1 &= ~UCSWRST;   // Initialize USCI
//}
//
//void rx_data()
//{
//
//    uart_init();
//    gpio_init();
//
//    while (1)
//    {
//        if (IFG2 & UCA0RXIFG)  // If RX buffer has data
//        {
//            volatile unsigned char received = UCA0RXBUF; // Read byte
//            P2OUT ^= BIT1;  // Toggle LED
//            __delay_cycles(100000); // Small delay for visibility
//        }
//    }
//}
