

#include <Wire.h>
#include <RTC_DS1307.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "Sensores.h"
#include "Timing.h"
#include "RTCmemory.h"


//----------------------------------------------------------------------------------
// Network connecting variables

char *ssid = "TP-LINK_5CF5A4";
char *pass = "60024060";
// char *ssid = "pr500k-27a997_3_RPT";
//char *pass = "ce32fcda56211";


//----------------------------------------------------------------------------------

ADC_MODE(ADC_VCC);



Sensores sensor;
Timing tm;
WiFiClient client;
RTCmemory rtcm;


void setup()
{
	sensor.initSerial();
	sensor.initDHT();
	sensor.initHX711();

	
	initWifi();

	rtcm.isfirstRTCmemwrite();
}

void loop()
{
	printf("Temperature = %f\n",sensor.readTemperature());
	printf("Weight = %f\n", sensor.readWeight());
	tm.DisplayTime();
	sendMessage(&client, &sensor, 0, 0);
	delay(2000);
  /* add main program code here */

}




