#include "RTCmemory.h"

RTCmemory::RTCmemory()
{
	
}


RTCmemory::~RTCmemory()
{
}


//----------------------------------------------------------------------------------
//// isfirstRTCmemwrite
//----------------------------------------------------------------------------------

bool RTCmemory::isfirstRTCmemwrite()
{

	uint32_t flag;

	system_rtc_mem_read(FLAG_ADR, &flag, sizeof(flag));
	//Serial.printf("RAM flag is = 0x%08x", flag);

	if (flag != 0xffffffff) 
	{ 
		return true; 
	}
	else 
		return false;
}

//----------------------------------------------------------------------------------
//// setRTCmemFlag
//----------------------------------------------------------------------------------

void RTCmemory::setRTCmemFlag(uint32_t flag)
{

//	Serial.println("setRTCmemFlag");
	system_rtc_mem_write(FLAG_ADR, &flag, sizeof(flag));
//	Serial.printf("RAM flag is = 0x%08x", flag);

}

//----------------------------------------------------------------------------------
//// Read parameters from RTC memory 
//----------------------------------------------------------------------------------


void RTCmemory::readRTCmemData(struct ee_data_str * str)
{

	ee_data_str struc;

//	Serial.println("ReadRTCmemData");
	system_rtc_mem_read(START_ADR, &struc, sizeof(struc));



	system_rtc_mem_read(START_ADR, &str, sizeof(str));


//	delay(500);
}
//----------------------------------------------------------------------------------
//// reset the EEPROM
//----------------------------------------------------------------------------------

void RTCmemory::EraseEEPROM()
{

/*	ee_data.am_wght = 0.00;
	ee_data.last_measure_time = 0;
	ee_data.not_wifi_cnnct_times = 0;
	ee_data.sleep_sec = 0;

	system_rtc_mem_write(START_ADR, &ee_data, sizeof(ee_data));
*/
}
// ----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RTCmemory::WriteRTCmemData(struct ee_data_str * str)
{
	


//	Serial.println("WriteRTCmemData");
	

	system_rtc_mem_write(START_ADR, &str, sizeof(str));

	
}