


#include <OneWire.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "Sensores.h"
#include "Timing.h"
#include "RTCmemory.h"

//----------------------------------------------------------------------------------
//		Network connecting variables
//----------------------------------------------------------------------------------


char *ssid = "TP-LINK_5CF5A4";
char *pass = "****";
const char *_server = "184.106.153.149"; // thingspeak.com

										 //const int channelID = 332445;
const char *_APIKey = "****; // write API key for your ThingSpeak Channel
const char *_GET = "GET https://api.thingspeak.com/update?api_key=";

//----------------------------------------------------------------------------------
// Network connecting variables

//char *ssid = "TP-LINK_5CF5A4";
//char *pass = "****";
// char *ssid = "pr500k-27a997_3_RPT";
//char *pass = "****";


//----------------------------------------------------------------------------------

//ADC_MODE(ADC_VCC);





Sensores sensor;

Timing tm;
WiFiClient client;
RTCmemory rtcm;

//void initWifi();

/*void setup()
{
	
	delay(5000);
	// serial
	sensor.initSerial();
	

	tm.initTime();
	//rtcm.EraseRTSmemStr();
	//delay(2000);
	// checking the memory
	
	tm.setDateTime();
	tm.increaseTime(40);
	// initialization of sensores
	//sensor.initDHT();
	sensor.initHX711();
	sensor.initDallas();
	

//	setupWifi();


}

void loop()
{

	tm.printTime(tm.getTimeNow());
	sensor.printTemperature();
	

	Serial.println("\nWeight = " + String(sensor.readWeight(), 2));

//	wifiMsgSend(NULL, NULL);

//	Serial.println("\n Voltage = " + String(sensor.readExternalVoltage(), 4));
//	float temp = sensor.readDallasTemperature();


	//Serial.println("Temperature = " + String(temp, 2));
	delay(1000);
//	sleep_sec_mode(30);
}// test loop */


void setup()
{

	tm.initTime();
//	tm.setDateTime();
// serial
	sensor.initSerial();
//	delay(2000);

	tm.printTime(tm.getTimeNow());

	//rtcm.setRTCmemFlag(0xfffffff0);

//	rtcm.EraseRTSmemStr();

	// checking the memory
	if (!rtcm.isfirstRTCmemwrite()) {
		//Serial.println("\nNot first time");
		rtcm.readRTCmemData();
		
	}
	else { // if it's firs run. Setting up the structure
		//Serial.println("\nFirst time");
		rtcm.setRTCmemFlag(0xffffffff);
		rtcm.EraseRTSmemStr();
	}

	Serial.printf("rtcm.ee_data->not_wifi_cnnct_times = %d\n", rtcm.ee_data->not_wifi_cnnct_times);
	if (tm.isDay() && (rtcm.ee_data->not_wifi_cnnct_times == 0)) {
		

		Serial.println("Initalizing sensors");
		// initialization of sensores
		sensor.initDallas();
		if (!sensor.initHX711())
			reset();

	}
	else {// it's nigth
		  //eeprom write ee_data.not_wifi_cnnct_times--

		if ((rtcm.ee_data->sleep_sec < 3600) && (tm.getAMWakeUPSecons() < 3600)) {

			rtcm.ee_data->sleep_sec = tm.getAMWakeUPSecons();

			rtcm.ee_data->last_measure_time = tm.getTimeNow();
			rtcm.ee_data->not_wifi_cnnct_times = 0;

			sleep_sec_mode(rtcm.ee_data->sleep_sec);
		}
		else {
			rtcm.ee_data->not_wifi_cnnct_times--;
			rtcm.ee_data->sleep_sec -= 3600;

			rtcm.WriteRTCmemData();
			//WriteEEPROMData();
			sleep_sec_mode(3600);
		}
	}

}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void loop()
{
	
	//----------------------------------------------------------------------------------
	//		// if 6am
	//----------------------------------------------------------------------------------

	if (tm.isWakeUPHour()) { // > 05:55 && 06:10 <

		// saving 6 am weight to eeporm
		rtcm.ee_data->am_wght = sensor.readWeight();
		rtcm.ee_data->last_measure_time = tm.getTimeNow();

		//send message
		wifiMsgSend(NULL, NULL);


		//		Serial.printf("6am... eeprom.sleep_sec = %d\n", ee_data.sleep_sec);
	//	Serial.printf("6am... eeprom.am_wght = %f\n", rtcm.ee_data->am_wght);

		// deep sleep untill Measure_Hr
		rtcm.ee_data->sleep_sec = tm.getNextMeasuringSecLeft(Measure_Hr,0);
		
		rtcm.WriteRTCmemData();

		sleep_sec_mode(rtcm.ee_data->sleep_sec);

	}

	//----------------------------------------------------------------------------------
	//		// day mesurements
	//----------------------------------------------------------------------------------

	else if (tm.isDayHours()) { // > 06:55 && 21:50 <
 
		float minD = rtcm.ee_data->am_wght - sensor.readWeight();
		rtcm.ee_data->last_measure_time = tm.getTimeNow();

		//send message
		wifiMsgSend(minD, NULL);


		rtcm.ee_data->sleep_sec = tm.getNextMeasuringSecLeft();

		rtcm.WriteRTCmemData();

		sleep_sec_mode(rtcm.ee_data->sleep_sec);
	}

	//----------------------------------------------------------------------------------
	//	// night 10 pm
	//----------------------------------------------------------------------------------

	else if // > 21:55 & 22:10 <
		(tm.isSleepHour()) {

		float weight = sensor.readWeight();
		float minD = rtcm.ee_data->am_wght - weight;
		float Delta = weight - rtcm.ee_data->am_wght;


		//send message
		wifiMsgSend(minD, Delta);

		//preparation for a night sleep
		rtcm.ee_data->sleep_sec = tm.getAMWakeUPSecons();

		rtcm.ee_data->last_measure_time = tm.getTimeNow();

		//rtcm.ee_data->not_wifi_cnnct_times = (uint8_t)ceil(rtcm.ee_data->sleep_sec / 3600);
		rtcm.ee_data->not_wifi_cnnct_times = (uint8_t)floor(rtcm.ee_data->sleep_sec / 3600);

		//Serial.printf("10pm... Delta = %f\n", Delta);

		rtcm.WriteRTCmemData();

		sleep_sec_mode(3600);
	}


	//rtcm.WriteRTCmemData();
	//WriteEEPROMData();

	sleep_sec_mode(rtcm.ee_data->sleep_sec);

}//loop*/




