### Wireless display driver
WiFi &amp; UART connection for GMT020-02 using an ESP32 to host a webserver connected to a MSP430 driving the display. The display driver is an extension of previous work: https://github.com/Sparrowehawk/ST778_MSP430_driver

## Project Directory
wireless-display-driver/
├── esp32/ # ESP32 firmware using ESP-IDF
│ ├── main/
│ │ ├── web_interface.c # HTTP GET handler, string parsing logic
│ │ └── uart_comm.c # UART communication with MSP430
│ ├── CMakeLists.txt
│ └── sdkconfig # ESP-IDF config (e.g., Wi-Fi, UART)
│
├── msp430/ # MSP430 firmware using Code Composer Studio
│ ├── main.c # UART receiver, ST7789 rendering logic
│ ├── gmt020-02.c # ST7789 driver implementation
│ ├── gmt020-02.h # ST7789 function declarations & color macros
│ ├── spi.c # SPI initialization for MSP430
│ └── uart.c # UART initialization for MSP430
│
│
├── README.md # This file
└── LICENSE

## Parts used
- ESP32
- MSP430G2553
- GTM020-02 LCD display
- Phone (or any device to connect to the webserver)

## System overview
- ESP32 hosts a web server locally that allows for users to put inputs through a web UI
- ESP32 communicates via UART to the MSP430
- MSP430 takes communication information and transaltes it for the the display via it's own drivers
