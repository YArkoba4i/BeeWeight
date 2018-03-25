#pragma once
#include <time.h>


#include <Wire.h>
#include <RTC_DS1307.h>

#include "config.h"


class Timing : public RTC_DS1307
{
private:
	const uint two_days = 115200;
public:
	Timing();
	~Timing();
	void initTime();
	
	void printTime(time_t time);
	time_t getTimeNow();
	uint16_t getYear();
	uint8_t getHH();
	uint8_t getMM();
	uint8_t getSS();
	uint16_t getAMwkUPmins();
	uint32_t getAMWakeUPSecons();
	bool isDay();
	bool isFreshStart(time_t timeNow, time_t mesure_time);
	bool isDayHours();
	bool isWakeUPHour();
	bool isSleepHour();
	uint16_t getNextMeasuringMinLeft();
	uint	getNextMeasuringSecLeft();
	uint	getNextMeasuringSecLeft(uint8_t hour, uint8_t minute);
};


