




bool sendMessage(WiFiClient *client, Sensores *sensor, float minDelta, float Delta) {


	if (client->connect(_server, 80)) {

		delay(500);

		// Update line
		//GET https ://api.thingspeak.com/update?api_key=P1SOR94TRFN6E5DM&field1=N&field2=N
		//	if (initSensores()) {
		float temp = sensor->readDallasTemperature();
		float humid = 0;// sensor->readHumidity();
		float weight = sensor->readWeight();
		float voltage = sensor->readExternalVoltage();
		minDelta = minDelta * (-1);
		// switch off HX711 to save power consumtion 
//		sensor->HX711_OFF();

		/*Serial.println("\n*****Sending Message*****\n");

		Serial.println("Weight = " + String(weight, 2));
		Serial.println("Temperature = " + String(temp, 2));
		Serial.println("Humidity = " + String(humid, 2));
		Serial.println("minDelta = " + String(minDelta, 2));
		Serial.println("Delta = " + String(Delta, 2));
		//tm.printTime();*/

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
		
		//Serial.println("\n*****Message send*****\n");
		delay(500);
		return true;
	}
	else
	{
		return false;
		//Serial.println("******Message wasn't send******");
	}
}


