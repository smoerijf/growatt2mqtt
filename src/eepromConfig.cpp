#include <Arduino.h>
#include <eeprom.h>
#include <globals.h>


const uint8_t DefEEpromInit[EE_INIT_STATE_SIZE] = EE_INIT_PATTERN;


configData_t config;


void saveConfig()
{
    EEPROM.begin(sizeof(config));
    EEPROM.put(EE_START_ADDR, config);
    EEPROM.commit(); // Only needed for ESP8266 to get data written
    EEPROM.end();
}

void loadEepromData()
{
    EEPROM.begin(sizeof(config));
    EEPROM.get(EE_START_ADDR, config);
    EEPROM.end();

    if (memcmp(config.EEpromInit, DefEEpromInit, EE_INIT_STATE_SIZE) != 0) // Init Code found?
    {
        memcpy(config.EEpromInit, DefEEpromInit, EE_INIT_STATE_SIZE); // initialize eeprom with default values 
        config.modbus_update_sec = UPDATE_MODBUS;
        config.status_update_sec = UPDATE_STATUS;
        config.wificheck_sec = WIFICHECK;
        saveConfig();
        ESP.eraseConfig();
#ifdef DEBUG_SERIAL
        delay(3000);
        Serial.println(F("Reset eesprom values to default and clean Wifi settings"));
#endif
    }
}