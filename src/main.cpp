#include <Arduino.h>
#include "rfid.h"
#include "config.h"
#include "lcd.h"

uint8_t uid[5];

void setup() {

    Serial.begin(BAUD_RATE);

    lcd_init();
    lcd_clear();
    lcd_setCursor(0,0);
    lcd_print("System Ready");

    rfid_init();

    uint8_t version = readReg(VERSION_REG);

    Serial.print("MFRC522 Version: 0x");
    Serial.println(version, HEX);

    delay(1000);

    lcd_clear();
    lcd_setCursor(0,0);
    lcd_print("Idle");
}

void loop() {

    if (rfid_request()) {

        if (rfid_read_uid(uid)) {

            lcd_clear();
            lcd_setCursor(0,0);
            lcd_print("Welcome");

            Serial.print("Card UID: ");

            for (int i = 0; i < 4; i++) {
                Serial.print(uid[i], HEX);
                Serial.print(" ");
            }

            Serial.println();

            delay(2000);

            lcd_clear();
            lcd_setCursor(0,0);
            lcd_print("Idle");
        }
    }
}