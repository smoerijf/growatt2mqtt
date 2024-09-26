#include <Arduino.h>

#if defined(ESP8266)
//#include <ESP8266WiFi.h>      // Wifi connection
//#include <ESPAsyncTCP.h>
//#include <ESPAsyncWebServer.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#endif

#include <ESPConnect.h>
#include <WebSerial.h>
//#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "globals.h"
#include "settings.h"
#include "mqtt.h"
#include "eepromConfig.h"


os_timer_t myTimer;
unsigned long uptime, seconds;
bool secondsFlag;
AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient mqtt(mqtt_server, mqtt_server_port, espClient);


// This is the 1 second timer callback function
void timerCallback(void *pArg)
{
  seconds++;
  uptime++;
  secondsFlag = true;
}

void setup()
{
    Serial.begin(SERIAL_RATE);
    Serial.println(F("\nGrowatt Solar Inverter to MQTT Gateway"));
    pinMode(STATUS_LED, OUTPUT);

    uptime = 0;
    loadEepromData();
#ifdef DEBUG_SERIAL
    Serial.println(F("Load Update values"));
    Serial.printf("Values via Modbus: %d sec\n", config.modbus_update_sec);
    Serial.printf("Status Update: %d sec\n", config.status_update_sec);
    Serial.printf("Wifi Check: %d sec\n", config.wificheck_sec);
#endif

    // Connect to Wifi
#ifdef FIXEDIP
    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    {
        Serial.println("STA Failed to configure");
    }
#endif
    
    // AutoConnect AP - Configure SSID and password for Captive Portal
    ESPConnect.autoConnect("Growatt2MQTT");
  
    // Begin connecting to previous WiFi or start autoConnect AP if unable to connect
    if (ESPConnect.begin(&server))
    {
        Serial.println("");
        Serial.println("Connected to WiFi");
        Serial.println("IPAddress: " + WiFi.localIP().toString());
        Serial.print("Signal [RSSI]: ");
        Serial.println(WiFi.RSSI());
    }
    else
    {
        Serial.println("Failed to connect to WiFi");
        ESP.restart();
    }
    
    // Set up the fully client ID
    byte mac[6];
    WiFi.macAddress(mac);
    snprintf(fullClientID, CLIENT_ID_SIZE, "%s-%02x%02x%02x", clientID, mac[3], mac[4], mac[5]);
    snprintf(topicRoot, TOPPIC_ROOT_SIZE, "%s-%02x%02x%02x", clientID, mac[3], mac[4], mac[5]);
    Serial.printf("Client ID: %s\n", fullClientID);
  
    WebSerial.onMessage([&](uint8_t *data, size_t len) 
    {
        Serial.printf("Received %u bytes from WebSerial: ", len);
        Serial.write(data, len);
        Serial.println();

        WebSerial.println("Received Data...");
        String d = "";
        for(size_t i=0; i < len; i++){
            d += char(data[i]);
        }
        WebSerial.println(d);
    });
    WebSerial.begin(&server);
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { request->send(200, "text/html", "ok"); });

    server.begin();
    WebSerial.println(F("HTTP server started"));
    
    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(fullClientID);    
    // ArduinoOTA.setPassword((const char *)"123"); // No authentication by default
    ArduinoOTA.onStart([]()
    {
        os_timer_disarm(&myTimer);
        WebSerial.println(F("Start"));
    });
    ArduinoOTA.onEnd([]()
    {
        WebSerial.println(F("\nEnd"));
        os_timer_arm(&myTimer, 1000, true);
    });    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
    {
        WebSerial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error)
    {
        WebSerial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) WebSerial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) WebSerial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) WebSerial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) WebSerial.println("Receive Failed");
        else if (error == OTA_END_ERROR) WebSerial.println("End Failed");
    });
    ArduinoOTA.begin();
    
    os_timer_setfn(&myTimer, timerCallback, NULL);
    os_timer_arm(&myTimer, 1000, true);
}

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


