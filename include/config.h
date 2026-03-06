#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pins mapping for ESP32 to MFRC522
#define RST_PIN   27
#define SS_PIN    5
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23

#define BAUD_RATE 115200

#endif // CONFIG_H