//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

void sleep_mode(uint8_t min) {

	//Serial.printf("Sleep for %d min\n ", min);
	
	if (WiFi.isConnected()) {
		client.stop();
		WiFi.disconnect();

	}
	
	delay(100);

	/*tm.increaseTime(min);
	Serial.println("Updated time ");
	tm.printTime(tm.getTimeNow());
	int sec = 5;
	ESP.deepSleep(sec * 1000000, WAKE_NO_RFCAL);
	//*/

	//ESP.deepSleep(20000);
	ESP.deepSleep(min * 60000000, WAKE_NO_RFCAL);

}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void sleep_sec_mode(uint32_t sec) {


	if (WiFi.isConnected()) {
		client.stop();
		WiFi.disconnect();
	}

	delay(100);

	/*tm.increaseTime(sec);
	Serial.println("Updated time ");
	tm.printTime(tm.getTimeNow());
	sec = 5;
	// */

	//	ESP.deepSleep(sec * 100000, WAKE_RF_DEFAULT);
	ESP.deepSleep(sec * 1000000, WAKE_NO_RFCAL);

}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void reset() {
	//Serial.println("..... reset ......");

	delay(10);
	if (WiFi.isConnected()) {
		client.stop();
		WiFi.disconnect();
	}
	delay(100);
	ESP.reset();
}





//----------------------------------------------------------------------------------
//	initWifi()
//----------------------------------------------------------------------------------
void initWifi()
{
//	Serial.println("setupWifi...");
	WiFi.mode(WIFI_STA);
	delay(500);
	bool connected = false;

	for (size_t i = 0; i < 5; i++)
	{
		if(WiFi.status() != WL_CONNECTED)
			WiFi.begin(ssid, pass); // only do WiFi.begin if not already connected
		
		delay(2000);
		
		if (WiFi.status() == WL_CONNECTED) {
			connected = true;
			Serial.println("WiFi is connected");
			break;
		}
		else {
			Serial.println("Retry WiFi connection ...");
			delay(10000);
		}
		
	}

	if (!connected) {
		Serial.println("Connection Failed! Rebooting...");
		delay(500);
		sleep_mode(20);
	}

//	Serial.println("WiFi is ready");
}

void wifiMsgSend(float minD, float Delta)
{
	// connecting to Wifi
	initWifi();
	//setupWifi();


	for (size_t i = 0; i < 5; i++)
	{
		if (sendMessage(&client, &sensor, minD, Delta)) {
//			Serial.println("Message sent");
			return;
		}
//		Serial.printf("Retrying to send message %d time \n ", i);
	}
	
	

}

