// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Please use an Arduino IDE 1.6.8 or greater

#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <time.h>

#include <stdlib.h>

#include "config.h"

ADC_MODE(ADC_VCC);


static bool messagePending = false;
static bool messageSending = true;

 char *connectionString = "HostName=IoTHubForYarkoba4i.azure-devices.net;DeviceId=ESP8266_temp;SharedAccessKey=OOTPq2Zv7BJQipiJV2z5n1CrD1ctqJibSSNGH8KoM5M=";
// char *ssid = "pr500k-27a997-RPT";

  char *ssid = "pr500k-27a997_3_RPT";
  char *pass = "ce32fcda56211";
  const char *_server = "184.106.153.149"; // thingspeak.com

 const int channelID = 332445;
 const char *_APIKey = "P1SOR94TRFN6E5DM"; // write API key for your ThingSpeak Channel
 const char *_GET = "GET https://api.thingspeak.com/update?api_key=";

static int interval = INTERVAL;
WiFiClient client;

const int sleepTimeMin = 10;


void initWifi()
{
	// Attempt to connect to Wifi network:
	Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);
	WiFi.mode(WIFI_STA);

	// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
	WiFi.begin(ssid, pass);
	//WiFi.begin("pr500k-27a997-RPT", "ce32fcda56211");
	while (WiFi.status() != WL_CONNECTED)
	{
		// Get Mac Address and show it.
		// WiFi.macAddress(mac) save the mac address into a six length array, but the endian may be different. The huzzah board should
		// start from mac[0] to mac[5], but some other kinds of board run in the oppsite direction.
		uint8_t mac[6];
		WiFi.macAddress(mac);
		Serial.printf("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
		WiFi.begin(ssid, pass);
		delay(10000);
		//Serial.println("WiFi.RSSI() = " + String(WiFi.RSSI(),10));
	}
	Serial.printf("Connected to wifi %s.\r\n", ssid);
}

void initTime()
{
	time_t epochTime;
	configTime(0, 0, "pool.ntp.org", "time.nist.gov");

	while (true)
	{
		epochTime = time(NULL);

		if (epochTime == 0)
		{
			Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
			delay(2000);
		}
		else
		{
			Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime);
			break;
		}
	}
}


void setup()
{

	initSerial();
	delay(2000);

//	readCredentials();

	initWifi();
	initTime();
	initSensor();

}

//static int messageCount = 1;
void loop()
{

	if (client.connect(_server, 80)) {

		// Measure Signal Strength (RSSI) of Wi-Fi connection
		//Serial.println("messageCount = " + String(messageCount));
		delay(10000);
		sendMessage();

	
	}

	sleep_mode(true);

}


void sleep_mode(bool sleep) {

	if (sleep) {
		//	Serial.println("Humidity: " + String(readHumidity(), 2));
		delay(10000);
		client.stop();
		WiFi.disconnect();
		HX711_OFF();
		delay(10);

		//ESP.deepSleep(20000);
		ESP.deepSleep(sleepTimeMin * 60000000);
				
	}
	
}


