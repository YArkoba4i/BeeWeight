#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <hx711.h>
#include "config.h"


/*
 struct HX711_ports {
	unsigned short int _DOUT;
	unsigned short int _CLK;

} HX711_dev[5];
*/
//HX711_dev[0]._CLK = 4;


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
void initSensor()
{
    dht.begin();
	
	
	scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
	//scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
	scale.power_up();
	
	//scale.set_gain(128);
	if (scale.is_ready()) {
		Serial.println("scale.is_ready:");
	}
	else
	{
		Serial.println("scale.is_NOT_ready:");

	}

	Serial.println("digitalRead(4) = " + String(digitalRead(4)));

	Serial.println("digitalRead(14) = " + String(digitalRead(14)));

	
}

void HX711_OFF() {

	scale.power_down();
}

float readTemperature()
{

    return dht.readTemperature();
}

float readHumidity()
{

	return dht.readHumidity();
}


float readWeight()
{

	return scale.get_units(MEASURE_TIMES);
}

#endif


void sendMessage() {

	// Update line
	//GET https ://api.thingspeak.com/update?api_key=P1SOR94TRFN6E5DM&field1=N&field2=N

	float temp = readTemperature();
	float humid = readHumidity();
	float weight = readWeight()-20.5;

	Serial.println("Weight = " + String(weight, 2));
	Serial.println("Temperature = " + String(temp, 2));
	Serial.println("Humidity = " + String(humid, 2));
	String cmd = _GET;
	cmd += _APIKey;
	cmd += "&field1=" + String(temp, 2);		// field 1 = Temperature
	cmd += "&field2=" + String(humid, 2);		// field 2 = Humidity
	cmd += "&field3=" + String(weight, 2);		// field 3 = Weight
	cmd += "&field4=" + String(getVoltage(), 2);// field 4 = Vcc
	cmd += "\r\n";

	client.print(cmd);
	//Serial.println(cmd);
	Serial.println("Message send");
/*	// Read all the lines of the reply from server and print them to Serial
	while (client.available()) {
		String line = client.readStringUntil('\r');
		Serial.print(line);
	}
*/

	
}

void parseTwinMessage(char *message)
{
	StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
	JsonObject &root = jsonBuffer.parseObject(message);
	if (!root.success())
	{
		Serial.printf("Parse %s failed.\r\n", message);
		return;
	}

	if (root["desired"]["interval"].success())
	{
		interval = root["desired"]["interval"];
	}
	else if (root.containsKey("interval"))
	{
		interval = root["interval"];
	}
}

float getVoltage() {

	float vccVolt = ((float)ESP.getVcc()*calibrVcc) / 1024;
	Serial.println("Voltage = " + String(vccVolt, 1));
	return vccVolt;
}

void setTare() {

	delay(1000);
	uint count = 10;
	while (count > 0)
	{
		Serial.println("... Set up Tare to zero" + String(count) + " sec ...");
		uint temp = digitalRead(TARE_BTN_PIN);

		if (temp == HIGH) {
			digitalWrite(LED_PIN1, HIGH);

			scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

			Serial.println("Tare is set to zero");
			delay(1000);
		}
		else {
			digitalWrite(LED_PIN1, LOW);

			delay(1000);
		}

		count--;
	}

}