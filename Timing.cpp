#include "Timing.h"



Timing::Timing()
{
}


Timing::~Timing()
{
}
void Timing::initTime() {
	configTime(3600*9, 0, "pool.ntp.org", "time.nist.gov");
	Serial.println("\nWaiting for time");
	while (!time(NULL)) {
		Serial.print(".");
		delay(500);
	}


	while (true)
	{
		time_t  time_now = time(NULL);

		if (time_now == 0)
		{
			Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
			delay(2000);
		}
		else
		{
			Serial.printf("Fetched NTP epoch time is: %lu.\r\n", time_now);
			break;
		}
	}
}
time_t Timing::getTimeNow() {
	time_t  time_now = time(NULL);

	return time_now;
}

uint8_t Timing::getHH() {
	time_t  time_now = time(NULL);
	struct tm * timeinfo;

	timeinfo = localtime(&time_now);
	//Serial.printf("getHH() = %d\n", timeinfo->tm_hour);
	return timeinfo->tm_hour;
}
uint8_t Timing::getMM() {
	time_t  time_now = time(NULL);
	struct tm * timeinfo;

	timeinfo = localtime(&time_now);
	//Serial.printf("getMM() = %d\n", timeinfo->tm_min);
	return timeinfo->tm_min;
}


uint16_t Timing::getAMwkUPmins() {

	
	if (getHH() < Wk_UP_Hr) // if night
	{

		return  (Wk_UP_Hr - (getHH() + 1)) + (60 - getMM());
	}
	else if(getHH() > Wk_UP_Hr)
	{
		return 60 - getMM() + ((24 - (getHH()- Wk_UP_Hr)) * 60);
	}
	else
	{
		return 0;
	}
	
}

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

bool Timing::isDay() {


	if ((getHH() >= Wk_UP_Hr) && (getHH() < Sleep_Hr))
	{

		Serial.printf("Hour of a day = %d\n", getHH());
		return true;
	}
	else {

		Serial.printf("Hour of night = %d\n", getHH());
		return false;
	}
}

uint16_t Timing::getNextMeasuringMMLeft() {
	//Serial.printf("\n....... getNextMeasuringMMLeft .......... \n");
	double  intpart;

	modf((double)getMM() / INTERVAL, &intpart);

	
	uint8_t mm_left = (uint8_t)((intpart + 1) * INTERVAL - getMM());
	//Serial.printf("....... mm_left = %f\n", (intpart + 1) * INTERVAL - getMM());
	
	return mm_left;

}