MqttDataPoint_t mqttDataPoints[] = 
{
    { 0, OneWord, Int, 10, "status", "" },
    { 1, TwoWord, FloatDeci, 0, "solar_power", "W" },
    { 3, OneWord, FloatDeci, 10, "pv1_voltage", "V" },
    { 4, OneWord, FloatDeci, 10, "pv1_current", "A" },
    { 5, TwoWord, FloatDeci, 0, "pv1_power", "W" }
};
#define TMP_BUFFER_SIZE  50 // used for String Operations


void loop()
{
    char value[MAX_JSON_TOPIC_LENGTH];
    char topic[MAX_ROOT_TOPIC_LENGTH];

    ArduinoOTA.handle();

    // Handle MQTT connection/reconnection
    if (strlen(mqtt_server) > 0)
    {
        if (!mqtt.connected())
        {
            reconnect();
        }
        mqtt.loop();
    }

    if (secondsFlag)
    {
        secondsFlag = false;

        if (seconds % 2 == 0)
        {
            WebSerial.printf("running %li\n", seconds);
            digitalWrite(STATUS_LED, 1);
        }
        else
        {
            digitalWrite(STATUS_LED, 0);
        }

        if (seconds % config.modbus_update_sec == 0)
        {
            if (strlen(mqtt_server) > 0)
            {
                uint16_t valueBuffer[] = 
                {
                    1, // status
                    2, 3, // solar_power
                    4, // pv1 v
                    5, // pv1 a
                    6, 7 // pw w
                };

                size_t size = sizeof(mqttDataPoints) / sizeof(MqttDataPoint_t);
                WebSerial.printf("MQTT datapoints: %i\n", size);
                for (uint8_t i = 0; i < size; i++)
                {
                    char dataTopic[TMP_BUFFER_SIZE];
                    char dataValue[TMP_BUFFER_SIZE];

                    MqttDataPoint_t datapointDescr = mqttDataPoints[i];
                    if (datapointDescr.datatype == Int)
                    {
                        uint16_t value = valueBuffer[datapointDescr.modbus_address];
                        snprintf(dataValue, TMP_BUFFER_SIZE, "%d", value);
                    }
                    else
                    {
                        float value;
                        if (datapointDescr.size == OneWord)
                        {
                            value = valueBuffer[datapointDescr.modbus_address];
                        }
                        else
                        {
                            value = ((valueBuffer[datapointDescr.modbus_address] << 16) | valueBuffer[datapointDescr.modbus_address + 1]);
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

                    snprintf(dataTopic, TMP_BUFFER_SIZE, "%s/data/%s", topicRoot, datapointDescr.mqtt_topic);
                    mqtt.publish(dataTopic, dataValue);
                    WebSerial.printf("MQTT publish %s = %s", dataTopic, dataValue);
                }
            }
        }
        
        if (seconds % config.status_update_sec == 0)
        {
            snprintf(value, MAX_JSON_TOPIC_LENGTH, "{\"rssi\":%d,\"uptime\":%lu,\"ssid\":\"%s\",\"ip\":\"%d.%d.%d.%d\",\"clientid\":\"%s\",\"version\":\"%s\",\"modbusUpdate\":%d,\"statusUpdate\":%d,\"Wifi check\":%d}", WiFi.RSSI(), uptime, WiFi.SSID().c_str(), WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3], fullClientID, buildversion, config.modbus_update_sec, config.status_update_sec, config.wificheck_sec);
            snprintf(topic, MAX_ROOT_TOPIC_LENGTH, "%s/%s", topicRoot, "status");
            mqtt.publish(topic, value);

            WebSerial.printf("MQTT status sent: %s", value);
        }

        if (seconds % config.wificheck_sec == 0)
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                WebSerial.println(F("Reconnecting to wifi..."));
                WiFi.reconnect();
                uptime = 0;
            }
        }
    }
}
