#pragma once
extern "C" {
#include "user_interface.h" // this is for the RTC memory read/write functions
}
//#include "config.h"
// Structure saived in eeprom
struct ee_data_str {
	float	am_wght;
	int		sleep_sec;		// sllep for sleep_mins, 
	time_t	last_measure_time;	// time of the last measurmet
	uint8_t	not_wifi_cnnct_times; // should device connect to wifi after awaking: 0->connect, "n"->do not connenct, every connect n--;
								  // used for night mode
};

#define FLAG_ADR	64
#define START_ADR	65 




class RTCmemory
{
public:
	RTCmemory();
	~RTCmemory();
	bool isfirstRTCmemwrite();
	void setRTCmemFlag(uint32_t flag);
	void readRTCmemData(struct ee_data_str * str);
	void EraseEEPROM();
	void WriteRTCmemData(struct ee_data_str * str);
};

