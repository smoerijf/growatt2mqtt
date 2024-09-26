#include <Arduino.h>
#include <mqtt.h>


char fullClientID[CLIENT_ID_SIZE];
char topicRoot[TOPPIC_ROOT_SIZE]; // MQTT root topic for the device, + client ID


// MQTT reconnect logic
void reconnect()
{
  // String mytopic;
  //  Loop until we're reconnected
  char topic[MAX_ROOT_TOPIC_LENGTH];

  while (!mqtt.connected())
  {
    Serial.print("Attempting MQTT connection...");
    Serial.print(F("Client ID: "));
    Serial.println(fullClientID);

    // Attempt to connect
    snprintf(topic, MAX_ROOT_TOPIC_LENGTH, "%s/%s", topicRoot, "connection");
    if (mqtt.connect(fullClientID, mqtt_user, mqtt_password, topic, 1, true, "offline"))
    { // last will
      Serial.println(F("connected"));
      // ... and resubscribe
      mqtt.publish(topic, "online", true);
      snprintf(topic, MAX_ROOT_TOPIC_LENGTH, "%s/write/#", topicRoot);
      mqtt.subscribe(topic);
      snprintf(topic, MAX_ROOT_TOPIC_LENGTH, "%s/writeconfig/#", topicRoot);
      mqtt.subscribe(topic);
    }
    else
    {
      Serial.print(F("failed, rc="));
      Serial.print(mqtt.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}