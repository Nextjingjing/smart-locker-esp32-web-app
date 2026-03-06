#ifndef RFID_H
#define RFID_H

#include <Arduino.h>

// MFRC522 Register addresses
#define COMMAND_REG     0x01
#define FIFO_DATA_REG   0x09
#define VERSION_REG     0x37
#define FIFO_LEVEL_REG  0x0A
#define BIT_FRAMING_REG 0x0D
#define TX_CONTROL_REG  0x14
#define ERROR_REG       0x06
#define COMM_IRQ_REG    0x04
#define CONTROL_REG     0x0C

// Register communication
void writeReg(uint8_t reg, uint8_t value);
uint8_t readReg(uint8_t reg);

// RFID core functions
void rfid_init();
void antennaOn();

// Card detection
bool rfid_request();

// Read UID
bool rfid_read_uid(uint8_t *uid);

#endif