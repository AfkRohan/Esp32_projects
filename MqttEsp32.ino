#include <WiFi.h>
#include <WifiClient.h>
#include <Arduino.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
char *mqttServer = "broker.hivemq.com";  //Credential
int mqttPort = 1883;   //Credential 
char *ssid = "ADVANCETECH_DIR_4G";
char *pwd = "12761487";

void connectToWifi(){
  Serial.print("Conneting to ");

  WiFi.begin(ssid,pwd);
  Serial.println(ssid);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected.");
}

void setupMQTT(){
  mqttClient.setServer(mqttServer,mqttPort);
  mqttClient.setCallback(callback);  
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "clientId-nwNLr2ABXl";
      clientId += String(random(0xffff), HEX);
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/swa/commands");
      }
  }
}

void callback(char* topiv,byte* payload,unsigned int length){
  Serial.print("Callback - ");
  Serial.print("Message:");
  for(int i = 0; i< length;i++){
    Serial.print((char)payload[i]);
  }
}

void setup(){
  Serial.begin(9600);
  connectToWifi();
  if (!bmp.begin(0x76)) {
    Serial.println("Problem connecting to BME280");
  }
  setupMQTT();
}

void loop() {
    if (!mqttClient.connected())
    reconnect();
    mqttClient.loop();
    long last_time=0;
    char data[25];
    long now = millis();
    if (now - last_time > 60000) {
//     Send data
     float temp = bmp.readTemperature();
     Serial.print(bmp.readTemperature());
//     Publishing data throgh MQTT
    sprintf(data, "%f", temp);
    Serial.println(data);
    mqttClient.publish("/swa/temperature", data);
    last_time = now;   
   }
}
