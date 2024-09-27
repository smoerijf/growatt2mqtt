#ifndef GLOBALS_h
#define GLOBALS_h


const char buildversion[] = "v0.1.3";


#define DEBUG_SERIAL    
#define DEBUG_MQTT       
#define useModulPower   
//#define AHTXX_SENSOR               // add support for the AHT10, AHT15, AHT20 sensor family 

#define ENABLE_WEBAPI_POST               // enables Inverter Control via Web API


#define UPDATE_MODBUS   2         // 1: modbus device is read every second and data are anounced via mqtt
#define UPDATE_STATUS   30        // 10: status mqtt message is sent every 10 seconds
#define WIFICHECK       1         // 1: every second


#define EE_START_ADDR 0x00 // start address of values stored in the eeprom
#define EE_INIT_STATE_SIZE 4
#define EE_INIT_PATTERN { 0xa8, 0xa4, 0xfa, 0xbf } //change min 1 number to reset the whole EEProm settinges to default and clean Wifi settings


typedef struct
{
    uint8_t EEpromInit[EE_INIT_STATE_SIZE]; // paaternif EEprom is used
    uint16_t modbus_update_sec;         // 1: modbus device is read every second and data are anounced via mqtt
    uint16_t status_update_sec;         // 10: status mqtt message is sent every 10 seconds
    uint16_t wificheck_sec;             // 1: every second
} configData_t;


extern configData_t config;

#endif