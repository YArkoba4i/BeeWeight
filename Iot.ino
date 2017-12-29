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

#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>



#include "config.h"

static bool messagePending = false;
static bool messageSending = true;

static int messageCount = 1;
static ESP8266WiFiClass esp;

static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;


void setup()
{
	pinMode(LED_PIN, OUTPUT);

	initSerial();
	//delay(2000);

//	readCredentials();

	initWifi();
	initTime();
	initSensor();

	azure_conect();
}


void loop()
{
	

	if (!messagePending && messageSending)
	{
		char messagePayload[MESSAGE_MAX_LEN];
		bool temperatureAlert = readMessage(messageCount, messagePayload);
		sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
		messageCount++;
		delay(INTERVAL);
	}
	IoTHubClient_LL_DoWork(iotHubClientHandle);
	delay(12000);
	//goToLightSleep();

}