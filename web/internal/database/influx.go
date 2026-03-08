package database

import (
	"context"
	"fmt"
	"time"

	influxdb2 "github.com/influxdata/influxdb-client-go/v2"
)

type InfluxClient struct {
	Client influxdb2.Client
	Bucket string
	Org    string
}

type DataPoint struct {
	Time  time.Time `json:"time"`
	Value float64   `json:"value"`
}

func NewInfluxClient(url, token, org, bucket string) *InfluxClient {
	client := influxdb2.NewClient(url, token)
	return &InfluxClient{Client: client, Bucket: bucket, Org: org}
}

func (db *InfluxClient) WriteEnvironmentData(temp, humidity, pressure float64) error {
	writeAPI := db.Client.WriteAPIBlocking(db.Org, db.Bucket)
	p := influxdb2.NewPoint("environment",
		map[string]string{"unit": "celsius"},
		map[string]interface{}{"temp": temp, "humidity": humidity, "pressure": pressure},
		time.Now())
	return writeAPI.WritePoint(context.Background(), p)
}

func (db *InfluxClient) WriteDoorData(status string) error {
	writeAPI := db.Client.WriteAPIBlocking(db.Org, db.Bucket)

	p := influxdb2.NewPoint("door_status",
		map[string]string{"device": "rfid_reader"},
		map[string]interface{}{"status": status},
		time.Now())

	return writeAPI.WritePoint(context.Background(), p)
}

func (db *InfluxClient) GetDailyData(day string, field string) ([]DataPoint, error) {
	// แยก Measurement ตาม field ที่ร้องขอ
	measurement := "environment"
	if field == "status" {
		measurement = "door_status"
	}

	query := fmt.Sprintf(`from(bucket:"%s")
		|> range(start: %sT00:00:00Z, stop: %sT23:59:59Z)
		|> filter(fn:(r) => r._measurement == "%s")
		|> filter(fn:(r) => r._field == "%s")`,
		db.Bucket, day, day, measurement, field)

	queryAPI := db.Client.QueryAPI(db.Org)
	result, err := queryAPI.Query(context.Background(), query)
	if err != nil {
		return nil, err
	}

	var points []DataPoint
	for result.Next() {
		var val float64

		if field == "status" {
			strVal, ok := result.Record().Value().(string)
			if ok && strVal == "granted" {
				val = 1.0
			} else {
				val = 0.0
			}
		} else {
			val, _ = result.Record().Value().(float64)
		}

		points = append(points, DataPoint{
			Time:  result.Record().Time(),
			Value: val,
		})
	}
	return points, nil
}
