/**
 * @file rfid.h
 * @author Your Name
 * @brief Low-level driver for the MFRC522 RFID reader module.
 * @details This header defines the hardware registers and core functions 
 * required to communicate with the MFRC522 over the SPI interface, 
 * including card detection and UID retrieval.
 */

#ifndef RFID_H
#define RFID_H

#include <Arduino.h>

/** * @name MFRC522 Register Addresses
 * Internal registers of the MFRC522 used for command execution and data buffering.
 * @{ 
 */
#define COMMAND_REG     0x01  ///< Starts and stops command execution
#define FIFO_DATA_REG   0x09  ///< Input and output of 64 byte FIFO buffer
#define VERSION_REG     0x37  ///< Software version of the MFRC522
#define FIFO_LEVEL_REG  0x0A  ///< Number of bytes stored in the FIFO buffer
#define BIT_FRAMING_REG 0x0D  ///< Adjustments for bit-oriented frames
#define TX_CONTROL_REG  0x14  ///< Controls the logical behavior of the antenna driver pins TX1 and TX2
#define ERROR_REG       0x06  ///< Error register showing the status of the last command executed
#define COMM_IRQ_REG    0x04  ///< Interrupt request bits
#define CONTROL_REG     0x0C  ///< Miscellaneous control registers
/** @} */

/** * @name Register Communication
 * Low-level SPI primitives to interact with the MFRC522 hardware.
 * @{ 
 */

/**
 * @brief Writes a single byte to a specific MFRC522 register.
 * @param reg The register address to write to.
 * @param value The byte value to store in the register.
 */
void writeReg(uint8_t reg, uint8_t value);

/**
 * @brief Reads a single byte from a specific MFRC522 register.
 * @param reg The register address to read from.
 * @return The 8-bit value stored in the register.
 */
uint8_t readReg(uint8_t reg);
/** @} */

/** * @name RFID Core Functions
 * Initialization and hardware state management.
 */

/**
 * @brief Performs the power-on reset and basic configuration of the MFRC522.
 */
void rfid_init();

/**
 * @brief Enables the internal antenna drivers to begin broadcasting the 13.56MHz field.
 */
void antennaOn();

/** * @name Card Detection and Data Retrieval
 * Functions for interacting with physical RFID tags.
 * @{ 
 */

/**
 * @brief Scans for the presence of an RFID tag in the induction field.
 * @return true if a card/tag is detected.
 * @return false if no tag is present or communication fails.
 */
bool rfid_request();

/**
 * @brief Retrieves the Unique Identifier (UID) from a detected tag.
 * @param[out] uid Pointer to a buffer where the 4-byte or 7-byte UID will be stored.
 * @return true if the UID was successfully read and validated.
 */
bool rfid_read_uid(uint8_t *uid);
/** @} */

#endif // RFID_H