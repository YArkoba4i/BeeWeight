#include <EEPROM.h>


//----------------------------------------------------------------------------------
//// Read parameters from EEPROM or Serial
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
