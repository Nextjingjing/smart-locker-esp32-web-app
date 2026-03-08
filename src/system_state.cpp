#include <Arduino.h>
#include "system_state.h"
#include "rfid.h"
#include "lcd.h"
#include "config.h"
#include "bme280_simple.h"
#include "web_client.h"

static system_state_t current_state = STATE_IDLE;
static unsigned long state_timer = 0;
static unsigned long bme_update_timer = 0;

/**
 * Reads sensor data once, then updates Serial, LCD, and returns values via parameters.
 */
static void update_bme_all(float &t, float &h, float &p) {
    bme280_read(t, h, p);

    // Debug output
    Serial.printf("[BME280] T: %.1f C | H: %.1f %% | P: %.1f hPa\n", t, h, p);

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
    }
    
    wifi_init();

    current_state = STATE_IDLE;
    
    // Initial display on boot
    float t, h, p;
    update_bme_all(t, h, p);
    bme_update_timer = millis();
}

void state_update() {
    uint8_t uid[5];
    float current_t, current_h, current_p;

    // Update and send BME data every 2 seconds only when in IDLE state
    if (current_state == STATE_IDLE) {
        if (millis() - bme_update_timer > 2000) {
            update_bme_all(current_t, current_h, current_p);
            send_environment_data(current_t, current_h, current_p);
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
                // Prepare UID as HEX string for HTTP transmission
                String card_uid = "";
                for (int i = 0; i < 4; i++) {
                    if (uid[i] < 0x10) card_uid += "0";
                    card_uid += String(uid[i], HEX);
                }
                card_uid.toUpperCase();

                bool is_authorized = true;
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
                    lcd_print("Access Granted!");
                    send_door_data("granted");
                } else {
                    Serial.println("[SYSTEM] Access Denied!");
                    current_state = STATE_ERROR;
                    lcd_clear();
                    lcd_print("Access Denied");
                    send_door_data("denied");
                }
            } else {
                current_state = STATE_ERROR;
                lcd_clear();
                lcd_print("Read Error");
            }
            state_timer = millis();
            break;

        case STATE_UNLOCK:
            if (millis() - state_timer > 3000) {
                current_state = STATE_IDLE;
                update_bme_all(current_t, current_h, current_p); // Resume sensor display immediately
                bme_update_timer = millis();
            }
            break;

        case STATE_ERROR:
            if (millis() - state_timer > 2000) {
                current_state = STATE_IDLE;
                update_bme_all(current_t, current_h, current_p);
                bme_update_timer = millis();
            }
            break;
    }
}