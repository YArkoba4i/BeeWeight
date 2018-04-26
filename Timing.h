#pragma once
#include <time.h>


#include <RtcDS1307.h>
#include <Wire.h>
#include "config.h"

#define countof(a) (sizeof(a) / sizeof(a[0]))
//RtcDS1307<TwoWire> Rtc(Wire);



class Timing : RtcDS1307<TwoWire>
{
private:
	
	const uint two_days = 115200;

public:
	Timing();
	~Timing();

	void initTime();
	
	void printTime(time_t time);
	void printDateTime(const RtcDateTime& dt);
	void setDateTime();
	void increaseTime(uint min);
	time_t getTimeNow();
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