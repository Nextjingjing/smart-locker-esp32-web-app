#ifndef BME280_SIMPLE_H
#define BME280_SIMPLE_H

#include <Arduino.h>
#include <Wire.h>

// I2C Address (SDO -> GND = 0x76)
#define BME280_ADDR 0x76

// ฟังก์ชันหลัก
bool bme280_begin(int sda, int scl);
void bme280_read(float &temp, float &hum, float &pres);

// ฟังก์ชันเสริม (Internal Use)
void bme280_write_reg(uint8_t reg, uint8_t val);
void bme280_read_regs(uint8_t reg, uint8_t *buffer, uint8_t len);

#endif