



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
	
	
	// serial
	sensor.initSerial();

	tm.initTime();
	//rtcm.EraseRTSmemStr();
	delay(2000);
	// checking the memory
	/*if (!rtcm.isfirstRTCmemwrite()) {
		Serial.println("\nNot first time");
		rtcm.readRTCmemData();
		Serial.printf("ee_data_str.am_wght = %f\n", rtcm.ee_data->am_wght);
		Serial.printf("ee_data_str.not_wifi_cnnct_times = %lu\n", rtcm.ee_data->not_wifi_cnnct_times);
		Serial.printf("ee_data_str.sleep_sec = %d\n", rtcm.ee_data->sleep_sec);
		Serial.printf("ee_data_str.last_measure_time = %d\n", rtcm.ee_data->last_measure_time);
	}
	else { // if it's firs run. Setting up the structure
		Serial.println("\nFirst time");
		rtcm.setRTCmemFlag(0xffffffff);
	}
	*/

	// initialization of sensores
	sensor.initDHT();
	sensor.initHX711();
	delay(2000);
	
	initWifi();

	/*if (rtcm.isfirstRTCmemwrite())
		rtcm.EraseRTSmemStr();
		*/

}

void loop()
{
//	rtcm.readRTCmemData();

	Serial.printf("hour = %d\n", tm.getHH());
	rtcm.ee_data->sleep_sec++;

	tm.printTime(tm.getTimeNow());
	 
	Serial.printf("Weight =%f", sensor.readWeight());
	Serial.printf("Temperature =%f", sensor.readTemperature());
	Serial.printf("Humidity =%f", sensor.readHumidity());
	//sendMessage(&client, &sensor,NULL, NULL);
//	rtcm.WriteRTCmemData();
	delay(5000);

}

/*
void setup()
{

	
	// serial
	sensor.initSerial();
	delay(2000);

	tm.printTime(tm.getTimeNow());
	
	//rtcm.setRTCmemFlag(0xfffffff0);

//	rtcm.EraseRTSmemStr();

	// checking the memory
	if (!rtcm.isfirstRTCmemwrite()) {
		Serial.println("\nNot first time");
		rtcm.readRTCmemData();
		Serial.printf("ee_data_str.am_wght = %f\n", rtcm.ee_data->am_wght);
		Serial.printf("ee_data_str.not_wifi_cnnct_times = %lu\n", rtcm.ee_data->not_wifi_cnnct_times);
		Serial.printf("ee_data_str.sleep_sec = %d\n", rtcm.ee_data->sleep_sec);
		Serial.printf("ee_data_str.last_measure_time = %d\n", rtcm.ee_data->last_measure_time);
	}
	else { // if it's firs run. Setting up the structure
		Serial.println("\nFirst time");
		rtcm.setRTCmemFlag(0xffffffff);
		rtcm.EraseRTSmemStr();
	}



	//readEEpromData();

	//	ee_data.not_wifi_cnnct_times = 0;

	Serial.println("if (tm.isDay())");
	if (tm.isDay()) {
			Serial.println("Initalizing sensors");
			// initialization of sensores
			sensor.initDHT();
			if (!sensor.initHX711())
				reset();

	}
	else {// it's nigth
		  //eeprom write ee_data.not_wifi_cnnct_times--

		if (rtcm.ee_data->sleep_sec < 3600) {

			rtcm.ee_data->sleep_sec = tm.getAMWakeUPSecons();

			rtcm.ee_data->last_measure_time = tm.Epoch64Time();
			rtcm.ee_data->not_wifi_cnnct_times = 0;

			sleep_sec_mode(rtcm.ee_data->sleep_sec);
		}
		else {
			rtcm.ee_data->not_wifi_cnnct_times--;
			rtcm.ee_data->sleep_sec -= 3600;
			Serial.printf("Night ... sleep minutes left = %d\n", rtcm.ee_data->sleep_sec);
			Serial.printf("Night ... wake up's left = %d\n", rtcm.ee_data->not_wifi_cnnct_times);

			rtcm.WriteRTCmemData();
			//WriteEEPROMData();
			sleep_sec_mode(3600);
		}
	}

	//tm.SetTime(tm.getSS(), tm.getMM(), 14, 1, 25, 3, 18);

	//if (tm.isFreshStart(tm.getTimeNow(), ee_data_str.mesure_time))
	//{
	//	Serial.println("Fresh start ... erasing eeprom.");
	//	EraseEEPROM();
	//}

}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void loop()
{

	tm.Epoch64Time();
	tm.printTime(tm.Epoch64Time());


	// connecting to Wifi
	initWifi();

	//----------------------------------------------------------------------------------
	//		// if 6am
	//----------------------------------------------------------------------------------

	if (tm.isWakeUPHour()) {

		// saving 6 am weight to eeporm
		rtcm.ee_data->am_wght = sensor.readWeight();
		rtcm.ee_data->last_measure_time = tm.Epoch64Time();

		
		sendMessage(&client, &sensor, NULL, NULL);


		//		Serial.printf("6am... eeprom.sleep_sec = %d\n", ee_data.sleep_sec);
		Serial.printf("6am... eeprom.am_wght = %f\n", rtcm.ee_data->am_wght);

		// deep sleep till next measure
		rtcm.ee_data->sleep_sec = tm.getNextMeasuringSecLeft();

	}

	//----------------------------------------------------------------------------------
	//		// day mesurements
	//----------------------------------------------------------------------------------

	else if (tm.isDayHours()) {

		float minD = rtcm.ee_data->am_wght - sensor.readWeight();
		rtcm.ee_data->last_measure_time = tm.Epoch64Time();


		sendMessage(&client, &sensor, minD, NULL);


		rtcm.ee_data->sleep_sec = tm.getNextMeasuringSecLeft();


		Serial.printf("Day... minD = %f\n", minD);

	}

	//----------------------------------------------------------------------------------
	//	// night 10 pm
	//----------------------------------------------------------------------------------

	else if // > 21:45 & 22:59 <
		(tm.isSleepHour()) {

		float minD = rtcm.ee_data->am_wght - sensor.readWeight();
		float Delta = sensor.readWeight() - rtcm.ee_data->am_wght;


		sendMessage(&client, &sensor, minD, Delta);

		//preparation for a night sleep
		rtcm.ee_data->sleep_sec = tm.getAMWakeUPSecons();

		rtcm.ee_data->last_measure_time = tm.Epoch64Time();
		rtcm.ee_data->not_wifi_cnnct_times = (uint8_t)ceil(rtcm.ee_data->sleep_sec / 3600);

		Serial.printf("10pm... Delta = %f\n", Delta);

		rtcm.WriteRTCmemData();

		sleep_sec_mode(3600);
	}


	rtcm.WriteRTCmemData();
	//WriteEEPROMData();

	sleep_sec_mode(rtcm.ee_data->sleep_sec);

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
	ESP.deepSleep(min * 60000000, RF_DISABLED);

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
	sec = 10;

	//	ESP.deepSleep(sec * 100000, WAKE_RF_DEFAULT);
	ESP.deepSleep(sec * 1000000, WAKE_NO_RFCAL);

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


*/


