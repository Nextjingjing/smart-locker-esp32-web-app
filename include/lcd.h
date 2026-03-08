#ifndef LCD_H
#define LCD_H

#include <Arduino.h>

#define LCD_ADDR  0x27

#define LCD_RS 0x01
#define LCD_EN 0x04
#define LCD_BL 0x08

#define LCD_INIT_1 0x33
#define LCD_INIT_2 0x32
#define LCD_FUNCTION_SET 0x28
#define LCD_DISPLAY_ON 0x0C
#define LCD_ENTRY_MODE 0x06
#define LCD_CLEAR 0x01

// cursor command
#define LCD_SET_DDRAM_ADDR 0x80

// row offsets
#define LCD_ROW0_OFFSET 0x00
#define LCD_ROW1_OFFSET 0x40


#define LCD_HIGH_NIBBLE_MASK 0xF0
#define LCD_NIBBLE_SHIFT     4

void lcd_init();
void lcd_clear();
void lcd_setCursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);
void lcd_write_char(char c);

#endif