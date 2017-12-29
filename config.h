// Physical device information for board and sensor
#define DEVICE_ID "ESP8266_temp" 
//"SparkFun ESP8266 Thing Dev"


#define DHT_TYPE DHT22

// Pin layout configuration
#define LED_PIN 0
#define DHT_PIN 2

// HX711
#define _DOUT 4
#define _CLK  5
#define calibration_factor 6120 // this calibration factor is for my 200g load cell
#define	MEASURE_TIMES	64

#define TEMPERATURE_ALERT 30

// Interval time(ms) for sending message to IoT Hub
#define INTERVAL 5000
// Interval time(ms) for deepsllep of esp
#define DEEPSLEEPTIME 60000000


// If don't have a physical DHT sensor, can send simulated data to IoT hub
#define SIMULATED_DATA false

// If to ESP-01 connected DHT sensor = TRUE, HX711 ADC = FALSE
//#define DHT_HX711	false


static  char *connectionString = "HostName=IoTHubForYarkoba4i.azure-devices.net;DeviceId=ESP8266_temp;SharedAccessKey=OOTPq2Zv7BJQipiJV2z5n1CrD1ctqJibSSNGH8KoM5M=";
// char *ssid = "pr500k-27a997-RPT";
static  char *ssid = "pr500k-27a997_3_RPT";
static  char *pass = "ce32fcda56211";
static int interval = INTERVAL;
static const uint32 deep_sleep_time = DEEPSLEEPTIME;

// EEPROM address configuration
#define EEPROM_SIZE 512

// SSID and SSID password's length should < 32 bytes
// http://serverfault.com/a/45509
#define SSID_LEN 32
#define PASS_LEN 32
#define CONNECTION_STRING_LEN 256

#define MESSAGE_MAX_LEN 256
