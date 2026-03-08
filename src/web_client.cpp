#include "web_client.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>

bool wifi_init() {
    Serial.println("\n[WiFi] Connecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    unsigned long start_attempt = millis();
    // รอเชื่อมต่อ 10 วินาที
    while (WiFi.status() != WL_CONNECTED && millis() - start_attempt < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] Connected Success!");
        Serial.print("[WiFi] IP Address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println("\n[WiFi] Connection Failed!");
        return false;
    }
}

void send_environment_data(float temp, float hum, float pres) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(API_ENV_ENDPOINT);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("X-API-KEY", API_KEY);
        String json = "{\"temp\":" + String(temp) + 
                      ",\"humidity\":" + String(hum) + 
                      ",\"pressure\":" + String(pres) + "}";

        int httpCode = http.POST(json);
        if (httpCode > 0) Serial.printf("[HTTP] Env Sent, Code: %d\n", httpCode);
        http.end();
    }
}

void send_door_data(String status) { // ลบพารามิเตอร์ String uid ออก
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(API_DOOR_ENDPOINT);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("X-API-KEY", API_KEY);
        // ส่งเฉพาะ status ไม่ต้องส่ง uid แล้ว
        String json = "{\"status\":\"" + status + "\"}";

        int httpCode = http.POST(json);
        if (httpCode > 0) Serial.printf("[HTTP] Door Sent, Code: %d\n", httpCode);
        http.end();
    }
}