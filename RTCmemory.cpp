#include "RTCmemory.h"



RTCmemory::RTCmemory()
{
	this->ee_data = new ee_data_str;
}


RTCmemory::~RTCmemory()
{
	delete this->ee_data;
}

//----------------------------------------------------------------------------------
//// Read parameters from RTC memory 
//----------------------------------------------------------------------------------


void RTCmemory::readRTCmemData()
{

	//Serial.println("ReadRTCmemData");
	system_rtc_mem_read(START_ADR, this->ee_data, sizeof(ee_data_str));

	/*Serial.printf("ee_data_str.am_wght = %f\n", this->ee_data->am_wght);
	Serial.printf("ee_data_str.not_wifi_cnnct_times = %lu\n", this->ee_data->not_wifi_cnnct_times);
	Serial.printf("ee_data_str.sleep_sec = %d\n", this->ee_data->sleep_sec);
	*/


	delay(500);
}

// ----------------------------------------------------------------------------------
//	WriteRTCmemData
//-----------------------------------------------------------------------------------
void RTCmemory::WriteRTCmemData()
{

	//Serial.println("WriteRTCmemData");


	system_rtc_mem_write(START_ADR, this->ee_data, sizeof(ee_data_str));
	//Serial.println("WriteRTCmemData");

}
//----------------------------------------------------------------------------------
//// reset the EEPROM
//----------------------------------------------------------------------------------

void RTCmemory::EraseRTSmemStr()
{

	this->ee_data->am_wght = 0;
	this->ee_data->last_measure_time = 0;
	this->ee_data->not_wifi_cnnct_times = 0;
	this->ee_data->sleep_sec = 0;
	system_rtc_mem_write(START_ADR, this->ee_data, sizeof(ee_data_str));


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

	//Serial.println("setRTCmemFlag");
	system_rtc_mem_write(FLAG_ADR, &flag, sizeof(flag));
	//Serial.printf("RAM flag is = 0x%08x", flag);

}