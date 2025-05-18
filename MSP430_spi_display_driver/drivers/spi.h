#ifndef DRIVERS_SPI_H_
#define DRIVERS_SPI_H_

#include <stdint.h>
#include <stdbool.h>

// SPI GPIO definitions
#define SCLK BIT5
#define MOSI BIT7
#define CS   BIT0
#define RST  BIT6
#define DC   BIT3

void spi_init(void);
void msp430_init(void);
void toggle_led(void);

#endif /* DRIVERS_SPI_H_ */
