/**
 * @file system_state.h
 * @author Your Name
 * @brief State Machine Controller for the RFID Access System.
 * @details This file defines the operational states and the main logic controller
 * that transitions the system between scanning, authenticating, and accessing data.
 */

#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <Arduino.h>

/**
 * @brief Enumeration of possible system operating modes.
 * Represents the current status of the Finite State Machine (FSM).
 */
typedef enum {
    STATE_IDLE,           ///< Default: Waiting for a card to be presented
    STATE_CARD_DETECTED,  ///< Processing: A tag is in range and UID is being read
    STATE_UNLOCK,         ///< Authorized: Access granted, updating LCD/Actuators
    STATE_ERROR           ///< Denied/Failure: Invalid card or communication error
} system_state_t;

/**
 * @brief Initializes the system state machine.
 * Sets the initial state to STATE_IDLE and prepares any required 
 * internal logic flags.
 */
void state_init();

/**
 * @brief The main execution loop for the state machine.
 * @details This function should be called repeatedly in the main loop(). 
 * It handles:
 * 1. Sensor polling (RFID/BME280)
 * 2. Logical transitions between states based on inputs
 * 3. Output updates (LCD messages/Door strike/LEDs)
 */
void state_update();

#endif // SYSTEM_STATE_H