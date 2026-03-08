/**
 * @file bme280_simple.h
 * @author Your Name
 * @brief Lightweight library for the BME280 Temperature, Humidity, and Pressure sensor.
 * @version 1.0
 * * This header defines the I2C interface and simplified functions to interact with 
 * the Bosch BME280 sensor using the Arduino Wire library.
 */

#ifndef BME280_SIMPLE_H
#define BME280_SIMPLE_H

#include <Arduino.h>
#include <Wire.h>

/** * @brief Default I2C Address for BME280.
 * @note 0x76 if SDO is connected to GND (Common for many breakout boards).
 * @note 0x77 if SDO is connected to VCC.
 */
#define BME280_ADDR 0x76

/** * @brief Initializes the BME280 sensor with specific I2C pins.
 * @param sda GPIO pin number for I2C Serial Data.
 * @param scl GPIO pin number for I2C Serial Clock.
 * @return true if initialization and communication were successful.
 * @return false if the sensor was not detected.
 */
bool bme280_begin(int sda, int scl);

/** * @brief Reads all environmental data from the sensor in one call.
 * @param[out] temp Reference to store Temperature in Celsius (°C).
 * @param[out] hum  Reference to store Relative Humidity percentage (%).
 * @param[out] pres Reference to store Atmospheric Pressure in Hectopascals (hPa).
 */
void bme280_read(float &temp, float &hum, float &pres);

/** * @name Internal Low-Level Functions
 * These functions handle direct register access over the I2C bus.
 * @{ 
 */

/** * @brief Writes a single byte of data to a specific sensor register.
 * @param reg The register address to write to.
 * @param val The 8-bit value to be stored in the register.
 */
void bme280_write_reg(uint8_t reg, uint8_t val);

/** * @brief Reads a sequence of bytes from the sensor registers.
 * @param reg The starting register address.
 * @param buffer Pointer to the array where the read data will be stored.
 * @param len Number of bytes to read.
 */
void bme280_read_regs(uint8_t reg, uint8_t *buffer, uint8_t len);

/** @} */

#endif // BME280_SIMPLE_H