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


#include <stdlib.h>

#include "config.h"
#include "Timing.h"

ADC_MODE(ADC_VCC);

struct ee_data_str {
	int		sleep_mins;
	float	am_wght;
	float	min_wght;
	uint8_t	not_wifi_cnnct_times; // should device connect to wifi after awaking: 0->connect, "n"->do not connenct, every connect n--;
								// used for night mode
};

float inline min(float, float);

struct ee_data_str ee_data;

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


const int sleepTimeMin = 1;


Timing tm;
WiFiClient client;
void initWifi();


void setup()
{

	initSerial();
	delay(1000);

	readEEpromData();
//	ee_data.not_wifi_cnnct_times = 0;

	if (ee_data.not_wifi_cnnct_times == 0) { // it's day 
		delay(200);
		initSensores();
		initWifi();

		tm.initTime();
		delay(500);
		ee_data.sleep_mins = tm.getNextMeasuringMMLeft();
		Serial.printf("Day ... eeprom.getNextMeasuringMMLeft = %d\n", tm.getNextMeasuringMMLeft());
		Serial.printf("Day ... eeprom.not_wifi_cnnct_times = %d\n", ee_data.not_wifi_cnnct_times);
	}
	else {// it's nigth
		//eeprom write ee_data.not_wifi_cnnct_times--;

		Serial.printf("Night ... eeprom.not_wifi_cnnct_times = %d\n", ee_data.not_wifi_cnnct_times);
		ee_data.not_wifi_cnnct_times--;
		ee_data.sleep_mins -= 60;

		WriteEEPROMData();
		sleep_mode(sleepTimeMin);
	}
	
	/*
	if (tm.isFreshStart(tm.getTimeNow(), ee_data_str.mesure_time))
	{
		Serial.println("Fresh start ... erasing eeprom.");
		EraseEEPROM();
	}*/
	
}

//static int messageCount = 1;
void loop()
{

	/*
*/
	delay(5000);


	// if 6am
	if ((tm.getHH() == Wk_UP_Hr) && (tm.getMM() < 15)) {
		
		// saving 6 am weight to eeporm
		ee_data.am_wght = readWeight();

		WriteEEPROMData();
		Serial.printf("6am... eeprom.sleep_mins = %d\n", ee_data.sleep_mins);
		Serial.printf("6am... eeprom.am_wght = %f\n", ee_data.am_wght);
		// deep sleep till next measure
		sleep_mode(tm.getNextMeasuringMMLeft());

	}// day mesurements
	else if ((tm.getHH() >= Wk_UP_Hr) && (tm.getMM() >= 15) && (tm.getHH() < Sleep_Hr)) {

		float minD = ee_data.am_wght - readWeight();
		if (client.connect(_server, 80)) {

			delay(500);
			sendMessage(minD, NULL);

		}

		Serial.printf("Day... eeprom.sleep_mins = %d\n", ee_data.sleep_mins);
		Serial.printf("Day... minD = %f\n", minD);
	}
	// 22 
	if ((tm.getHH() == Sleep_Hr) && (tm.getMM() < 15)) {
		float minD = ee_data.am_wght - readWeight();
		float Delta = readWeight() - ee_data.am_wght;
		if (client.connect(_server, 80)) {

			delay(500);
			sendMessage(minD, Delta);

		}

		//preparation for a night sleep
		ee_data.sleep_mins = tm.getAMwkUPmins();
		ee_data.not_wifi_cnnct_times = (uint8_t)ceil(ee_data.sleep_mins / 60);
		WriteEEPROMData();

		Serial.printf("10pm... eeprom.sleep_mins = %d\n", ee_data.sleep_mins);
		Serial.printf("10pm... Delta = %f\n", Delta);

		sleep_mode(60);
	}
	
	
	

	WriteEEPROMData();



	sleep_mode(ee_data.sleep_mins);

}

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

void sleep_mode(uint8_t min) {

		Serial.printf("Sleep for %d min\n ", min);
		delay(10);
		client.stop();
		WiFi.disconnect();

		delay(10);

		//ESP.deepSleep(20000);
		ESP.deepSleep(min * 60000000);

}



float inline min(float amw, float mw) {
	return ((amw < mw) ? amw : mw);
}


