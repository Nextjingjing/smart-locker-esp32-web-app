/**
 * @file main.ino
 * @author Pruek Tanvorakul
 * @brief Entry point for the RFID Access Control & Environmental Monitoring System.
 * @details This file initializes the Serial communication and hands over control 
 * to the System State Machine. The architecture follows a non-blocking 
 * design to ensure the RFID reader remains responsive.
 */

#include <Arduino.h>
#include "system_state.h"

/**
 * @brief Standard Arduino setup function.
 * @details Configures the hardware serial port for debugging and 
 * triggers the initialization of all peripheral drivers via state_init().
 */
void setup() {
    // Initialize serial communication at 115200 bits per second.
    Serial.begin(115200); 
    Serial.println(F("System Booting..."));
    
    /** * @note Initializing all hardware (RFID, LCD, BME280) through the state manager
     * to ensure the system starts in a known safe state (STATE_IDLE).
     */
    state_init();
}

/**
 * @brief Standard Arduino execution loop.
 * @details Repeatedly calls the state machine update function. 
 * The 10ms delay provides a small window for background stability 
 * without compromising sensor polling frequency.
 */
void loop() {
    // Core logic: Process sensor inputs and handle state transitions.
    state_update();
    
    /** * @brief Small task-switching delay.
     * @details A 10ms delay (100Hz polling rate) is generally sufficient 
     * for human interaction (RFID card tapping) while allowing the 
     * ESP32 background tasks (like WiFi/Watchdog) to run if needed.
     */
    delay(10); 
}