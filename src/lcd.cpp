#include "lcd.h"
#include <Wire.h>
#include "config.h"

/* --- Internal Helper Functions --- */

/**
 * Sends a raw byte to the I2C expander chip (PCF8574).
 * Always ORs with LCD_BL to maintain the backlight state.
 */
static void lcd_write(uint8_t data) {
    Wire.beginTransmission(LCD_ADDR);
    Wire.write(data | LCD_BL); 
    Wire.endTransmission();
}

/**
 * Latches the data into the LCD by pulsing the Enable (EN) pin.
 */
static void lcd_pulse(uint8_t data) {
    lcd_write(data | LCD_EN);      // Rise
    delayMicroseconds(1);          
    lcd_write(data & ~LCD_EN);     // Fall
    delayMicroseconds(50);         // Command execution time
}

/**
 * Splits an 8-bit byte into two 4-bit nibbles for transmission.
 * The High nibble is sent first, followed by the Low nibble.
 */
static void lcd_send(uint8_t data, uint8_t mode) {
    uint8_t high = (data & LCD_HIGH_NIBBLE_MASK) | mode;
    uint8_t low  = ((data << LCD_NIBBLE_SHIFT) & LCD_HIGH_NIBBLE_MASK) | mode;

    lcd_write(high);
    lcd_pulse(high);

    lcd_write(low);
    lcd_pulse(low);
}

static void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0);
}

/* --- Public API Implementation --- */

void lcd_write_char(char c) {
    lcd_send(c, LCD_RS); 
}

void lcd_init() {
    Wire.begin(SDA, SCL);
    delay(50); 

    // Standard HD44780 4-bit startup sequence
    lcd_command(LCD_INIT_1);
    lcd_command(LCD_INIT_2);
    lcd_command(LCD_FUNCTION_SET);
    lcd_command(LCD_DISPLAY_ON);
    lcd_command(LCD_ENTRY_MODE);
    lcd_command(LCD_CLEAR);

    delay(5); 
}

void lcd_clear() {
    lcd_command(LCD_CLEAR);
    delay(2); 
}

void lcd_setCursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {LCD_ROW0_OFFSET, LCD_ROW1_OFFSET};
    lcd_command(LCD_SET_DDRAM_ADDR | (col + row_offsets[row]));
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_write_char(*str++);
    }
}