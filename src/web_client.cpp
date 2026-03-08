#include "web_client.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>

bool wifi_init() {
    Serial.println("\n[WiFi] Connecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    unsigned long start_attempt = millis();
    // Wait for connection (10-second timeout)
    while (WiFi.status() != WL_CONNECTED && millis() - start_attempt < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] Connection Successful!");
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

        // Construct JSON payload
        String json = "{\"temp\":" + String(temp) + 
                      ",\"humidity\":" + String(hum) + 
                      ",\"pressure\":" + String(pres) + "}";

        int httpCode = http.POST(json);
        if (httpCode > 0) {
            Serial.printf("[HTTP] Environment data sent. Response Code: %d\n", httpCode);
        } else {
            Serial.printf("[HTTP] Environment POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("[HTTP] Environment data not sent: WiFi disconnected");
    }
}

void send_door_data(String status) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(API_DOOR_ENDPOINT);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("X-API-KEY", API_KEY);

        String json = "{\"status\":\"" + status + "\"}";

        int httpCode = http.POST(json);
        if (httpCode > 0) {
            Serial.printf("[HTTP] Door status sent. Response Code: %d\n", httpCode);
        } else {
            Serial.printf("[HTTP] Door POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("[HTTP] Door status not sent: WiFi disconnected");
    }
}