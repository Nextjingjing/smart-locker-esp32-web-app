#include <Arduino.h>
#include "system_state.h"

void setup() {
    Serial.begin(115200);
    // ฟังก์ชันนี้จะทำการ Initialize ทั้ง LCD และ RFID ทันที
    state_init();
}

void loop() {
    // ให้ State Machine ทำงานวนลูปตรวจสอบตลอดเวลา
    state_update();
    
    // หน่วงเวลาเล็กน้อยเพื่อไม่ให้ลูปทำงานหนักเกินไป
    delay(10); 
}