#include "Sensores.h"



Sensores::Sensores():DHT(DHT_PIN, DHT_TYPE), DallasTemperature(),HX711()
{}
	



Sensores::~Sensores()
{
	delete(this->dht);
	delete(this->scale);
	delete(this->dallas);
	delete(this->oneWire);
}

void Sensores::initDHT()
{
	this->dht = new DHT (DHT_PIN, DHT_TYPE);
	this->dht->begin();
}

void Sensores::initDallas()
{
	this->oneWire = new OneWire (ONE_WIRE_BUS);

	this->dallas = new DallasTemperature(this->oneWire);
	this->dallas->begin();
	Serial.print("Found ");
	Serial.print(this->dallas->getDeviceCount(), DEC);
	Serial.println(" devices.");

	// report parasite power requirements
	Serial.print("Parasite power is: ");
	if (this->dallas->isParasitePowerMode()) Serial.println("ON");
	else Serial.println("OFF");

	// Assign address manually. The addresses below will beed to be changed
	// to valid device addresses on your bus. Device address can be retrieved
	// by using either oneWire.search(deviceAddress) or individually via
	// sensors.getAddress(deviceAddress, index)
	// Note that you will need to use your specific address here
	//insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

	// Method 1:
	// Search for devices on the bus and assign based on an index. Ideally,
	// you would do this to initially discover addresses on the bus and then 
	// use those addresses and manually assign them (see above) once you know 
	// the devices on your bus (and assuming they don't change).
	if (!this->dallas->getAddress(this->insideThermometer, 0)) Serial.println("Unable to find address for Device 0");

	// method 2: search()
	// search() looks for the next device. Returns 1 if a new address has been
	// returned. A zero might mean that the bus is shorted, there are no devices, 
	// or you have already retrieved all of them. It might be a good idea to 
	// check the CRC to make sure you didn't get garbage. The order is 
	// deterministic. You will always get the same devices in the same order
	//
	// Must be called before search()
	//oneWire.reset_search();
	// assigns the first address found to insideThermometer
	//if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");

	// show the addresses we found on the bus
	Serial.print("Device 0 Address: ");
	this->printAddress(this->insideThermometer);
	Serial.println();

	// set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
	this->dallas->setResolution(this->insideThermometer, 9);

	Serial.print("Device 0 Resolution: ");
	Serial.print(this->dallas->getResolution(this->insideThermometer), DEC);
	Serial.println();
}

// function to print a device address
void Sensores::printAddress(DeviceAddress deviceAddress)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (deviceAddress[i] < 16) Serial.print("0");
		Serial.print(deviceAddress[i], HEX);
	}
}

// function to print the temperature for a device
void Sensores::printTemperature()
{
	// method 1 - slower
	//Serial.print("Temp C: ");
	//Serial.print(sensors.getTempC(deviceAddress));
	//Serial.print(" Temp F: ");
	//Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

	// method 2 - faster
	float tempC = this->dallas->getTempC(this->insideThermometer);
	Serial.print("Temp C: ");
	Serial.print(tempC);
	Serial.print(" Temp F: ");
	Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

// function to print the temperature for a device
float Sensores::readDallasTemperature()
{
	return this->dallas->getTempC(this->insideThermometer);
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
	Serial.println("readWeight");
	//return this->scale->get_units();
	return this->scale->get_units(MEASURE_TIMES);// -20.74;
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
//	printWeight()
//----------------------------------------------------------------------------------
void Sensores::printWeight()
{

	Serial.printf("Weight =%f", this->scale->get_units(MEASURE_TIMES));// -20.74;
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
