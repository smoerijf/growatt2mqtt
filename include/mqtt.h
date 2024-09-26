#include <PubSubClient.h> 
#include <settings.h>


#define MAX_JSON_TOPIC_LENGTH 1024
#define MAX_ROOT_TOPIC_LENGTH 80
#define MAX_EXPECTED_TOPIC_LENGTH 50

#define CLIENT_ID_SIZE   (sizeof(clientID) + 7) // 3*2 char = 6 + '-'
#define TOPPIC_ROOT_SIZE (sizeof(topicRootStart) + 7) // 3*2 char = 6 + '-'


extern PubSubClient mqtt;
extern char fullClientID[CLIENT_ID_SIZE];
extern char topicRoot[TOPPIC_ROOT_SIZE];


void reconnect();