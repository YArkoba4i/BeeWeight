#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <HX711.h>




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
	scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
	scale.power_up();
	//scale.set_gain(128);
	if (scale.is_ready())
		Serial.println("scale.is_ready:");
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

bool readMessage(int messageId, char *payload)
{
    float temperature = readTemperature();
    float humidity = readHumidity();
	float weight = readWeight();

    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;
    bool sensor_Alert = false;

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
			sensor_Alert = true;
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

	// NAN is not the valid json, change it to NULL
	if (std::isnan(weight))
	{
		root["weight"] = NULL;
	}
	else
	{
		root["weight"] = weight;
		if(!sensor_Alert)	// if no alert -> check what's with weight
			if (weight < 0.0 || weight > 200.0) 
			{
				sensor_Alert = true;
			}
	}

    root.printTo(payload, MESSAGE_MAX_LEN);
    return sensor_Alert;
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
