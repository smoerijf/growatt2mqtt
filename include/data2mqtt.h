#include <PubSubClient.h>


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


typedef struct
{
    uint16_t modbus_address;
    RegisterSize_t size;
    DataPointType_t datatype;
    uint16_t modbus_update_skips; // 0 = every modbus poll cycle
    const char* mqtt_topic;
    const char* value_unit;
} MqttDataPoint_t;


class Data2mqtt
{
private:
    PubSubClient& _mqtt;
    char* _topicPrefix;
    const MqttDataPoint_t* _datapoints;
    uint8_t _numberOfDatapoints;

public:
    Data2mqtt(PubSubClient& mqtt, const MqttDataPoint_t* datapoints, uint8_t numberOfDatapoints, char* topicPrefix);

    void publish(uint16_t* data);
};
