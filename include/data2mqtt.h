#ifndef DATA2MQTT_h
#define DATA2MQTT_h

#include <PubSubClient.h>
#include "device.h"


class Data2mqtt
{
private:
    PubSubClient& _mqtt;
    char* _topicPrefix;
    Device& _device;

public:
    Data2mqtt(PubSubClient& mqtt, Device& device, char* topicPrefix);

    void publish();
};

#endif