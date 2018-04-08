#pragma once
#include <hx711.h>
#include <DHT.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include "config.h"


class Sensores : public DHT, public DallasTemperature, public HX711
{
private:
	// arrays to hold device address
	DeviceAddress insideThermometer;
	// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	OneWire *oneWire;

	DallasTemperature * dallas;
	DHT * dht;
	HX711 * scale;
	void printAddress(DeviceAddress deviceAddress);
public:
	Sensores();
	~Sensores();
	void initDHT();
	void initDallas();
	bool initHX711();
	void HX711_OFF();
	float readDallasTemperature();
	float readTemperature();
	float readHumidity();
	float readWeight();
	float getVoltage();
	void printTemperature();
	void printWeight();
	void setTare();
	void initSerial();
	bool readFromSerial(char *prompt, char *buf, int maxLen, int timeout);
};
