#ifndef SETTINGS_h
#define SETTINGS_h

#define SERIAL_RATE     115200    // Serial speed for status info
#define MAX485_DE       16         // D0, DE pin on the TTL to RS485 converter
#define MAX485_RE_NEG   13        // D7, RE pin on the TTL to RS485 converter
#define MAX485_RX       14        // D5, RO pin on the TTL to RS485 converter
#define MAX485_TX       12        // D6, DI pin on the TTL to RS485 converter
#define STATUS_LED      2         // Status LED on the Wemos D1 mini (D4)
#define SCL_PIN         5         // D1/SCL to Pin SCL on AHT Sensor
#define SDA_PIN         4         // D2/SDA to Pin SDA on AHT Sensor


// Update the below parameters for your project
// Also check NTP.h for some parameters as well
const char mqtt_server[] = "192.168.1.42";    // MQTT server
const int mqtt_server_port = 1883;             // MQTT server port, default is 1883
const char mqtt_user[] = "sege";               // MQTT userid
const char mqtt_password[] = "Pumice-Curse-Progress";        // MQTT password
const char clientID[] = "growatt2mqtt";       // MQTT client ID
const char topicRootStart[] = "growatt2mqtt";      // MQTT root topic for the device, + client ID

// Comment the entire second below for dynamic IP (including the define)
#define FIXEDIP   1
const IPAddress local_IP(192, 168, 1, 43);     // Set your Static IP address
const IPAddress gateway(192, 168, 1, 1);          // Set your Gateway IP address
const IPAddress subnet(255, 255, 255, 0);
const IPAddress primaryDNS(192, 168, 1, 1);   //optional
const IPAddress secondaryDNS(8, 8, 4, 4); //optional

#endif
