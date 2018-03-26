#include "RTCmemory.h"



RTCmemory::RTCmemory()
{
	ee_data = new ee_data_str;
}


RTCmemory::~RTCmemory()
{
}


// ----------------------------------------------------------------------------------
//	WriteRTCmemData
//-----------------------------------------------------------------------------------
void RTCmemory::WriteRTCmemData()
{

	Serial.println("WriteRTCmemData");


	system_rtc_mem_write(START_ADR, &ee_data, sizeof(ee_data));
	Serial.println("WriteRTCmemData");

}
//----------------------------------------------------------------------------------
//// reset the EEPROM
//----------------------------------------------------------------------------------

void RTCmemory::EraseRTSmemStr()
{

	ee_data->am_wght = 0;
	ee_data->last_measure_time = 0;
	ee_data->not_wifi_cnnct_times = 0;
	ee_data->sleep_sec = 0;
	system_rtc_mem_write(START_ADR, &ee_data, sizeof(ee_data));


}
//----------------------------------------------------------------------------------
//// isfirstRTCmemwrite
//----------------------------------------------------------------------------------

bool RTCmemory::isfirstRTCmemwrite()
{

	uint32_t flag;

	system_rtc_mem_read(FLAG_ADR, &flag, sizeof(flag));
	Serial.printf("RAM flag is = 0x%08x", flag);

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

	Serial.println("setRTCmemFlag");
	system_rtc_mem_write(FLAG_ADR, &flag, sizeof(flag));
	Serial.printf("RAM flag is = 0x%08x", flag);

}