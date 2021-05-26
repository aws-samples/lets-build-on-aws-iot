#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

#include <WiFiClientSecure.h>
#include <MQTTClient.h>

#include "secrets.h"
#include "ClosedCube_HDC1080.h"

#include <ArduinoJson.h>


#define hostName "letsbuild01"

const char* ssid = "surfing-iot";
const char* password = "iotiotiot";

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_SENSORS   "foodcontrol/sensors"
#define AWS_IOT_PUBLISH_BUTTON   "callwaiter/button"
#define AWS_IOT_SUBSCRIBE_TOPIC "control/lamp"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

#define SECRET
#define THINGNAME "letsbuild01"

ClosedCube_HDC1080 hdc1080;

#define TABLE_NUMBER 10

#define IO_BUTTON 23
#define IO_RELAY 27

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  //button setup
  pinMode(23, INPUT_PULLUP);   //enable user button pull-up
  //relay
  pinMode(27, OUTPUT);

  // Default settings: 
  //  - Heater off
  //  - 14 bit Temperature and Humidity Measurement Resolutions
  hdc1080.begin(0x40);
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
  attachInterrupt(23, buttonISR, FALLING);
  connectWIFI();
  connectAWS();
}

double temperature, humidity;
unsigned long lastSensorsSending=0;
int volatile lastButtonTimeStamp=0, sendMessage=0;

void loop() {
  client.loop();
  temperature = hdc1080.readTemperature();
  humidity = hdc1080.readHumidity();  
  /*Serial.print("T=");
  Serial.print(hdc1080.readTemperature());
  Serial.print("C, RH=");
  Serial.print(hdc1080.readHumidity());
  Serial.println("%");
  Serial.print("Button=");
  Serial.println(digitalRead(23) ? "on" : "off");*/
  if(millis() - lastSensorsSending>1000) {
    sensors();
    lastSensorsSending = millis();
  }
  if(sendMessage) {
    sendMQTTButtonMessage();
  }

}

void buttonISR() {
  if(millis() - lastButtonTimeStamp>400) {
    Serial.println("publishing...");
    sendMessage=1;
    lastButtonTimeStamp=millis();
  }
}

void sendMQTTButtonMessage() {
    StaticJsonDocument<200> doc;    
    doc["device_time"] = millis();
    doc["device_name"] = THINGNAME;
    doc["table_number"] = TABLE_NUMBER;
    char jsonBuffer[128];
    serializeJson(doc, jsonBuffer); // print to client
    client.publish(AWS_IOT_PUBLISH_BUTTON, jsonBuffer);
    sendMessage=0;
}

void sensors() {
  StaticJsonDocument<200> doc;
  doc["device_time"] = millis();
  doc["device_name"] = THINGNAME;
  doc["humidity"] = humidity;
  doc["temperature"] = temperature;
  char jsonBuffer[128];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_SENSORS, jsonBuffer);
}

void connectWIFI() {
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
