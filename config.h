// Physical device information for board and sensor
#define DEVICE_ID "ESP8266 ESP-01" 
//"SparkFun ESP8266 Thing Dev"
#define DHT_TYPE DHT22

// Pin layout configuration
#define LED_PIN 5
#define DHT_PIN 2

// HX711
#define _DOUT  0
#define _CLK  2
#define calibration_factor 6120 // this calibration factor is for my 200g load cell
#define	MEASURE_TIMES	64

#define TEMPERATURE_ALERT 30

// Interval time(ms) for sending message to IoT Hub
#define INTERVAL 5000

// If don't have a physical DHT sensor, can send simulated data to IoT hub
#define SIMULATED_DATA false

// If to ESP-01 connected DHT sensor = TRUE, HX711 ADC = FALSE
#define DHT_HX711	false

// EEPROM address configuration
#define EEPROM_SIZE 512

// SSID and SSID password's length should < 32 bytes
// http://serverfault.com/a/45509
#define SSID_LEN 32
#define PASS_LEN 32
#define CONNECTION_STRING_LEN 256

#define MESSAGE_MAX_LEN 256
