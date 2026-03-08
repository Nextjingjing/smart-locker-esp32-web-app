#ifndef WEB_CLIENT_H
#define WEB_CLIENT_H

#include <Arduino.h>

/**
 * @brief Initializes WiFi connection using SSID/PASS defined in config.h.
 * @return true if connection is successful, false otherwise.
 */
bool wifi_init();

/**
 * @brief Sends BME280 sensor readings (temperature, humidity, pressure) to the Go Server.
 * @param temp Temperature value in Celsius.
 * @param hum Relative humidity percentage.
 * @param pres Atmospheric pressure in hPa.
 */
void send_environment_data(float temp, float hum, float pres);

/**
 * @brief Reports RFID card tap events and door status to the Go Server.
 * @param status A string representing the door or card event (e.g., "AUTHORIZED", "DENIED").
 */
void send_door_data(String status);

#endif