#pragma once

extern "C" {
#include "user_interface.h" // this is for the RTC memory read/write functions
}

#include "config.h"

#define FLAG_ADR	65
#define START_ADR	66 

// Structure saived in eeprom
struct ee_data_str {
	float	am_wght;
	int		sleep_sec;		// sllep for sleep_mins, 
	time_t	last_measure_time;	// time of the last measurmet
	uint8_t	not_wifi_cnnct_times; // should device connect to wifi after awaking: 0->connect, "n"->do not connenct, every connect n--;
								  // used for night mode
};


class RTCmemory
{
public:
	ee_data_str * ee_data;
	RTCmemory();
	~RTCmemory();
	void readRTCmemData();
	void WriteRTCmemData();
	void EraseRTSmemStr();
	bool isfirstRTCmemwrite();
	void setRTCmemFlag(uint32_t flag);
};

