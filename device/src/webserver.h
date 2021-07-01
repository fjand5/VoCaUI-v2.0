#pragma once
#define APSSID "ESP-TaoLao"
#define APPSK  "78787878"
#include <list>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "./ws/WebSocketsServer.h"
#include <LittleFS.h>
#include "config.h"
#include "render.h"
#include "event.h"
#include "dist.h"
#include "utils.h"
#include "mqtt.h"
#include "user.h"

DynamicJsonDocument webDoc(512);
void (*onClientCommit)(uint8_t num, String);
ESP8266WebServer server(80);
std::list < uint8_t > listLogin;
WebSocketsServer webSocket = WebSocketsServer(81);
bool isConnect = false;

bool checkIsLogin(uint8_t num){
return (std::find(listLogin.begin(), listLogin.end(), num) != listLogin.end());
 
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            break;
        case WStype_CONNECTED:
            break;
        case WStype_TEXT:
            String data;
            for (unsigned int i = 0; i < length; i++) {
                data+=(char)payload[i];
            }

            if(data== "ping"){
              webSocket.sendTXT(num, "pong");
              return;
            }
            if(onClientCommit != NULL)
              onClientCommit(num, data);
            break;
    }

}
void addComonHeader(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.sendHeader("Access-Control-Expose-Headers", "*");
 
}
void renderSystem(){
  renderInputText("system","sta_id","Station wifi",R"({
    "label":"Wifi"
    }))",[](String key, String val){
      setValue(key,val);
    });
  renderPassword("system","sta_pass","Station Password",R"({
        "newLine":true,
        "label":"Password",
        })",[](String key, String val){
            setValue(key,val);
    });
  renderButton("system","try_conn","Thử kết nối",R"({
        "newLine":true,
        "showSplit":true
    }))",[](String key, String val){
      Serial.println("Thử kết nối");
      uint32 preTime = millis();
      if (checkKey("sta_id") && checkKey("sta_pass")) {
        Serial.println("Dang Thử kết nối");
        setValue("sta_ip","");
        WiFi.begin(getValue("sta_id"), getValue("sta_pass"));
        while (WiFi.status() != WL_CONNECTED
              && (millis()-preTime) < 30000) {
          delay(100);
          Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("");
          Serial.println("WiFi connected");
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          setValue("sta_ip",IpAddress2String(WiFi.localIP()));
          isConnect = true;
        }
      } else {
        Serial.println("Not found station info!!");
      }
    });

  renderInputText("system","ap_id","Access point wifi",R"({
    "label":"Wifi"
    }))",[](String key, String val){
            setValue(key,val);
    });
  renderPassword("system","ap_pass","Access point Password",R"({
        "newLine":true,
        "label":"Password",
        "showSplit":true
        })",[](String key, String val){
            setValue(key,val);
    });
  renderInputText("system","mqtt-token","Token ngoinhaiot.com",R"({
    "label":"Token",
    "newLine":true
    }))",[](String key, String val){
            setValue(key,val);
    });
  renderButton("system","conn_mqtt","Kết nối",R"({
    "newLine":true,
    "showSplit":true
    }))",[](String key, String val){
      connToMqttBroker(getValue("mqtt-token"));
    });
  renderButton("system","reset","Reset hệ thống",R"({
    }))",[](String key, String val){
      ESP.reset();
    });
  renderButton("system","restart","Restart hệ thống",R"({
    }))",[](String key, String val){
      ESP.restart();
    });

}
void setupWebserver() {
  renderSystem();
  setupUser();
  WiFi.mode(WIFI_AP_STA);
  if (checkKey("sta_id") && checkKey("sta_pass")) {
  // if (true) {

    WiFi.begin(getValue("sta_id"), getValue("sta_pass"));
    while (WiFi.status() != WL_CONNECTED
           && millis() < 30000) {
      delay(100);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      setValue("sta_ip",IpAddress2String(WiFi.localIP()));
      isConnect = true;
    }
  } else {
    Serial.println("Not found station info!!");
  }
  WiFi.softAP(getValue("ap_id", "vocauiu2"), getValue("ap_pass", "12345678"));


  // client truyền dữ liệu cho server
  onClientCommit = [](uint8_t num, String data){
    DeserializationError err = deserializeJson(webDoc, data);
    if (!err){
        JsonObject obj = webDoc.as<JsonObject>();
        String cmd = obj["cmd"];
        String token = obj["token"];
        int ctxPms = getPermisionFromToken(token);
        serializeJsonPretty(webDoc, Serial);

        
        if(cmd != "lou" && cmd != "lgn" && ctxPms < 0){
          webSocket.sendTXT(num,"");
          return;
        }

        
        if(cmd == "get" && checkIsLogin(num)){
          String key = obj["key"];
          String res = "{\"key\":\"" + key +"\",\"val\":\""+getValue(key)+"\"}";
          
          webSocket.sendTXT(num,res.c_str());
        }
        if(cmd == "gal" && checkIsLogin(num)){
          webSocket.sendTXT(num,getValuesByString().c_str());
        }
        if(cmd == "exe" && checkIsLogin(num)){
          String key = obj["key"];
          String val = obj["val"];
          execEvent(key, val);

        }
        // register
        if(cmd == "reg" ){
          if(ctxPms != 0){
            webSocket.sendTXT(num,"err:permission");
            return;
          }
          String user = obj["user"];
          String pwd = obj["pwd"];
          int pms = obj["pms"];
          bool ret = createUser(user, pwd, pms);
          if (ret)
            webSocket.sendTXT(num,(String("reg:")+ ret).c_str());
          else
            webSocket.sendTXT(num,"err:can't create user");



        }
        // login
        if(cmd == "lgn"){
          String user = obj["user"];
          String pwd = obj["pwd"];
          if(getPermission(user,pwd)>=0){
            webSocket.sendTXT(num,(String("token:")+createTokenFromUser(user)).c_str());
        Serial.println(createTokenFromUser(user));

            listLogin.push_front(num);
            listLogin.unique();
            return;
          }
          webSocket.sendTXT(num,"err:can't auth");

        }
        // log out

        if(cmd == "lou"){
            listLogin.remove(num);
        }
        // change password
        if(cmd == "cpw"){
          String user = obj["user"];
          String opwd = obj["opwd"];
          String npwd = obj["npwd"];
          webSocket.sendTXT(num,(String("cpw:")+changePassword(user, opwd, npwd)).c_str());

        }
        // get users data
        if(cmd == "gud"){
          if(ctxPms == 0){
          webSocket.sendTXT(num,(String("gud:")+getUsersData()).c_str());
            return;
          }else{
            webSocket.sendTXT(num,"err:permission");
          }
        }
        // set permission read 
        if(cmd == "spr"){
          if(ctxPms == 0){
            String key = obj["key"];
            int pms = obj["pms"];
            setPermissionRead(key,pms);
            webSocket.sendTXT(num,(String("spr:")+getUsersData()).c_str());
            return;
          }else{
            webSocket.sendTXT(num,"err:permission");
          }
        }
    }
  };

  // Server truyền dữ liệu cho client
  setOnConfigChange([](String key, String val){
    String sendValue = key+":"+val;
    for (auto rit=listLogin.rbegin(); rit!=listLogin.rend(); ++rit){
        webSocket.sendTXT(*rit, sendValue.c_str());
    }
  });


  server.on("/", HTTP_GET, []() {
    addComonHeader();
    server.sendHeader("Content-Encoding","gzip");
    server.send(200, "text/html", index_html, index_html_length);

  });
  server.on("/css/chunk-vendors.css", []() {
    addComonHeader();
    server.sendHeader("Content-Encoding","gzip");
    server.send(200, "text/css", vendor_css, vendor_css_length);

  });
  server.on("/css/app.css", []() {
    addComonHeader();
    server.sendHeader("Content-Encoding","gzip");
    server.send(200, "text/css", app_css, app_css_length);

  });
  server.on("/js/chunk-vendors.js", []() {
    addComonHeader();
    server.sendHeader("Content-Encoding","gzip");
    server.send(200, "application/javascript", vendor_js, vendor_js_length);

  });
  server.on("/js/app.js", []() {
    addComonHeader();
    server.sendHeader("Content-Encoding","gzip");
    server.send(200, "application/javascript", app_js, app_js_length);

  });
  server.on("/favicon.ico", []() {
    addComonHeader();
    server.sendHeader("Content-Encoding","gzip");
    server.send(200, "image/x-icon", favicon_ico, favicon_ico_length);

  });
  server.on("/render", []() {
    addComonHeader();
    server.send(200, "application/json", getRender());

  });  
  server.onNotFound([]() {
    addComonHeader();
    server.send(200, "text/plain", "");
  });

  const char * headerkeys[] = {"Version-Content-Client"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();


  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loopWebserver() {
  server.handleClient();
  webSocket.loop();
}
