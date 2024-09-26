#ifndef DEVICE_h
#define DEVICE_h

#include "Arduino.h"


typedef enum
{
    Int,       // Integer
    Float,     // float
    FloatDeci, // float, map with 0,1 precision
    FloatDeci5,// float, map with 0,5 precision
    FloatMilli // float, map with 0,01 precision
} DataPointType_t;


typedef enum
{
    OneWord,
    TwoWord,
} RegisterSize_t;


typedef enum
{
    Input,
    Holding
} RegisterType_t;


typedef struct
{
    uint16_t modbus_address;
    RegisterType_t modbus_address_type;
    RegisterSize_t size;
    DataPointType_t datatype;
    uint16_t modbus_update_skips; // 0 = every modbus poll cycle
    const char* mqtt_topic;
    const char* value_unit;
} MqttDataPoint_t;


class Device
{    
private:
   uint16_t _inputData[128];

public:
    Device(const MqttDataPoint_t* datapoints, uint8_t numberOfDatapoints);

    void SetInputRegister(uint16_t reg, uint16_t value);
    uint16_t GetInputRegister(uint16_t reg);

    void Set(RegisterType_t type, uint16_t reg, uint16_t value);
    uint16_t Get(RegisterType_t type, uint16_t reg);

    void Set(MqttDataPoint_t& datapoint, uint16_t value);
    uint16_t Get(MqttDataPoint_t& datapoint);
    uint32_t Get32(MqttDataPoint_t& datapoint);

    const MqttDataPoint_t* datapoints;
    uint8_t numberOfDatapoints;
};

#endif