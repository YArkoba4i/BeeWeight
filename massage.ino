
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const char *_server = "184.106.153.149"; // thingspeak.com

//const int channelID = 332445;
const char *_APIKey = "P1SOR94TRFN6E5DM"; // write API key for your ThingSpeak Channel
const char *_GET = "GET https://api.thingspeak.com/update?api_key=";



void sendMessage(WiFiClient *client, Sensores *sensor, float minDelta, float Delta) {


	if (client->connect(_server, 80)) {

		delay(500);

		// Update line
		//GET https ://api.thingspeak.com/update?api_key=P1SOR94TRFN6E5DM&field1=N&field2=N
		//	if (initSensores()) {
		float temp = sensor->readTemperature();
		float humid = sensor->readHumidity();
		float weight = sensor->readWeight();
		float voltage = sensor->getVoltage();

		// switch off HX711 to save power consumtion 
//		sensor->HX711_OFF();

		Serial.println("\n*****Sending Message*****\n");

		Serial.println("Weight = " + String(weight, 2));
		Serial.println("Temperature = " + String(temp, 2));
		Serial.println("Humidity = " + String(humid, 2));
		Serial.println("minDelta = " + String(minDelta, 2));
		Serial.println("Delta = " + String(Delta, 2));
		//tm.printTime();

		String cmd = _GET;
		cmd += _APIKey;

		///day
		if (Delta == NULL) {
			cmd += "&field1=" + String(temp, 2);			// field 1 = Temperature
			cmd += "&field2=" + String(humid, 2);			// field 2 = Humidity
			cmd += "&field3=" + String(weight, 2);			// field 3 = Weight
			cmd += "&field4=" + String(voltage, 2);	// field 4 = Vcc
			cmd += "&field5=" + String(minDelta, 2);		// field 5 = minDelta = weight at 06:00 - readWeight() (every INTERVAL)
			cmd += "\r\n";
		}/// morning
		else if (minDelta == NULL) {
			cmd += "&field1=" + String(temp, 2);			// field 1 = Temperature
			cmd += "&field2=" + String(humid, 2);			// field 2 = Humidity
			cmd += "&field3=" + String(weight, 2);			// field 3 = Weight
			cmd += "&field4=" + String(voltage, 2);	// field 4 = Vcc
			cmd += "\r\n";
		}
		else	///	evening	
		{
			cmd += "&field1=" + String(temp, 2);			// field 1 = Temperature
			cmd += "&field2=" + String(humid, 2);			// field 2 = Humidity
			cmd += "&field3=" + String(weight, 2);			// field 3 = Weight
			cmd += "&field4=" + String(voltage, 2);	// field 4 = Vcc
			cmd += "&field5=" + String(minDelta, 2);		// field 5 = minDelta = weight at 06:00 - readWeight() (every INTERVAL)
			cmd += "&field6=" + String(Delta, 2);			// field 6 = Delta = Weight at 22:00 - weight at 06:00
			cmd += "\r\n";
		}


		//int send_size = 
		client->print(cmd);

		//	Serial.printf("Send size = %i,  cmd size = %i \n",send_size, sizeof(cmd));

		Serial.println("\n*****Message send*****\n");
		delay(500);
	}
	else
	{
		Serial.println("******Message wasn't send******");
	}
}


//----------------------------------------------------------------------------------
//	initWifi()
//----------------------------------------------------------------------------------
void initWifi()
{
	// Attempt to connect to Wifi network:
	//Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);
	WiFi.mode(WIFI_STA);

	// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
	WiFi.begin(ssid, pass);
	//WiFi.begin("pr500k-27a997-RPT", "ce32fcda56211");
	int i = 5;
	while ((WiFi.status() != WL_CONNECTED) || (i == 0))
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
		i--;
	}
	Serial.printf("Connected to wifi %s.....\r\n", ssid);
}
