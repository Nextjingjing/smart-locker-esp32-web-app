#include "system_state.h"
#include "rfid.h"
#include "lcd.h"

// ตัวแปรเก็บสถานะปัจจุบันและตัวจับเวลา
static system_state_t current_state = STATE_IDLE;
static unsigned long state_timer = 0;

// ตัวอย่าง UID ที่ได้รับอนุญาต (คุณสามารถเปลี่ยนเป็น UID บัตรของคุณได้)
const uint8_t AUTHORIZED_UID[4] = {0x35, 0x85, 0x4E, 0x06};

// ฟังก์ชันแสดงผลหน้าจอเริ่มต้น
static void display_idle_screen() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("System Ready");
    lcd_setCursor(0, 1);
    lcd_print("Scan your card..");
}

void state_init() {
    // กำหนดค่าเริ่มต้นให้กับ Hardware
    lcd_init();
    rfid_init();
    
    // ตั้งสถานะเริ่มต้น
    current_state = STATE_IDLE;
    display_idle_screen();
}

void state_update() {
    uint8_t uid[5]; // เก็บค่า UID 4 ไบต์ + BCC 1 ไบต์
    bool is_authorized = true;

    switch (current_state) {
        
        case STATE_IDLE:
            // หากตรวจพบบัตรให้เปลี่ยนสถานะไปที่ CARD_DETECTED
            if (rfid_request()) {
                current_state = STATE_CARD_DETECTED;
            }
            break;

        case STATE_CARD_DETECTED:
            // พยายามอ่าน UID จากบัตร
            if (rfid_read_uid(uid)) {
                // ตรวจสอบความถูกต้องของบัตร (เทียบกับ AUTHORIZED_UID)
                for (int i = 0; i < 4; i++) {
                    if (uid[i] != AUTHORIZED_UID[i]) {
                        is_authorized = false;
                        break;
                    }
                }

                if (is_authorized) {
                    current_state = STATE_UNLOCK;
                    lcd_clear();
                    lcd_setCursor(0, 0);
                    lcd_print("Access Granted!");
                    lcd_setCursor(0, 1);
                    lcd_print("Unlocking...");
                } else {
                    current_state = STATE_ERROR;
                    lcd_clear();
                    lcd_setCursor(0, 0);
                    lcd_print("Access Denied");
                    lcd_setCursor(0, 1);
                    lcd_print("Unknown Card");
                }
            } else {
                // หากอ่านล้มเหลว
                current_state = STATE_ERROR;
                lcd_clear();
                lcd_setCursor(0, 0);
                lcd_print("Read Error");
                lcd_setCursor(0, 1);
                lcd_print("Please try again");
            }
            
            // บันทึกเวลาที่เริ่มแสดงข้อความ
            state_timer = millis();
            break;

        case STATE_UNLOCK:
            // ค้างสถานะปลดล็อคไว้ 3 วินาที (3000 ms) 
            // *สามารถเพิ่มโค้ดสั่งงาน Relay ปลดล็อคประตูตรงนี้ได้*
            if (millis() - state_timer > 3000) {
                current_state = STATE_IDLE;
                display_idle_screen();
            }
            break;

        case STATE_ERROR:
            // ค้างข้อความแจ้งเตือนผิดพลาดไว้ 2 วินาที (2000 ms)
            if (millis() - state_timer > 2000) {
                current_state = STATE_IDLE;
                display_idle_screen();
            }
            break;
    }
}