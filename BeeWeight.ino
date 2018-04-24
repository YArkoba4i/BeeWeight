


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
char *pass = "60024060";
const char *_server = "184.106.153.149"; // thingspeak.com

										 //const int channelID = 332445;
const char *_APIKey = "P1SOR94TRFN6E5DM"; // write API key for your ThingSpeak Channel
const char *_GET = "GET https://api.thingspeak.com/update?api_key=";

//----------------------------------------------------------------------------------
// Network connecting variables

//char *ssid = "TP-LINK_5CF5A4";
//char *pass = "60024060";
// char *ssid = "pr500k-27a997_3_RPT";
//char *pass = "ce32fcda56211";


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
	


	// initialization of sensores
	//sensor.initDHT();
//	sensor.initHX711();
//	sensor.initDallas();
	

//	setupWifi();


}

void loop()
{

	tm.printTime(tm.getTimeNow());
	


//	Serial.println("\n Voltage = " + String(sensor.readExternalVoltage(), 4));

	
	delay(1000);

}// test loop */


void setup()
{

	tm.initTime();
	
// serial
//	sensor.initSerial();
//	delay(2000);

//	tm.printTime(tm.getTimeNow());

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


	if (tm.isDay() && (rtcm.ee_data->not_wifi_cnnct_times == 0)) {
		
		//Serial.println("Initalizing sensors");
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

		// deep sleep till next measure
		rtcm.ee_data->sleep_sec = tm.getNextMeasuringSecLeft(Wk_UP_Hr+1,0);
		
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

		float minD = rtcm.ee_data->am_wght - sensor.readWeight();
		float Delta = sensor.readWeight() - rtcm.ee_data->am_wght;


		//send message
		wifiMsgSend(minD, Delta);

		//preparation for a night sleep
		rtcm.ee_data->sleep_sec = tm.getAMWakeUPSecons();

		rtcm.ee_data->last_measure_time = tm.getTimeNow();
		rtcm.ee_data->not_wifi_cnnct_times = (uint8_t)ceil(rtcm.ee_data->sleep_sec / 3600);

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

	delay(10);
	if (WiFi.isConnected()) {
		client.stop();
		WiFi.disconnect();
	}

	delay(10);

//	tm.increaseTime(sec);
//	Serial.println("Updated time ");
//	tm.printTime(tm.getTimeNow());
//	sec = 15;

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
	// Attempt to connect to Wifi network:
//	Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);
	WiFi.mode(WIFI_STA);

	// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
	WiFi.begin(ssid, pass);
	//WiFi.begin("TP-LINK_5CF5A4", "60024060");

	while ((WiFi.status() != WL_CONNECTED))
	{
		// Get Mac Address and show it.
		// WiFi.macAddress(mac) save the mac address into a six length array, but the endian may be different. The huzzah board should
		// start from mac[0] to mac[5], but some other kinds of board run in the oppsite direction.
		uint8_t mac[6];
		WiFi.macAddress(mac);
//		Serial.printf("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
//			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
		WiFi.begin(ssid, pass);
	//	WiFi.begin("TP-LINK_5CF5A4", "60024060");
		delay(10000);
//		Serial.println("WiFi.RSSI() = " + String(WiFi.RSSI(), 10));

	}
//	Serial.printf("Connected to wifi %s.....\r\n", ssid);
}

void setupWifi() {
//	Serial.begin(115200);
//	delay(2000);
//	Serial.println("Booting...---");
	WiFi.mode(WIFI_STA);
//	delay(500);
	int teltje = 0;
//	pinMode(LED_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
	while (WiFi.status() != WL_CONNECTED) {
		delay(250);
		//digitalWrite(LED_PIN, HIGH);
		delay(250);
		//digitalWrite(LED_PIN, LOW);
		if (teltje == 0) {
			WiFi.begin(ssid, pass); // only do WiFi.begin if not already connected
		}
		teltje++;
	//	Serial.print(".");
		if (teltje>20) {
	//		Serial.println("Connection Failed! Rebooting...");
			delay(500);
			reset();
		}
	}
//	Serial.println("Ready");
//	Serial.print("IP address: ");
//	Serial.println(WiFi.localIP());
}


void wifiMsgSend(float minD, float Delta)
{
	// connecting to Wifi

	setupWifi();


	for (size_t i = 0; i < 5; i++)
	{
		if (sendMessage(&client, &sensor, minD, Delta)) {
		//	Serial.println("Message sent");
			return;
		}
		
	}
	
}

