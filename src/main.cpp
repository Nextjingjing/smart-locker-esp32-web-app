#include <Arduino.h>
#include "rfid.h"
#include "config.h"

uint8_t uid[5];

void setup() {
    Serial.begin(BAUD_RATE);
    rfid_init();

    // The Version Register (0x37) should return 0x91 or 0x92
    uint8_t version = readReg(VERSION_REG);
    
    Serial.print("MFRC522 Version: 0x");
    Serial.println(version, HEX);
}

void loop() {
    if (rfid_request()) {

        if (rfid_read_uid(uid)) {

            Serial.print("Card UID: ");

            for (int i = 0; i < 4; i++) {
                Serial.print(uid[i], HEX);
                Serial.print(" ");
            }

            Serial.println();
        }

        delay(1000);
    }
}