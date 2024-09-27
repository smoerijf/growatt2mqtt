#include <data2mqtt.h>
#include <WebSerial.h>
#include "device.h"


#define TMP_BUFFER_SIZE  64 // used for String Operations


Data2mqtt::Data2mqtt(PubSubClient& mqtt, Device& device, char* topicPrefix) : _mqtt(mqtt), _device(device)
{
    _topicPrefix = topicPrefix;
}

void Data2mqtt::publish()
{
    for (uint8_t i = 0; i < _device.numberOfDatapoints; i++)
    {
        char dataTopic[TMP_BUFFER_SIZE];
        char dataValue[TMP_BUFFER_SIZE];

        MqttDataPoint datapointDescr = _device.datapoints[i];
        if (datapointDescr.datatype == Int)
        {
            uint16_t value = _device.Get(datapointDescr);
            snprintf(dataValue, TMP_BUFFER_SIZE, "%d", value);
        }
        else
        {
            float value;
            if (datapointDescr.size == OneWord)
            {
                value = _device.Get(datapointDescr);
            }
            else
            {
                value = _device.Get32(datapointDescr);
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