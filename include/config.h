#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pins mapping for ESP32 to MFRC522
#define RST_PIN   27
#define SS_PIN    5
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23

// LCD I2C
#define LCD_ADDR  0x27
#define LCD_SDA   21
#define LCD_SCL   22

#define BAUD_RATE 115200

// uid
const uint8_t AUTHORIZED_UID[4] = {0x35, 0x85, 0x4E, 0x06};

#endif // CONFIG_H