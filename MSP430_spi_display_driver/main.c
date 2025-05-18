#include <msp430.h> 
#include <string.h>
#include <stdio.h>
#include "drivers/spi.h"
#include "drivers/gmt020-02.h"

#define MAX_LEN 256

void uart_init(void)
{
    P1SEL  |= BIT1 + BIT2;  // P1.1 = RXD, P1.2 = TXD
    P1SEL2 |= BIT1 + BIT2;

    UCA0CTL1 |= UCSSEL_2;   // SMCLK
    UCA0BR0 = 8;            // 1MHz 115200 baud rate
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS2 + UCBRS0;
    UCA0CTL1 &= ~UCSWRST;
}

int main(void)
{
    unsigned int idx = 0;
    char buffer[MAX_LEN];
    char tempCheck = ' ';
    int startFlag = 0;

    msp430_init();
    spi_init();
    ST7789_init();
    uart_init();

    P2OUT &= ~BIT1;

    while (1)
    {
        if (IFG2 & UCA0RXIFG)  // If RX buffer has data
        {
            char received = UCA0RXBUF;

            // Detect end condition: ";;" or '\n'
            if ((received == ';' && tempCheck == ';') || received == '\n' || idx >= MAX_LEN - 1)
            {
                if (startFlag)                                                  // Only process buffer if startFlag was set
                {
                    buffer[idx-1] = '\0';                                       // Null-terminate the string and the 2nd last component (';')

                    char colour[6];                                             // Pre-allocating space needed
                    int mode = 0;
                    int x = 0;
                    int y = 0;
                    char msg[200];

                    char* token = strtok(buffer, "|");
                    int field = 0;

                    while (token != NULL) {                                     // Splits the string into each token by '|' and assigns via index
                        switch (field) {
                            case 0:
                                strncpy(colour, token, sizeof(colour));
                                break;
                            case 1:
                                mode = atoi(token);
                                break;
                            case 2:
                                x = atoi(token);
                                break;
                            case 3:
                                y = atoi(token);
                                break;
                            case 4:
                                strncpy(msg, token, sizeof(msg));
                                msg[sizeof(msg) - 1] = '\0';                    // Ensure null termination
                                break;
                        }

                        field++;
                        token = strtok(NULL, "|");
                    }

                    uint16_t colour_code = get_color_code(colour);              // Colour parsed is char*, needs to be in uint16_t

                if(mode == 1){                                                  // If the mode selected is fill screen or text
                        ST7789_clear_screen(colour_code);
                    } else {
                        ST7789_draw_string(x, y, colour_code, msg);
                    }

                    P2OUT |= BIT1;                                              // Turn LED on (sanity check)
                }
                idx = 0;
                startFlag = 0;                                                  // Reset for next message
            }
            else
            {
                if (!startFlag)
                {
                    if (received == '|' && tempCheck == '|')                    // Checks for starting condition of ||
                    {
                        startFlag = 1;
                        idx = 0;                                                // Clear buffer
                    }
                }
                else
                {
                    if(received == '+'){                                        // From the ESP32, ' ' is a '+'
                        buffer[idx++] = ' ';
                    } else {
                        buffer[idx++] = received;
                    }
                }
            }

            tempCheck = received;                                               // Always update for next comparison
        }
    }
}
