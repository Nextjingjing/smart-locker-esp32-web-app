package discord

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"os"
	"time"
)

var (
	lastTempAlertTime     time.Time
	lastHumidityAlertTime time.Time
)

const alertCooldown = 10 * time.Minute

func NotifyEnvironment(temp float64, humidity float64) {
	webhookURL := os.Getenv("DISCORD_WEBHOOK")
	if webhookURL == "" {
		return
	}

	now := time.Now()

	if temp >= 40.0 && now.Sub(lastTempAlertTime) > alertCooldown {
		lastTempAlertTime = now
		sendAlert("⚠️ High Temperature Alert!",
			fmt.Sprintf("อุณหภูมิสูงเกินกำหนด!\n**Temp:** %.2f°C\n**Humidity:** %.2f%%", temp, humidity),
			16744192)
	}

	if humidity < 15.0 && now.Sub(lastHumidityAlertTime) > alertCooldown {
		lastHumidityAlertTime = now
		sendAlert("💧 Low Humidity Alert!",
			fmt.Sprintf("ความชื้นต่ำเกินไป (อากาศแห้งมาก)!\n**Temp:** %.2f°C\n**Humidity:** %.2f%%", temp, humidity),
			3447003)
	}
}

func sendAlert(title string, description string, color int) {
	webhookURL := os.Getenv("DISCORD_WEBHOOK")
	payload := map[string]interface{}{
		"embeds": []map[string]interface{}{
			{
				"title":       title,
				"description": description,
				"color":       color,
				"timestamp":   time.Now().Format(time.RFC3339),
			},
		},
	}

	jsonData, _ := json.Marshal(payload)
	go http.Post(webhookURL, "application/json", bytes.NewBuffer(jsonData))
}
