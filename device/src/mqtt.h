#pragma once
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include "./json/ArduinoJson.h"
#include "./mqtt/PubSubClient.h"
// #include <PubSubClient.h>
#include "utils.h"
#include "render.h"
#include "event.h"
#include "config.h"
DynamicJsonDocument mqttDoc(1024);
WiFiClient mc;
PubSubClient mqttClient(mc);
String baseTopic;
void largePublish(String topic, String data, bool retain){
    mqttClient.beginPublish(topic.c_str(),
    data.length(), retain);
    for(unsigned int i=0; i<data.length(); i++){
        mqttClient.print(data[i]);
    }
    mqttClient.endPublish();
}
bool getMqttInfo(String token){
    

    WiFiClient client;

    HTTPClient http;
    client.setTimeout(300);
    //  if (http.begin(client, String("http://192.168.1.8:8000/dashboard/device/?token=") + token)) {  // HTTP
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
        if(cmd == "get"){
            String res = "{\"key\":\"" + key +"\",\"val\":\""+getValue(key)+"\"}";
            largePublish((baseTopic+"/Tx/").c_str(), res.c_str(), false);
        }
        if(cmd == "gal"){
            largePublish((baseTopic+"/Tx/").c_str(), getValuesByJson(), false);
        }
        if(cmd == "exe"){
            execEvent(key, val);
        }
    }
}
bool connToMqttBroker(String token, uint8_t countTry = 3){
    mc.setTimeout(100);
    mqttClient.disconnect();

    while(countTry && getMqttInfo(token) == false){
        countTry-- ;
    };
    JsonObject obj = mqttDoc.as<JsonObject>();
    const char* username = obj["mqtt"]["username"];
    const char* password = obj["mqtt"]["password"];
    const char* server = obj["mqtt"]["server"];
    const int port = obj["mqtt"]["tcpPort"];
    mqttClient.setServer(server, port);
    // mqttClient.setServer("192.168.1.8", port);
    mqttClient.setCallback (mqttCallback);
    baseTopic = splitString(token,".",2);
    mqttClient.setKeepAlive(5);
    bool isSuccess =  mqttClient.connect(baseTopic.c_str(), username, password,
      (baseTopic + "/state/").c_str(),
      1,
      true,
      "offline"
     );
    
    if(isSuccess){
        String tmp = getRender();
        largePublish((baseTopic+"/render/").c_str(), getRender(), true);
        largePublish((baseTopic + "/state/").c_str(), "online", true);
        mqttClient.subscribe((baseTopic+"/Rx/").c_str(), 1);
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
    if (mqttClient.connected())
        mqttClient.loop();
    else if(checkKey("token")){
      connToMqttBroker(getValue("token"),0);

    }
}
