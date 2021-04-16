#pragma once
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "../lib/json/ArduinoJson.h"
#include "../lib/mqtt/PubSubClient.h"
#include "utils.h"
#include "render.h"
#include "event.h"
#include "config.h"
DynamicJsonDocument mqttDoc(1024);
WiFiClient mc;
PubSubClient mqttClient(mc);
String baseTopic;
bool getMqttInfo(String token){
    

    WiFiClient client;

    HTTPClient http;
    //if (http.begin(client, String("http://192.168.1.10:8000/dashboard/device/?token=") + token)) {  // HTTP
    if (http.begin(client, String("http://ngoinhaiot.com:8000/dashboard/device/?token=") + token)) {  // HTTP

        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = http.getString();
            DeserializationError err = deserializeJson(mqttDoc, payload);
            if (err)
                return false;
            else
                return true;
        } else {
          return false;
        }

      http.end();
    } else {
      return false;
    }
}
void mqttCallback(char* topic, byte* payload, unsigned int length){
    String data;
    for (unsigned int i = 0; i < length; i++) {
        data+=(char)payload[i];
    }
    DynamicJsonDocument cbDoc(512);
    DeserializationError err = deserializeJson(cbDoc, data);
    if (!err){
        JsonObject obj = cbDoc.as<JsonObject>();
        String cmd = obj["cmd"];
        String key = obj["key"];
        String val = obj["val"];
        if(cmd == "pig"){
            String res = "{\"cmd\":\"pog\",\"val\":\""+val+"\"}";
            mqttClient.publish((baseTopic+"/Tx/").c_str(), res.c_str());
        }
        if(cmd == "get"){
            String res = "{\"key\":\"" + key +"\",\"val\":\""+getValue(key)+"\"}";

            mqttClient.publish((baseTopic+"/Tx/").c_str(), res.c_str());
        }
        if(cmd == "gal"){
            String tmp = getValuesByJson();
            mqttClient.beginPublish((baseTopic+"/Tx/").c_str(),
            tmp.length(), true);
            for(unsigned int i=0; i<tmp.length(); i++){
                mqttClient.print(tmp[i]);
            }
            mqttClient.endPublish();
        }
        if(cmd == "exe"){
            execEvent(key, val);
            String tmp = getValuesByJson();
            mqttClient.beginPublish((baseTopic+"/Tx/").c_str(),
            tmp.length(), true);
            for(unsigned int i=0; i<tmp.length(); i++){
                mqttClient.print(tmp[i]);
            }
            mqttClient.endPublish();

        }
    }
}
bool connToMqttBroker(String token, uint8_t countTry = 3){
    mqttClient.disconnect();
    while(getMqttInfo(token) == false){
        delay(1000);
        countTry-- ;
        if(countTry == 0){
            return false;
        };
    };
    JsonObject obj = mqttDoc.as<JsonObject>();
    serializeJsonPretty(obj,Serial);
    const char* username = obj["mqtt"]["username"];
    const char* password = obj["mqtt"]["password"];
    const char* server = obj["mqtt"]["server"];
    const int port = obj["mqtt"]["tcpPort"];
    mqttClient.setServer(server, port);
    mqttClient.setCallback (mqttCallback);
    baseTopic = splitString(token,".",2);
    bool isSuccess =  mqttClient.connect(baseTopic.c_str(), username, password);
    
    if(isSuccess){
        String tmp = getRender();

        mqttClient.beginPublish((baseTopic+"/render/").c_str(),
        tmp.length(), true);
        for(unsigned int i=0; i<tmp.length(); i++){
            mqttClient.print(tmp[i]);
        }
        mqttClient.endPublish();
        mqttClient.subscribe((baseTopic+"/Rx/").c_str());
        return true;
    }
    return false;
}
void setupMqtt(){
    if(checkKey("token")){
        connToMqttBroker(getValue("token"));
    }
    setOnConfigChange([](String key, String val){
        String res = "{\"" + key +"\":\""+getValue(key)+"\"}";
        mqttClient.publish((baseTopic+"/Tx/").c_str(), res.c_str());
    });
};
void loopMqtt(){
    if(!mqttClient.connected() && checkKey("token"))
        connToMqttBroker(getValue("token"),1);
    mqttClient.loop();
}