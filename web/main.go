package main

import (
	"log"
	"os"
	"sync"

	"github.com/gofiber/contrib/websocket"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/middleware/cors"
	"github.com/joho/godotenv"
)

var MY_API_KEY string

func init() {
	// Load environment variables from .env file
	if err := godotenv.Load(); err != nil {
		log.Println("No .env file found, using system environment variables")
	}

	// Retrieve the API Key from environment variables
	MY_API_KEY = os.Getenv("MY_API_KEY")
	if MY_API_KEY == "" {
		log.Fatal("MY_API_KEY is not set in environment variables")
	}
}

// Hub maintains the set of active WebSocket connections
type Hub struct {
	clients map[*websocket.Conn]bool
	mutex   sync.Mutex
}

var hub = Hub{clients: make(map[*websocket.Conn]bool)}

// EnvironmentData defines the structure for BME280 sensor data
type EnvironmentData struct {
	Temp     float64 `json:"temp"`
	Humidity float64 `json:"humidity"`
	Pressure float64 `json:"pressure"`
}

// DoorData defines the structure for RFID door status data
type DoorData struct {
	Status string `json:"status"`
}

// apiKeyAuth middleware to validate requests against the API Key
func apiKeyAuth(c *fiber.Ctx) error {
	key := c.Get("X-API-KEY")
	if key != MY_API_KEY {
		return c.Status(401).JSON(fiber.Map{
			"error": "Unauthorized: Invalid API Key",
		})
	}
	return c.Next()
}

func main() {
	app := fiber.New()
	app.Use(cors.New())

	// Serve static files from the 'public' directory
	app.Static("/", "./public")

	// --- WebSocket Route (Publicly accessible for browser clients) ---
	app.Get("/ws", websocket.New(func(c *websocket.Conn) {
		hub.mutex.Lock()
		hub.clients[c] = true
		hub.mutex.Unlock()
		defer func() {
			hub.mutex.Lock()
			delete(hub.clients, c)
			hub.mutex.Unlock()
			c.Close()
		}()
		for {
			if _, _, err := c.ReadMessage(); err != nil {
				break
			}
		}
	}))

	// --- API Routes requiring API Key authentication ---
	api := app.Group("/data", apiKeyAuth)

	// 1. Receive data from BME280 -> URL: http://localhost:3000/data/environment
	api.Post("/environment", func(c *fiber.Ctx) error {
		var data EnvironmentData
		if err := c.BodyParser(&data); err != nil {
			return c.Status(400).SendString("Invalid BME280 Data")
		}

		broadcast(fiber.Map{
			"type": "environment",
			"data": data,
		})
		return c.SendStatus(200)
	})

	// 2. Receive data from MFRC522 -> URL: http://localhost:3000/data/door
	api.Post("/door", func(c *fiber.Ctx) error {
		var data DoorData
		if err := c.BodyParser(&data); err != nil {
			return c.Status(400).SendString("Invalid Data Format")
		}

		if data.Status == "" {
			return c.Status(400).SendString("Status is required")
		}

		broadcast(fiber.Map{
			"type": "door",
			"data": data,
		})
		return c.SendStatus(200)
	})

	log.Println("Server running at http://localhost:3000")
	log.Fatal(app.Listen(":3000"))
}

// broadcast sends the provided data to all connected WebSocket clients
func broadcast(data interface{}) {
	hub.mutex.Lock()
	defer hub.mutex.Unlock()
	for client := range hub.clients {
		client.WriteJSON(data)
	}
}
