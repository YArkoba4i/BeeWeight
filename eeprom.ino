#include <EEPROM.h>
extern "C" {
#include "user_interface.h" // this is for the RTC memory read/write functions
}

//----------------------------------------------------------------------------------
//// Read parameters from EEPROM 
//----------------------------------------------------------------------------------


void readEEpromData()
{

	EEPROM.begin(EEPROM_SIZE);

	EEPROM.get(0, ee_data);

	Serial.printf("\n.....eeprom read ....\n");
	Serial.printf("ee_data_str.am_wght = %f\n", ee_data.am_wght);
	Serial.printf("ee_data_str.not_wifi_cnnct_times = %lu\n", ee_data.not_wifi_cnnct_times);
	Serial.printf("ee_data_str.sleep_sec = %d\n", ee_data.sleep_sec);
	
	EEPROM.end();
	delay(500);
}

//----------------------------------------------------------------------------------
//// reset the EEPROM
//----------------------------------------------------------------------------------

void EraseEEPROM()
{

    char data[EEPROM_SIZE];
    memset(data, '\0', EEPROM_SIZE);
	EEPROM.begin(EEPROM_SIZE);
	
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#define EEPROM_END 0
#define EEPROM_START 1
void EEPROMWrite(int addr, char *data, int size)
{


	EEPROM.begin(EEPROM_SIZE);
	// write the start marker
	EEPROM.write(addr, EEPROM_START);
	addr++;
	for (int i = 0; i < size; i++)
	{
		EEPROM.write(addr, data[i]);
		addr++;
	}
	EEPROM.write(addr, EEPROM_END);
	EEPROM.commit();
	EEPROM.end();
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void WriteEEPROMData()
{

    EEPROM.begin(EEPROM_SIZE);
	Serial.printf("\n......Writing data to eeprom ....\n");
	 // write the start marker
	Serial.printf("ee_data_str.am_wght = %f\n", ee_data.am_wght);
	Serial.printf("ee_data_str.not_wifi_cnnct_times = %lu\n", ee_data.not_wifi_cnnct_times);
	Serial.printf("ee_data_str.sleep_sec = %d\n", ee_data.sleep_sec);
	EEPROM.put(0, ee_data);
    
    EEPROM.commit();
    EEPROM.end();
	
}

//----------------------------------------------------------------------------------
//// Read parameters from RTC memory 
//----------------------------------------------------------------------------------


void readRTCmemData()
{

	

	Serial.println("ReadRTCmemData");
	system_rtc_mem_read(START_ADR, &ee_data, sizeof(ee_data));

	Serial.printf("ee_data_str.am_wght = %f\n", ee_data.am_wght);
	Serial.printf("ee_data_str.not_wifi_cnnct_times = %lu\n", ee_data.not_wifi_cnnct_times);
	Serial.printf("ee_data_str.sleep_sec = %d\n", ee_data.sleep_sec);



	delay(500);
}

// ----------------------------------------------------------------------------------
//	WriteRTCmemData
//-----------------------------------------------------------------------------------
void WriteRTCmemData()
{


	Serial.println("WriteRTCmemData");


	system_rtc_mem_write(START_ADR, &ee_data, sizeof(ee_data));
	Serial.println("WriteRTCmemData");

}
//----------------------------------------------------------------------------------
//// reset the EEPROM
//----------------------------------------------------------------------------------

void EraseRTSmemStr()
{

	ee_data.am_wght = 0;
	ee_data.last_measure_time = 0;
	ee_data.not_wifi_cnnct_times = 0;
	ee_data.sleep_sec = 0;
	system_rtc_mem_write(START_ADR, &ee_data, sizeof(ee_data));


}
//----------------------------------------------------------------------------------
//// isfirstRTCmemwrite
//----------------------------------------------------------------------------------

bool isfirstRTCmemwrite()
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

void setRTCmemFlag(uint32_t flag)
{

	Serial.println("setRTCmemFlag");
	system_rtc_mem_write(FLAG_ADR, &flag, sizeof(flag));
	Serial.printf("RAM flag is = 0x%08x", flag);

}