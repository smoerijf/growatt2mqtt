#ifndef DEVICE_h
#define DEVICE_h

#include "Arduino.h"


enum DataPointType
{
    Int,       // Integer
    Float,     // float
    FloatDeci, // float, map with 0,1 precision
    FloatDeci5,// float, map with 0,5 precision
    FloatMilli // float, map with 0,01 precision
};


enum RegisterSize
{
    OneWord,
    TwoWord,
};


enum RegisterType
{
    Input,
    Holding
};


struct MqttDataPoint
{
    uint16_t modbus_address;
    RegisterType modbus_address_type;
    RegisterSize size;
    DataPointType datatype;
    uint16_t modbus_update_skips; // 0 = every modbus poll cycle
    const char* mqtt_topic;
    const char* value_unit;
};


class Device
{    
private:
   uint16_t _inputData[128];

public:
    Device(const MqttDataPoint* datapoints, uint8_t numberOfDatapoints);

    void SetInputRegister(uint16_t reg, uint16_t value);
    uint16_t GetInputRegister(uint16_t reg);

    void Set(RegisterType type, uint16_t reg, uint16_t value);
    uint16_t Get(RegisterType type, uint16_t reg);

    void Set(MqttDataPoint& datapoint, uint16_t value);
    uint16_t Get(MqttDataPoint& datapoint);
    uint32_t Get32(MqttDataPoint& datapoint);

    const MqttDataPoint* datapoints;
    uint8_t numberOfDatapoints;
};

#endif