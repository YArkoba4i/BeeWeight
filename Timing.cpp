#include "Timing.h"


Timing::Timing()
{
	TwoWire twr;

	twr.begin();
}


Timing::~Timing()
{
}


//----------------------------------------------------------------------------------
// getTimeNow(
//----------------------------------------------------------------------------------
time_t Timing::getTimeNow() {
	RTC_DS1307 rtc;
	
	return rtc.UnixTimestamp();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
uint8_t Timing::getHH() {

	byte hh;

	RTC_DS1307 rtc;

	rtc.GetTime(0,0,&hh,0,0,0,0);

	return hh;
}


//----------------------------------------------------------------------------------
//		uint8_t Timing::getMM()
//----------------------------------------------------------------------------------
uint8_t Timing::getMM() {
	byte mm;

	RTC_DS1307 rtc;

	rtc.GetTime(0, &mm, 0, 0, 0, 0, 0);

	return mm;
}


//----------------------------------------------------------------------------------
//		uint8_t Timing::getSS()
//----------------------------------------------------------------------------------
uint8_t Timing::getSS() {
	byte ss;

	RTC_DS1307 rtc;

	rtc.GetTime(&ss, 0, 0, 0, 0, 0, 0);

	return ss;
}

//----------------------------------------------------------------------------------
//	uint16_t Timing::getAMwkUPmins()
//----------------------------------------------------------------------------------
uint16_t Timing::getAMwkUPmins() {

	
	if (getHH()  < Wk_UP_Hr) // if night
	{
		//Serial.println("Night");
		return  (60 * (Wk_UP_Hr - (getHH() + 1))) + (60 - getMM());
	}
	else if (getHH() > Wk_UP_Hr)
	{
		return (60 - getMM()) + ((24 - ((getHH() + 1) - Wk_UP_Hr)) * 60);
	}
	else
	{
		return 0;
	}

}


//----------------------------------------------------------------------------------
//	uint16_t Timing::getAMWakeUPSecons()
//----------------------------------------------------------------------------------
uint32_t Timing::getAMWakeUPSecons() {
	RTC_DS1307 rtc;

	time_t next = rtc.UnixTimestamp();
	printTime(next);

	time(&next);
	struct tm beg;
	beg = *localtime(&next);

	beg.tm_mday++;
	beg.tm_hour = 6;
	beg.tm_min = 0;
	beg.tm_sec = 0;

	next = mktime(&beg);

	Serial.println("Wake UP time is ");
	printTime(next);


	uint32_t ss_left = (uint32_t)difftime(next, getTimeNow());

	return ss_left;

}


//----------------------------------------------------------------------------------
//		isFreshStart
//----------------------------------------------------------------------------------
bool Timing::isFreshStart(time_t timeNow, time_t mesure_time) {
	//;
	time_t timeDiff = (timeNow - mesure_time);

	struct tm * timeinfo;
	timeinfo = localtime(&timeDiff);
	Serial.printf("Time Diff = %d\n", timeinfo->tm_hour);

	if ((uint)timeinfo->tm_hour > this->two_days) {

		return true;
	}
	else
		return false;
}



//----------------------------------------------------------------------------------
//	isDay()
//----------------------------------------------------------------------------------
bool Timing::isDay() {


	if ((getHH() >= Wk_UP_Hr - 1) && (getHH() <= Sleep_Hr))
	{

		Serial.printf("Hour of a day = %d\n", getHH());
		return true;
	}
	else {

		Serial.printf("Hour of night = %d\n", getHH());
		return false;
	}
}



//----------------------------------------------------------------------------------
//	isWakeUPHour()
//----------------------------------------------------------------------------------
bool Timing::isWakeUPHour() {


	if ((getHH() == (Wk_UP_Hr - 1))  // > 05:00 
		|| ((getHH() == Wk_UP_Hr) && (getMM() < 10))) // 6:10 <

	{
		return true;
	}
	else
	{
		return false;
	}

}


//----------------------------------------------------------------------------------
//	isDayHours()
//----------------------------------------------------------------------------------
bool Timing::isDayHours() {


	if (((getHH() == Wk_UP_Hr) && (getMM() >= 10)) // > 06:10 & 07:00 <
		|| ((getHH() > Wk_UP_Hr) && (getHH() < Sleep_Hr - 1)) // > 6:00 & 21:00 <
		|| ((getHH() == (Sleep_Hr - 1)) && (getMM() <= 50))) // 21:50 <=
	{
		return true;
	}
	else
	{
		return false;
	}

}


//----------------------------------------------------------------------------------
//	isSleepHour()
//----------------------------------------------------------------------------------
bool Timing::isSleepHour() {


	if (((getHH() == (Sleep_Hr - 1)) && (getMM() > 50)) // > 21:50
		|| (getHH() == Sleep_Hr)) // > 22:00
	{
		return true;
	}
	else
	{
		return false;
	}

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
uint16_t Timing::getNextMeasuringMinLeft() {

	double  intpart;

	modf((double)getMM() / INTERVAL, &intpart);

	uint8_t mm_left = (uint8_t)((intpart + 1) * INTERVAL - getMM());

	if (mm_left < 2)
		mm_left = INTERVAL;

	return mm_left;

}



//----------------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------------
uint Timing::getNextMeasuringSecLeft() {


	time_t next = getTimeNow();
	printTime(next);

	double  intpart;

	double fractpart = modf((double)getMM() / INTERVAL, &intpart);

	// getting next minute interval measure
	uint nextMin = (uint)((intpart + ceil(fractpart))*INTERVAL);


	// time structure
	time(&next);
	struct tm beg;
	beg = *localtime(&next);

	// setting next minute interval measure
	if (nextMin == 60) { nextMin = 0; beg.tm_hour++; }
	beg.tm_min = nextMin;
	beg.tm_sec = 0;

	next = mktime(&beg);

	Serial.println("Next measuring time is ");
	printTime(next);

	uint ss_left = (uint)fabs(difftime(next, getTimeNow()));
	Serial.printf("ss_left = %d\n", ss_left);
	if (ss_left < 180) {

		next += INTERVAL * 60;

		Serial.println("Next measuring time is ");
		printTime(next);


		uint ss_left = (uint)fabs(difftime(next, getTimeNow()));
		return ss_left;
	}
	else
	{
		return ss_left;
	}
}

//----------------------------------------------------------------------------------
//	getNextMeasuringSecLeft
//----------------------------------------------------------------------------------
uint Timing::getNextMeasuringSecLeft(uint8_t hour, uint8_t minute) {


	time_t next = getTimeNow();
	//	printTime(next);

	time(&next);
	struct tm beg;
	beg = *localtime(&next);

	double  intpart;
	modf((double)getMM() / INTERVAL, &intpart);
	beg.tm_hour = hour;
	beg.tm_min = minute;
	beg.tm_sec = 0;


	next = mktime(&beg);

	Serial.println("Next measuring time is ");
	printTime(next);

	uint ss_left = (uint)fabs(difftime(next, getTimeNow()));

	if (ss_left < 240) {
		beg.tm_min = ++intpart * INTERVAL;
		next = mktime(&beg);

		Serial.println("Next measuring time is ");
		printTime(next);


		uint ss_left = (uint)fabs(difftime(next, getTimeNow()));
		return ss_left;
	}
	else
	{
		return ss_left;
	}
}
//----------------------------------------------------------------------------------
//	printTime(time_t time)
//----------------------------------------------------------------------------------
void Timing::printTime(time_t time) {
	Serial.printf("%s\n", asctime(gmtime(&time)));
}
