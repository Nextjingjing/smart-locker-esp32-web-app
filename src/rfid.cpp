#include "config.h"
#include "rfid.h"
#include <SPI.h>

void antennaOn() {
    uint8_t value = readReg(TX_CONTROL_REG);
    if (!(value & 0x03)) {
        writeReg(TX_CONTROL_REG, value | 0x03);
    }
}

void writeReg(uint8_t reg, uint8_t value) {
    digitalWrite(SS_PIN, LOW);
    SPI.transfer((reg << 1) & 0x7E); 
    SPI.transfer(value);
    digitalWrite(SS_PIN, HIGH);
}


uint8_t readReg(uint8_t reg) {
    uint8_t value;
    digitalWrite(SS_PIN, LOW);
    SPI.transfer(((reg << 1) & 0x7E) | 0x80); 
    value = SPI.transfer(0x00); 
    digitalWrite(SS_PIN, HIGH);
    return value;
}

/* --- Public API Implementation --- */

void rfid_init() {
    pinMode(RST_PIN, OUTPUT);
    pinMode(SS_PIN, OUTPUT);
    
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    
    // Hard reset
    digitalWrite(RST_PIN, LOW);
    delay(10);
    digitalWrite(RST_PIN, HIGH);
    delay(10);
    
    // Soft reset command
    writeReg(COMMAND_REG, 0x0F);
    delay(50);

    antennaOn();
}

bool rfid_request() {
    writeReg(COMMAND_REG, 0x00);
    writeReg(COMM_IRQ_REG, 0x7F);
    writeReg(FIFO_LEVEL_REG, 0x80);

    writeReg(BIT_FRAMING_REG, 0x07); // 7-bit transmission
    writeReg(FIFO_DATA_REG, 0x26);    // REQA command
    writeReg(COMMAND_REG, 0x0C);      // Transceive
    writeReg(BIT_FRAMING_REG, 0x87);  // Start transmission

    delay(5);

    // Check if data was received (RxIRq bit)
    return (readReg(COMM_IRQ_REG) & 0x20);
}

bool rfid_read_uid(uint8_t *uid) {
    writeReg(COMMAND_REG, 0x00);
    writeReg(COMM_IRQ_REG, 0x7F);
    writeReg(FIFO_LEVEL_REG, 0x80);
    writeReg(BIT_FRAMING_REG, 0x00);

    writeReg(FIFO_DATA_REG, 0x93);    // Anticollision command
    writeReg(FIFO_DATA_REG, 0x20);
    writeReg(COMMAND_REG, 0x0C);
    writeReg(BIT_FRAMING_REG, 0x80);

    delay(5);

    if (!(readReg(COMM_IRQ_REG) & 0x20)) {
        return false;
    }

    // Standard Mifare UID response is 5 bytes
    if (readReg(FIFO_LEVEL_REG) != 5) {
        return false;
    }

    for (int i = 0; i < 5; i++) {
        uid[i] = readReg(FIFO_DATA_REG);
    }

    return true;
}