#include <msp430.h>
#include "drivers/gmt020-02.h"
#include "drivers/fonts.h"

void ST7789_init()
{
    // Tell the ST7789 about the display and whatnot

    ST7789_start_write_command();

    // Reset LCD, hold for 10ns as stated in 7.4.5
    P1OUT &= ~RST;
    __delay_cycles(1000);
    P1OUT |= RST;
    __delay_cycles(1000);

    ST7789_start_write_command();
    ST7789_write(ST7789_SWRST);
    ST7789_write(ST7789_SLPOUT);

    ST7789_write(ST7789_COLMOD); // 0xBE 0x77
    ST7789_start_write_data();
    ST7789_write(0x55);

    ST7789_start_write_command();
    ST7789_write(ST7789_MADCTL);
    ST7789_start_write_data();
    ST7789_write(0x00);

    // Display is 240 x 320
    // 239 => 0x00EF
    // 319 => 0x013F
    ST7789_start_write_command();
    ST7789_write(ST7789_CASET);
    ST7789_start_write_data();
    ST7789_write_16bit(ST7789_XSTART);
    // 239
    ST7789_write_16bit(0x00EF);

    ST7789_start_write_command();
    ST7789_write(ST7789_RASET);
    ST7789_start_write_data();
    ST7789_write_16bit(ST7789_YSTART);
    ST7789_write_16bit(0x013F);

    ST7789_start_write_command();
    ST7789_write(ST7789_INVON);
    ST7789_write(ST7789_NORON);


    ST7789_write(ST7789_DISPON);
    ST7789_end_write();
}

void ST7789_write(uint8_t data)
{
    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = data;
    while (UCB0STAT & UCBUSY);
}

void ST7789_start_write_data()
{
    P1OUT &= ~CS;
    P1OUT |= DC;
}

void ST7789_start_write_command()
{
    P1OUT &= ~CS;
    P1OUT &= ~DC;
}

void ST7789_end_write()
{
    P1OUT |= CS;
}

void ST7789_write_16bit(uint16_t data)
{
    ST7789_write(data >> 8);
    ST7789_write(data & 0xFF);
}

void ST7789_set_addr(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
    // Tell what LCD it is writing to
    ST7789_start_write_command();
    ST7789_write(ST7789_CASET);
    ST7789_start_write_data();
    ST7789_write_16bit(x0 + ST7789_XSTART);
    ST7789_write_16bit(x1 + ST7789_XSTART);

    ST7789_start_write_command();
    ST7789_write(ST7789_RASET);
    ST7789_start_write_data();
    ST7789_write_16bit(y0 + ST7789_YSTART);
    ST7789_write_16bit(y1 + ST7789_YSTART);

    ST7789_start_write_command();
    ST7789_write(ST7789_RAMWR);
    ST7789_end_write();
}

void ST7789_draw_pixel(uint16_t x, uint16_t y, uint16_t colour)
{
    ST7789_set_addr(x, x+1, y, y+1);
    ST7789_start_write_data();
    ST7789_write_16bit(colour);
    ST7789_end_write();
}


void ST7789_draw_char(uint16_t x, uint16_t y, uint16_t colour, char letter)
{
    if (letter < 32 || letter > 127) return;

    int col, row;

    for (col = 0; col < 5; col++) {
        uint8_t line = ascii[letter - 32][col];

        for (row = 0; row < 8; row++) {
            if (line & (1 << row)) {
                ST7789_draw_pixel(x + col, y + row, colour);
            }
        }
    }
}

void ST7789_clear_screen(uint16_t colour)
{
    uint32_t i;
    ST7789_set_addr(0, LCD_WIDTH-1, 0, LCD_HEIGHT-1);

    ST7789_start_write_data();
    for(i = LCD_SIZE; i > 0; i--){
        ST7789_write_16bit(colour);
    }
    ST7789_end_write();
}

void ST7789_draw_string(uint16_t x, uint16_t y, uint16_t colour, char* string)
{
    int i;
    uint16_t x_pos = x;
    uint16_t y_pos = y;

    for (i = 0; string[i] != '\0'; i++) {
        if(x_pos + 6 > LCD_WIDTH){
            y_pos += 8;
            x_pos = 0;

            if(y_pos + 8 > LCD_HEIGHT){
                y_pos = 0;
            }

        }

        ST7789_draw_char(x_pos, y_pos, colour, string[i]);
        x_pos += 6;
    }
}

uint16_t get_color_code(const char* color_name) {
    if (strcmp(color_name, "BLACK") == 0) return 0x0000;
    else if (strcmp(color_name, "WHITE") == 0) return 0xFFFF;
    else if (strcmp(color_name, "RED") == 0) return 0xF800;
    else if (strcmp(color_name, "GREEN") == 0) return 0x07E0;
    else if (strcmp(color_name, "BLUE") == 0) return 0x001F;
    else if (strcmp(color_name, "CYAN") == 0) return 0x07FF;
    else if (strcmp(color_name, "YELLOW") == 0) return 0xFFC0;
    else if (strcmp(color_name, "PURPLE") == 0) return 0xF81F;
    else if (strcmp(color_name, "PINK") == 0) return 0xD252;
    else return 0x0000;  // Default to BLACK if unknown
}
