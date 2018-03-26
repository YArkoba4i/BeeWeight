#include "Sensores.h"



Sensores::Sensores():DHT(DHT_PIN, DHT_TYPE),HX711()
{}
	



Sensores::~Sensores()
{
	delete(this->dht);
	delete(this->scale);
}

void Sensores::initDHT()
{
	this->dht = new DHT (DHT_PIN, DHT_TYPE);
	this->dht->begin();
}

bool Sensores::initHX711() {

	this->scale = new HX711(_DOUT, _CLK);

	for (uint8_t i = 0; i < 5; i++) {

		this->scale->set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
											 //scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
		this->scale->power_up();
		delay(500);
		//scale.set_gain(128);
		if (this->scale->is_ready()) {
			Serial.println("scale.is_ready:");
			return true;
		}
		else
		{
			Serial.println("scale.is_NOT_ready:");
			this->scale->power_down();
			delay(2000);
		}
	}
	this->scale = NULL;
	return false;
}


//----------------------------------------------------------------------------------
//	HX711_OFF()
//----------------------------------------------------------------------------------
void Sensores::HX711_OFF() {

	this->scale->power_down();
	delay(500);

}
//----------------------------------------------------------------------------------
//	readTemperature()
//----------------------------------------------------------------------------------
float Sensores::readTemperature()
{

	return this->dht->readTemperature();
}
//----------------------------------------------------------------------------------
//	readHumidity()
//----------------------------------------------------------------------------------
float Sensores::readHumidity()
{

	return this->dht->readHumidity();
}

//----------------------------------------------------------------------------------
//	readWeight()
//----------------------------------------------------------------------------------
float Sensores::readWeight()
{

	return this->scale->get_units(MEASURE_TIMES) - 20.74;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float Sensores::getVoltage() {

	float vccVolt = ((float)ESP.getVcc()*calibrVcc) / 1024;
	//	Serial.println("Voltage = " + String(vccVolt, 1));
	return vccVolt;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Sensores::setTare() {


	uint count = 10;
	while (count > 0)
	{
		Serial.println("... Set up Tare to zero" + String(count) + " sec ...");

		this->scale->tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

		Serial.println("Tare is set to zero");

		count--;
	}

}


//----------------------------------------------------------------------------------
//	initSerial()
//----------------------------------------------------------------------------------
void Sensores::initSerial()
{
	// Start serial and initialize stdout
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println("Serial successfully inited.");
}

/* Read a string whose length should in (0, lengthLimit) from Serial and save it into buf.
*
*        prompt   - the interact message and buf should be allocaled already and return true.
*        buf      - a part of memory which is already allocated to save string read from serial
*        maxLen   - the buf's len, which should be upper bound of the read-in string's length, this should > 0
*        timeout  - If after timeout(ms), return false with nothing saved to buf.
*                   If no timeout <= 0, this function will not return until there is something read.
*/
bool Sensores::readFromSerial(char *prompt, char *buf, int maxLen, int timeout)
{
	int timer = 0, delayTime = 1000;
	String input = "";
	if (maxLen <= 0)
	{
		// nothing can be read
		return false;
	}

	Serial.println(prompt);
	while (1)
	{
		input = Serial.readString();
		int len = input.length();
		if (len > maxLen)
		{
			Serial.printf("Your input should less than %d character(s), now you input %d characters.\r\n", maxLen, len);
		}
		else if (len > 0)
		{
			// save the input into the buf
			sprintf(buf, "%s", input.c_str());
			return true;
		}

		// if timeout, return false directly
		timer += delayTime;
		if (timeout > 0 && timer >= timeout)
		{
			Serial.println("You input nothing, skip...");
			return false;
		}
		// delay a time before next read
		delay(delayTime);
	}
}
