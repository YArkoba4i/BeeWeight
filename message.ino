#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <HX711.h>


#if DHT_HX711	// If temperature sensor is used

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

#else

static DHT dht(DHT_PIN, DHT_TYPE);
void initSensor()
{
    dht.begin();
}

float readTemperature()
{
    return dht.readTemperature();
}

float readHumidity()
{
    return dht.readHumidity();
}

#endif

bool readMessage(int messageId, char *payload)
{
    float temperature = readTemperature();
    float humidity = readHumidity();
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;
    bool temperatureAlert = false;

    // NAN is not the valid json, change it to NULL
    if (std::isnan(temperature))
    {
        root["temperature"] = NULL;
    }
    else
    {
        root["temperature"] = temperature;
        if (temperature > TEMPERATURE_ALERT)
        {
            temperatureAlert = true;
        }
    }

    if (std::isnan(humidity))
    {
        root["humidity"] = NULL;
    }
    else
    {
        root["humidity"] = humidity;
    }
    root.printTo(payload, MESSAGE_MAX_LEN);
    return temperatureAlert;
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
#else
#if SIMULATED_DATA	// If load cell is used

void initSensor()
{
	// use SIMULATED_DATA, no sensor need to be inited
}



float readWeight()
{
	return random(1, 190);
}


#else

static HX711 scale(_DOUT, _CLK);



void initSensor()
{
	
	scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
	scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
	scale.power_up();
	//scale.set_gain(128);
	if (scale.is_ready())
		Serial.println("scale.is_ready:");
}

float readWeight()
{
	return scale.get_units(MEASURE_TIMES);
}


#endif

bool readMessage(int messageId, char *payload)
{
	float weight = readWeight();

	StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	root["deviceId"] = DEVICE_ID;
	root["messageId"] = messageId;
	bool weightAlert = false;

	// NAN is not the valid json, change it to NULL
	if (std::isnan(weight))
	{
		root["weight"] = NULL;
	}
	else
	{
		root["weight"] = weight;
		if (weight < 0.0 || weight > 200.0)
		{
			weightAlert = true;
		}
	}


	root.printTo(payload, MESSAGE_MAX_LEN);
	return weightAlert;
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
#endif