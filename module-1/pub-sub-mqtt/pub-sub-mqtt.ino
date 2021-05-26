#include <WiFiClientSecure.h>
#include <MQTTClient.h>

#include "secrets.h"

#define AWS_THING_NAME "esp32"
#define AWS_IOT_PUBLISH_TOPIC   "foodstuff/publish"
#define AWS_IOT_SUBSCRIBE_TOPIC "foodstuff/subscribe"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(512);

// the setup function runs once when you power the board or press reset
void setup() {
  Serial.begin(115200);
  connectWiFi();
  connectAWS();

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  // Create a message handler
  client.onMessage(messageHandler);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("Receiving MQTT message:");
  Serial.println(topic + " " + payload);
}

// the loop function runs over and over again forever
void loop() {
  // Sends and receives packets
  client.loop();

  // Publish to a topic
  client.publish(AWS_IOT_PUBLISH_TOPIC, "{\"message\": \"Hello from ESP32!\"}");
}
