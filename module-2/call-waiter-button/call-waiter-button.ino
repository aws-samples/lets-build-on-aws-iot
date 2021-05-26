#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

#include "secrets.h"

#define AWS_THING_NAME "esp32"
#define AWS_IOT_PUBLISH_BUTTON "callwaiter/button"

#define TABLE_NUMBER 13  // table number
#define BUTTON_PIN 23    // push-buton pin

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(512);
volatile unsigned long lastCallWaiterTime = 0; // Records last call waiter time
volatile bool callWaiter = false;              // Flag to call waiter

void setup() {
  Serial.begin(115200);

  // Initialize the push-button pin as an input
  // with an internal pull-up resistor enabled:
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // When push-button pin goes from high to low (FALLING),
  // call the buttonPressed() function.
  attachInterrupt(BUTTON_PIN, buttonPressed, FALLING);

  connectWiFi();
  connectAWS();
}

void buttonPressed() {
  unsigned long currentTime = millis();

  // We want to avoid calling the waiter multiple times at once
  if (currentTime - lastCallWaiterTime > 1000) {
    callWaiter = true;
    lastCallWaiterTime = currentTime;
  }
}

void loop()  {
  // Sends and receives packets
  client.loop();

  if (callWaiter) {
    Serial.println("Calling waiter...");

    // Create JSON document
    StaticJsonDocument<200> json;
    json["tableNumber"] = TABLE_NUMBER;
    json["deviceTime"] = millis();
    json["deviceName"] = AWS_THING_NAME;

    // Serialize JSON
    char jsonBuffer[128];
    serializeJson(json, jsonBuffer);

    // Publish message to callwaiter/button topic
    client.publish(AWS_IOT_PUBLISH_BUTTON, jsonBuffer);

    // Reset the callWaiter flag
    callWaiter = false;
  }
}
