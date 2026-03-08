#ifndef WEB_CLIENT_H
#define WEB_CLIENT_H

#include <Arduino.h>

/**
 * @brief เชื่อมต่อ WiFi โดยใช้ SSID/PASS จาก config.h
 * @return true เมื่อเชื่อมต่อสำเร็จ
 */
bool wifi_init();

/**
 * @brief ส่งค่าเซนเซอร์ BME280 ไปยัง Go Server
 */
void send_environment_data(float temp, float hum, float pres);

/**
 * @brief ส่งสถานะการแตะบัตร RFID ไปยัง Go Server
 */
void send_door_data(String status);

#endif