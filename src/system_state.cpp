#include <Arduino.h>
#include "system_state.h"
#include "rfid.h"
#include "lcd.h"
#include "config.h"
#include "bme280_simple.h"

static system_state_t current_state = STATE_IDLE;
static unsigned long state_timer = 0;
static unsigned long bme_update_timer = 0;

/**
 * Reads BME280 data and updates both Serial and LCD.
 * Formats the floats into a 16-character string for the LCD rows.
 */
static void display_bme_screen() {
    float t, h, p;
    bme280_read(t, h, p);

    // Debug output to Serial
    Serial.printf("[BME280] Temp: %.1f C | Hum: %.1f %% | Pres: %.1f hPa\n", t, h, p);

    char line1[17];
    char line2[17];
    snprintf(line1, sizeof(line1), "T:%.1fC H:%.1f%%", t, h);
    snprintf(line2, sizeof(line2), "P:%.1fhPa", p);

    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print(line1);
    lcd_setCursor(0, 1);
    lcd_print(line2);
}

void state_init() {
    lcd_init();
    rfid_init();
    
    if (!bme280_begin(SDA, SCL)) {
        Serial.println("Warning: BME280 not found!");
    } else {
        Serial.println("BME280 initialized.");
    }
    
    current_state = STATE_IDLE;
    display_bme_screen(); 
    bme_update_timer = millis();
}

void state_update() {
    uint8_t uid[5];
    bool is_authorized = true;

    // Periodic BME280 update during IDLE to prevent LCD flickering
    if (current_state == STATE_IDLE) {
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
                // Log scanned HEX UID for debugging/adding new cards
                Serial.print("[RFID] Scanned UID: ");
                for (int i = 0; i < 4; i++) {
                    Serial.printf("%02X ", uid[i]);
                }
                Serial.println();

                // Authentication Check
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
                    Serial.println("[SYSTEM] Access Denied!");
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
            }
            
            state_timer = millis();
            break;

        case STATE_UNLOCK:
            // Hold the unlock message for 3 seconds before reverting
            if (millis() - state_timer > 3000) {
                current_state = STATE_IDLE;
                display_bme_screen(); 
                bme_update_timer = millis();
            }
            break;

        case STATE_ERROR:
            // Hold error message for 2 seconds
            if (millis() - state_timer > 2000) {
                current_state = STATE_IDLE;
                display_bme_screen();
                bme_update_timer = millis();
            }
            break;
    }
}