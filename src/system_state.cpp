#include <Arduino.h>
#include "system_state.h"
#include "rfid.h"
#include "lcd.h"
#include "config.h"
#include "bme280_simple.h" // อย่าลืม include bme280

static system_state_t current_state = STATE_IDLE;
static unsigned long state_timer = 0;
static unsigned long bme_update_timer = 0; // เพิ่มตัวจับเวลาสำหรับอัปเดตหน้าจอ BME

// ฟังก์ชันสำหรับอ่านและแสดงผล BME280 (ทั้งขึ้นจอและลง Serial)
static void display_bme_screen() {
    float t, h, p;
    bme280_read(t, h, p);

    // 1. ส่งข้อมูลออก Serial Monitor
    Serial.printf("[BME280] Temp: %.1f C | Hum: %.1f %% | Pres: %.1f hPa\n", t, h, p);

    // 2. แสดงผลบนจอ LCD (สมมติว่าเป็นจอ 16x2)
    char line1[17];
    char line2[17];
    // จัดฟอร์แมตข้อความให้อยู่ใน 16 ตัวอักษร (ESP32 รองรับ %f ใน sprintf)
    snprintf(line1, sizeof(line1), "T:%.1fC H:%.1f%%", t, h);
    snprintf(line2, sizeof(line2), "P:%.1fhPa", p);

    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print(line1);
    lcd_setCursor(0, 1);
    lcd_print(line2);
}

void state_init() {
    // กำหนดค่าเริ่มต้นให้กับ Hardware
    lcd_init();
    rfid_init();
    
    // เริ่มต้น BME280 ที่ขา I2C (SDA=21, SCL=22)
    if (!bme280_begin(21, 22)) {
        Serial.println("Warning: BME280 not found! Check wiring.");
    } else {
        Serial.println("BME280 initialized successfully.");
    }
    
    // ตั้งสถานะเริ่มต้น
    current_state = STATE_IDLE;
    display_bme_screen(); // โชว์ค่าครั้งแรกทันที
    bme_update_timer = millis();
}

void state_update() {
    uint8_t uid[5];
    bool is_authorized = true;

    // ----- ตรวจสอบการอัปเดตหน้าจอ BME280 ตอนที่ระบบว่าง -----
    if (current_state == STATE_IDLE) {
        // อัปเดตหน้าจอทุกๆ 2 วินาที (2000 ms) จะได้ไม่กะพริบถี่เกินไป
        if (millis() - bme_update_timer > 2000) {
            display_bme_screen();
            bme_update_timer = millis();
        }
    }

    switch (current_state) {
        
        case STATE_IDLE:
            if (rfid_request()) {
                current_state = STATE_CARD_DETECTED;
            }
            break;

        case STATE_CARD_DETECTED:
            if (rfid_read_uid(uid)) {
                // --- เพิ่มการพิมพ์รหัสบัตรที่อ่านได้ออก Serial Monitor ---
                Serial.print("[RFID] Card Scanned UID: ");
                for (int i = 0; i < 4; i++) {
                    Serial.printf("%02X ", uid[i]); // พิมพ์เป็นเลขฐาน 16 (HEX)
                }
                Serial.println();

                // ตรวจสอบความถูกต้องของบัตร
                for (int i = 0; i < 4; i++) {
                    if (uid[i] != AUTHORIZED_UID[i]) {
                        is_authorized = false;
                        break;
                    }
                }

                if (is_authorized) {
                    Serial.println("[SYSTEM] Access Granted!");
                    current_state = STATE_UNLOCK;
                    lcd_clear();
                    lcd_setCursor(0, 0);
                    lcd_print("Access Granted!");
                    lcd_setCursor(0, 1);
                    lcd_print("Unlocking...");
                } else {
                    Serial.println("[SYSTEM] Access Denied - Unknown Card");
                    current_state = STATE_ERROR;
                    lcd_clear();
                    lcd_setCursor(0, 0);
                    lcd_print("Access Denied");
                    lcd_setCursor(0, 1);
                    lcd_print("Unknown Card");
                }
            } else {
                Serial.println("[RFID] Read Error!");
                current_state = STATE_ERROR;
                lcd_clear();
                lcd_setCursor(0, 0);
                lcd_print("Read Error");
                lcd_setCursor(0, 1);
                lcd_print("Please try again");
            }
            
            state_timer = millis();
            break;

        case STATE_UNLOCK:
            if (millis() - state_timer > 3000) {
                current_state = STATE_IDLE;
                display_bme_screen(); // กลับมาโชว์ค่า BME ทันที
                bme_update_timer = millis(); // รีเซ็ตเวลาให้เริ่มนับใหม่
            }
            break;

        case STATE_ERROR:
            if (millis() - state_timer > 2000) {
                current_state = STATE_IDLE;
                display_bme_screen(); // กลับมาโชว์ค่า BME ทันที
                bme_update_timer = millis();
            }
            break;
    }
}