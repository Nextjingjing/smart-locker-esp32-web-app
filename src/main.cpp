#include <Arduino.h>
#include "system_state.h"

void setup() {
    Serial.begin(115200); // อย่าลืมตั้ง Baudrate ใน Serial Monitor ให้ตรงกันนะ
    Serial.println("System Booting...");
    
    // Initialize Hardware ทั้งหมด
    state_init();
}

void loop() {
    // State Machine ตรวจสอบตลอดเวลา
    state_update();
    
    // หน่วงเวลาเล็กน้อย
    delay(10); 
}