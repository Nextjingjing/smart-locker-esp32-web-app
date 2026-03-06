#include "config.h"
#include "rfid.h"
#include <SPI.h>

// function prototypes
void antennaOn();
void writeReg(uint8_t reg, uint8_t value);
uint8_t readReg(uint8_t reg);

void antennaOn() {
    uint8_t value = readReg(TX_CONTROL_REG);
    if (!(value & 0x03)) {
        writeReg(TX_CONTROL_REG, value | 0x03);
    }
}

/**
 * @brief Sends a byte to an MFRC522 register.
 */
void writeReg(uint8_t reg, uint8_t value) {
    digitalWrite(SS_PIN, LOW);
    SPI.transfer((reg << 1) & 0x7E); // Send address
    SPI.transfer(value);             // Send data
    digitalWrite(SS_PIN, HIGH);
}

/**
 * @brief Reads a byte from an MFRC522 register.
 */
uint8_t readReg(uint8_t reg) {
    uint8_t value;
    digitalWrite(SS_PIN, LOW);
    SPI.transfer(((reg << 1) & 0x7E) | 0x80); // Send address with Read bit
    value = SPI.transfer(0x00);               // Receive data
    digitalWrite(SS_PIN, HIGH);
    return value;
}

/**
 * @brief Initializes the MFRC522 module.
 */
void rfid_init() {
    pinMode(RST_PIN, OUTPUT);
    pinMode(SS_PIN, OUTPUT);
    
    // Configure SPI hardware on ESP32
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    
    // Hardware reset
    digitalWrite(RST_PIN, LOW);
    delay(10);
    digitalWrite(RST_PIN, HIGH);
    delay(10);
    
    // Software reset
    writeReg(COMMAND_REG, 0x0F);
    delay(50);

    antennaOn();
}

bool rfid_request() {

    // Stop command
    writeReg(COMMAND_REG, 0x00);

    // ล้างค่า Interrupt
    writeReg(COMM_IRQ_REG, 0x7F);

    // Clear FIFO
    writeReg(FIFO_LEVEL_REG, 0x80);

    // Send REQA (7 bits)
    writeReg(BIT_FRAMING_REG, 0x07);

    // Write REQA command
    writeReg(FIFO_DATA_REG, 0x26);

    // Start Transceive
    writeReg(COMMAND_REG, 0x0C);

    // Start sending
    writeReg(BIT_FRAMING_REG, 0x87);

    // รอให้บัตรตอบกลับ
    delay(5);

    uint8_t irq = readReg(COMM_IRQ_REG);

    // *** แก้ไขตรงนี้: เช็คแค่ 0x20 (RxIRq) คือการยืนยันว่าได้รับข้อมูลจริง ***
    if (irq & 0x20) {
        return true;
    }

    return false;
}

bool rfid_read_uid(uint8_t *uid) {

    // Stop command
    writeReg(COMMAND_REG, 0x00);

    // ล้างค่า Interrupt
    writeReg(COMM_IRQ_REG, 0x7F);

    // Clear FIFO
    writeReg(FIFO_LEVEL_REG, 0x80);

    writeReg(BIT_FRAMING_REG, 0x00);

    // ส่งคำสั่ง Anticollision
    writeReg(FIFO_DATA_REG, 0x93);
    writeReg(FIFO_DATA_REG, 0x20);

    writeReg(COMMAND_REG, 0x0C);
    writeReg(BIT_FRAMING_REG, 0x80);

    delay(5);

    uint8_t irq = readReg(COMM_IRQ_REG);

    // เช็คอีกรอบว่าได้รับข้อมูล UID จริงไหม
    if (!(irq & 0x20)) {
        return false;
    }

    uint8_t level = readReg(FIFO_LEVEL_REG);

    if (level != 5)
        return false;

    for (int i = 0; i < 5; i++) {
        uid[i] = readReg(FIFO_DATA_REG);
    }

    return true;
}