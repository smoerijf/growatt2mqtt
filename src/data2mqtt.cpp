#include <data2mqtt.h>
#include <WebSerial.h>


#define TMP_BUFFER_SIZE  50 // used for String Operations


Data2mqtt::Data2mqtt(PubSubClient& mqtt, const MqttDataPoint_t* datapoints, uint8_t numberOfDatapoints, char* topicPrefix) : _mqtt(mqtt)
{
    _datapoints = datapoints;
    _numberOfDatapoints = numberOfDatapoints;
    _topicPrefix = topicPrefix;
}

void Data2mqtt::publish(uint16_t* data)
{
    for (uint8_t i = 0; i < _numberOfDatapoints; i++)
    {
        char dataTopic[TMP_BUFFER_SIZE];
        char dataValue[TMP_BUFFER_SIZE];

        MqttDataPoint_t datapointDescr = _datapoints[i];
        if (datapointDescr.datatype == Int)
        {
            uint16_t value = (data[datapointDescr.modbus_address]);
            snprintf(dataValue, TMP_BUFFER_SIZE, "%d", value);
        }
        else
        {
            float value;
            if (datapointDescr.size == OneWord)
            {
                value = data[datapointDescr.modbus_address];
            }
            else
            {
                value = ((data[datapointDescr.modbus_address] << 16) | data[datapointDescr.modbus_address + 1]);
            }

            if (datapointDescr.datatype == FloatDeci)
            {
                value *= 0.1;
            }
            else if (datapointDescr.datatype == FloatMilli)
            {
                value *= 0.01;
            }
            else if (datapointDescr.datatype == FloatDeci5)
            {
                value *= 0.5;
            }

            snprintf(dataValue, TMP_BUFFER_SIZE, "%f", value);
        }

        snprintf(dataTopic, TMP_BUFFER_SIZE, "%s/%s", _topicPrefix, datapointDescr.mqtt_topic);
        _mqtt.publish(dataTopic, dataValue);
    }

    WebSerial.printf("MQTT publish done");
}