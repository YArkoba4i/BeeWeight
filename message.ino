#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <hx711.h>
#include "config.h"


#if SIMULATED_DATA

void initSensor()
{
    // use SIMULATED_DATA, no sensor need to be inited
}

float readTemperature()
{
    return random(20, 30);
}

float readHumidity()
{
    return random(30, 40);
}


float readWeight()
{
	return random(1, 190);
}


#else

static DHT dht(DHT_PIN, DHT_TYPE);

static HX711 scale(_DOUT, _CLK);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool initSensores()
{
	for (uint8_t i = 0; i < 5; i++) {
		dht.begin();


		scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
											 //scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
		scale.power_up();
		delay(500);
		//scale.set_gain(128);
		if (scale.is_ready()) {
			Serial.println("scale.is_ready:");
			return true;
		}
		else
		{
			Serial.println("scale.is_NOT_ready:");
			scale.power_down(); 
			delay(2000);
		}
	}

	return false;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void HX711_OFF() {

	scale.power_down();
	delay(500);

}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float readTemperature()
{
	
    return dht.readTemperature();
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float readHumidity()
{

	return dht.readHumidity();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float readWeight()
{

	return scale.get_units(MEASURE_TIMES) - 20.74;
}

#endif
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void sendMessage(float minDelta, float Delta) {

	if (client.connect(_server, 80)) {

		delay(500);

		// Update line
		//GET https ://api.thingspeak.com/update?api_key=P1SOR94TRFN6E5DM&field1=N&field2=N
	//	if (initSensores()) {
			float temp = readTemperature();
			float humid = readHumidity();
			float weight = readWeight();

			// switch off HX711 to save power consumtion 
			HX711_OFF();

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
				cmd += "&field4=" + String(getVoltage(), 2);	// field 4 = Vcc
				cmd += "&field5=" + String(minDelta, 2);		// field 5 = minDelta = weight at 06:00 - readWeight() (every INTERVAL)
				cmd += "\r\n";
			}/// morning
			else if (minDelta == NULL) {
				cmd += "&field1=" + String(temp, 2);			// field 1 = Temperature
				cmd += "&field2=" + String(humid, 2);			// field 2 = Humidity
				cmd += "&field3=" + String(weight, 2);			// field 3 = Weight
				cmd += "&field4=" + String(getVoltage(), 2);	// field 4 = Vcc
				cmd += "\r\n";
			}
			else	///	evening	
			{
				cmd += "&field1=" + String(temp, 2);			// field 1 = Temperature
				cmd += "&field2=" + String(humid, 2);			// field 2 = Humidity
				cmd += "&field3=" + String(weight, 2);			// field 3 = Weight
				cmd += "&field4=" + String(getVoltage(), 2);	// field 4 = Vcc
				cmd += "&field5=" + String(minDelta, 2);		// field 5 = minDelta = weight at 06:00 - readWeight() (every INTERVAL)
				cmd += "&field6=" + String(Delta, 2);			// field 6 = Delta = Weight at 22:00 - weight at 06:00
				cmd += "\r\n";
			}


			int send_size = client.print(cmd);

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
//
//----------------------------------------------------------------------------------
float getVoltage() {

	float vccVolt = ((float)ESP.getVcc()*calibrVcc) / 1024;
//	Serial.println("Voltage = " + String(vccVolt, 1));
	return vccVolt;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void setTare() {


	uint count = 10;
	while (count > 0)
	{
		Serial.println("... Set up Tare to zero" + String(count) + " sec ...");

		scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

		Serial.println("Tare is set to zero");

		count--;
	}

}