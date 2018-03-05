#pragma once
#include <time.h>
#include <stdint.h>
#include <Arduino.h>

#include "config.h"


class Timing
{
private:
	const uint two_days = 115200;
public:
	Timing();
	~Timing();
	void initTime();
	time_t getTimeNow();
	uint8_t getHH();
	uint8_t getMM();
	uint16_t getAMwkUPmins();
	bool isFreshStart(time_t timeNow, time_t mesure_time);
	bool isDay();
	uint16_t getNextMeasuringMMLeft();
};


