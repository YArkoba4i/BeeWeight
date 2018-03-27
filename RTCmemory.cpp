#include "RTCmemory.h"



RTCmemory::RTCmemory()
{
	ee_data = new ee_data_str;
}


RTCmemory::~RTCmemory()
{
	delete ee_data;
}

//----------------------------------------------------------------------------------
//// Read parameters from RTC memory 
//----------------------------------------------------------------------------------


void RTCmemory::readRTCmemData()
{

	Serial.println("ReadRTCmemData");
	system_rtc_mem_read(START_ADR, ee_data, sizeof(ee_data_str));

	Serial.printf("ee_data_str.am_wght = %f\n", ee_data->am_wght);
	Serial.printf("ee_data_str.not_wifi_cnnct_times = %lu\n", ee_data->not_wifi_cnnct_times);
	Serial.printf("ee_data_str.sleep_sec = %d\n", ee_data->sleep_sec);



	delay(500);
}

// ----------------------------------------------------------------------------------
//	WriteRTCmemData
//-----------------------------------------------------------------------------------
void RTCmemory::WriteRTCmemData()
{

	Serial.println("WriteRTCmemData");


	system_rtc_mem_write(START_ADR, ee_data, sizeof(ee_data_str));
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
	system_rtc_mem_write(START_ADR, ee_data, sizeof(ee_data_str));


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