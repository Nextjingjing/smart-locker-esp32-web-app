/**
 * @file config.h
 * @author Your Name
 * @brief Configuration file for ESP32 with MFRC522 RFID and I2C LCD.
 * @version 1.0
 * * This file contains pin mappings for SPI and I2C interfaces, 
 * serial communication settings, and security credentials.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/** * @section RFID_PINS MFRC522 Pin Mapping (SPI)
 * Connections between the ESP32 and the MFRC522 RFID module.
 * @{ 
 */
#define RST_PIN   27  ///< Reset pin for the RFID module
#define SS_PIN    5   ///< Slave Select (SDA) pin for SPI communication
#define SCK_PIN   18  ///< Serial Clock pin
#define MISO_PIN  19  ///< Master In Slave Out pin
#define MOSI_PIN  23  ///< Master Out Slave In pin
/** @} */

/** * @section LCD_PINS LCD I2C Pin Mapping
 * Standard I2C pins for the ESP32 used to interface with the LCD backpack.
 * @{ 
 */
#define SDA       21  ///< Serial Data line
#define SCL       22  ///< Serial Clock line
/** @} */

/** * @brief Serial Baud Rate
 * Data transmission speed for the Serial Monitor.
 */
#define BAUD_RATE 115200

/** * @brief Authorized RFID Tag UID
 * Unique Identifier for the card allowed to trigger access.
 * @note Format: {Byte1, Byte2, Byte3, Byte4}
 */
const uint8_t AUTHORIZED_UID[4] = {0x35, 0x85, 0x4E, 0x06};

// Wi-Fi Settings
#define WIFI_SSID "Pruek"
#define WIFI_PASS "065xxxxxxxxxxxx"

#define API_ENV_ENDPOINT  "http://192.168.1.75:3000/data/environment"
#define API_DOOR_ENDPOINT "http://192.168.1.75:3000/data/door"

#define API_KEY "xxxxxxxxxxxxxxxxxxxxx"

#endif // CONFIG_H