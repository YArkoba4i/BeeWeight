#pragma once
#include <hx711.h>
#include <DHT.h>
#include <DallasTemperature.h>
#include "config.h"

class Sensores : public DHT, public HX711
{
private:

	DHT * dht;
	HX711 * scale;
public:
	Sensores();
	~Sensores();
	void initDHT();
	bool initHX711();
	void HX711_OFF();
	float readTemperature();
	float readHumidity();
	float readWeight();
	float getVoltage();
	void printWeight();
	void setTare();
	void initSerial();
	bool readFromSerial(char *prompt, char *buf, int maxLen, int timeout);
};
