/**
 * @file lcd.h
 * @author Your Name
 * @brief I2C Character LCD Driver for HD44780-compatible displays.
 * @details This library manages 4-bit data communication over I2C, handling
 * initialization, cursor positioning, and data transmission.
 */

#ifndef LCD_H
#define LCD_H

#include <Arduino.h>

/** @brief Default I2C Address for the LCD backpack (PCF8574). */
#define LCD_ADDR  0x27

/** * @name I2C Expansion Port Bitmasks
 * Mapping of the PCF8574 output pins to the LCD controller pins.
 * @{ 
 */
#define LCD_RS 0x01  ///< Register Select: 0 = Command, 1 = Data
#define LCD_EN 0x04  ///< Enable bit: Triggers the data latch on falling edge
#define LCD_BL 0x08  ///< Backlight control bit: 1 = On, 0 = Off
/** @} */

/** * @name LCD Command Set
 * Standard HD44780 instructions for display initialization and control.
 * @{ 
 */
#define LCD_INIT_1       0x33  ///< Initial magic sequence part 1
#define LCD_INIT_2       0x32  ///< Initial magic sequence part 2 (Set to 4-bit mode)
#define LCD_FUNCTION_SET 0x28  ///< 2 lines, 5x8 dot matrix
#define LCD_DISPLAY_ON   0x0C  ///< Display ON, Cursor OFF, Blink OFF
#define LCD_ENTRY_MODE   0x06  ///< Increment cursor, No display shift
#define LCD_CLEAR        0x01  ///< Clear display and return cursor to home
/** @} */

/** @brief Base command to set the Display Data RAM (DDRAM) address. */
#define LCD_SET_DDRAM_ADDR 0x80

/** * @name Row Memory Offsets
 * Memory addresses for the start of each row in DDRAM.
 * @{ 
 */
#define LCD_ROW0_OFFSET 0x00  ///< Starting address of the first row
#define LCD_ROW1_OFFSET 0x40  ///< Starting address of the second row
/** @} */

/** * @name Nibble Manipulation
 * Utilities for 4-bit data transmission over an 8-bit I2C bus.
 */
#define LCD_HIGH_NIBBLE_MASK 0xF0  ///< Mask to extract the upper 4 bits
#define LCD_NIBBLE_SHIFT     4     ///< Shift value for nibble alignment

/**
 * @brief Initializes the LCD over I2C.
 * Configures 4-bit mode, clears the display, and turns on the backlight.
 */
void lcd_init();

/**
 * @brief Clears all text from the screen and resets the cursor to (0,0).
 */
void lcd_clear();

/**
 * @brief Moves the cursor to a specific coordinate.
 * @param col Column index (starting at 0).
 * @param row Row index (0 for the first row, 1 for the second).
 */
void lcd_setCursor(uint8_t col, uint8_t row);

/**
 * @brief Prints a null-terminated string to the current cursor position.
 * @param str Pointer to the character array (string) to display.
 */
void lcd_print(const char *str);

/**
 * @brief Writes a single ASCII character to the LCD.
 * @param c The character to be displayed.
 */
void lcd_write_char(char c);

#endif // LCD_H