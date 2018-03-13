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

// Structure saived in eeprom
struct ee_data_str {
	float	am_wght;
	int		sleep_sec;		// sllep for sleep_mins, 
	time_t	last_measure_time;	// time of the last measurmet
	uint8_t	not_wifi_cnnct_times; // should device connect to wifi after awaking: 0->connect, "n"->do not connenct, every connect n--;
							// used for night mode
};

float inline min(float, float);

struct ee_data_str ee_data;

//----------------------------------------------------------------------------------
// Network connecting variables

 char *connectionString = "HostName=IoTHubForYarkoba4i.azure-devices.net;DeviceId=ESP8266_temp;SharedAccessKey=OOTPq2Zv7BJQipiJV2z5n1CrD1ctqJibSSNGH8KoM5M=";
// char *ssid = "pr500k-27a997-RPT";

 char *ssid = "***";
 char *pass = "***";
 const char *_server = "184.106.153.149"; // thingspeak.com

 const int channelID = ***;
 const char *_APIKey = "***"; // write API key for your ThingSpeak Channel
 const char *_GET = "GET https://api.thingspeak.com/update?api_key=";

 //----------------------------------------------------------------------------------

Timing tm;
WiFiClient client;
void initWifi();


void setup()
{

	initSerial();
	delay(1000);


//	EraseEEPROM();
	readEEpromData();
	
//	ee_data.not_wifi_cnnct_times = 0;

	if (ee_data.not_wifi_cnnct_times == 0) { // it's day 
		Serial.printf("\nDay ... \n");

		initWifi();

		tm.initTime();
		
		if (tm.getTimeNow() < ee_data.last_measure_time)
		{
			Serial.println("Wrong time... Waiting for initTime ... ");
			for (uint i = 5; i >= 0; i--) {
				tm.initTime();
				delay(3000);
				if (tm.getTimeNow() > ee_data.last_measure_time)
					break;
				else
					reset();
			}

		}

		if (tm.isDay()) {
			Serial.println("Initalizing sensors");
			uint8_t i = 5;
			while(!initSensores() || i==0){
				Serial.print(".");
				i--;
				delay(500);
			}

		}
		else {		//preparation for a night sleep
			
			ee_data.sleep_sec = tm.getAMwkUPmins();
			ee_data.not_wifi_cnnct_times = (uint8_t)ceil(ee_data.sleep_sec / 360);
			WriteEEPROMData();

			Serial.printf("\n Not day... falling asleep for %d min\n", ee_data.sleep_sec/60);

			sleep_sec_mode(3600);
		}
	}
	else {// it's nigth
		//eeprom write ee_data.not_wifi_cnnct_times--

		
		ee_data.not_wifi_cnnct_times--;
		ee_data.sleep_sec -= 3600;
		Serial.printf("Night ... sleep minutes left = %d\n", ee_data.sleep_sec);
		Serial.printf("Night ... wake up's left = %d\n", ee_data.not_wifi_cnnct_times);

	
		WriteEEPROMData();
		sleep_sec_mode(3600);
	}
	
	/*
	if (tm.isFreshStart(tm.getTimeNow(), ee_data_str.mesure_time))
	{
		Serial.println("Fresh start ... erasing eeprom.");
		EraseEEPROM();
	}*/
	
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void loop()
{
	time_t timenow = tm.getTimeNow();
	tm.printTimeNow();


	delay(500);

	//----------------------------------------------------------------------------------
	//		// if 6am
	//----------------------------------------------------------------------------------
	
	if (tm.isWakeUPHour()) {
		
		// saving 6 am weight to eeporm
		ee_data.am_wght = readWeight();
		ee_data.last_measure_time = tm.getTimeNow();

		sendMessage(NULL, NULL);

//		Serial.printf("6am... eeprom.sleep_sec = %d\n", ee_data.sleep_sec);
		Serial.printf("6am... eeprom.am_wght = %f\n", ee_data.am_wght);

		// deep sleep till next measure
		ee_data.sleep_sec = tm.getNextMeasuringSecLeft();

	}

	//----------------------------------------------------------------------------------
	//		// day mesurements
	//----------------------------------------------------------------------------------

	else if (tm.isDayHours()) {

		float minD = ee_data.am_wght - readWeight();
		ee_data.last_measure_time = tm.getTimeNow();

		sendMessage(minD, NULL);

		ee_data.sleep_sec = tm.getNextMeasuringSecLeft();


		Serial.printf("Day... minD = %f\n", minD);

	}
	
	//----------------------------------------------------------------------------------
	//	// night 10 pm
	//----------------------------------------------------------------------------------

	else if // > 21:45 & 22:59 <
		(tm.isSleepHour()) {

		float minD = ee_data.am_wght - readWeight();
		float Delta = readWeight() - ee_data.am_wght;

		sendMessage(minD, Delta);


		//preparation for a night sleep
		ee_data.sleep_sec = tm.getAMWakeUPSecons();

		ee_data.last_measure_time = tm.getTimeNow();
		ee_data.not_wifi_cnnct_times = (uint8_t)ceil(ee_data.sleep_sec / 3600);

		Serial.printf("10pm... Delta = %f\n", Delta);

		WriteEEPROMData();
		sleep_sec_mode(3600);
	}
	


	WriteEEPROMData();

	sleep_sec_mode(ee_data.sleep_sec);
	
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

void sleep_mode(uint8_t min) {

	Serial.printf("Sleep for %d min\n ", min);
	delay(10);
	if (WiFi.isConnected()) {
		client.stop();
		WiFi.disconnect();
	}

	delay(10);

	//ESP.deepSleep(20000);
	ESP.deepSleep(min * 60000000);

}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void sleep_sec_mode(uint32_t sec) {

	Serial.printf("Sleep for %d sec\n ", sec);
	
	delay(10);
	if (WiFi.isConnected()) {
		client.stop();
		WiFi.disconnect();
	}

	delay(10);
//	sec = 5;

//	ESP.deepSleep(sec * 100000);
	ESP.deepSleep(sec * 1000000);
	
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void reset() {
	Serial.println("..... reset ......");

	delay(10);
	if (WiFi.isConnected()) {
		client.stop();
		WiFi.disconnect();
	}
	delay(100);
	ESP.reset();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void initWifi()
{
	// Attempt to connect to Wifi network:
	//Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);
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
		//Serial.printf("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
		//	mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
		WiFi.begin(ssid, pass);
		delay(10000);
		//Serial.println("WiFi.RSSI() = " + String(WiFi.RSSI(),10));
	}
	Serial.printf("Connected to wifi %s.....\r\n", ssid);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float inline min(float amw, float mw) {
	return ((amw < mw) ? amw : mw);
}


