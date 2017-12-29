


void blinkLED()
{
	digitalWrite(LED_PIN, HIGH);
	delay(500);
	digitalWrite(LED_PIN, LOW);
}

void initWifi()
{
	// Attempt to connect to Wifi network:
	Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);


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

void azure_conect() {
	// initIoThubClient();

	iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
	if (iotHubClientHandle == NULL)
	{
		Serial.println("Failed on IoTHubClient_CreateFromConnectionString.");
		while (1);
	}

	IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);
	IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL);
	IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL);
}

void goToLightSleep()
{

	Serial.println("Going for a deep sleep");
	
	//esp.deepSleep(3600000000); // one hour
	//setup();
	Serial.println("awaken");

		Serial.println("switching off Wifi Modem and CPU");
		//stop any clients
		///thisclient.stop();
		delay(1000);

		esp.forceSleepBegin(deep_sleep_time); // 1min

		WiFi.disconnect();
		WiFi.mode(WIFI_OFF);
		// how do we call this: 
		//gpio_pin_wakeup_enable(GPIO_ID_PIN(switchPin),GPIO_PIN_INTR_NEGEDGE);
		//wifi_fpm_open();
		//wifi_fpm_do_sleep(26843455);
		//if(WiFi.forceSleepBegin(26843455)) sleep = true;
	
}