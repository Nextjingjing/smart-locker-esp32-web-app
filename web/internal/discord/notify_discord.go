package discord

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"strings"
	"time"
)

func NotifyDiscord(status string) {
	webhookURL := os.Getenv("DISCORD_WEBHOOK")
	if webhookURL == "" {
		return
	}

	s := strings.ToLower(status)

	color := 15158332
	statusEmoji := "❌"
	title := "Access Denied"

	if s == "granted" {
		color = 3066993
		statusEmoji = "✅"
		title = "Access Granted"
	}

	payload := map[string]interface{}{
		"embeds": []map[string]interface{}{
			{
				"title":       title,
				"description": fmt.Sprintf("%s สถานะ: **%s**", statusEmoji, status),
				"color":       color,
				"footer": map[string]string{
					"text": "Smart Locker System",
				},
				"timestamp": time.Now().Format(time.RFC3339),
			},
		},
	}

	jsonData, _ := json.Marshal(payload)

	go func() {
		resp, err := http.Post(webhookURL, "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			log.Printf("Discord Error: %v", err)
			return
		}
		defer resp.Body.Close()
	}()
}
