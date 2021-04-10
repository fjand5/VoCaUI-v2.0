#pragma once
#define APSSID "ESP-TaoLao"
#define APPSK  "78787878"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "config.h"
#include "render.h"
#include "event.h"
#include "dist.h"
#include "utils.h"
ESP8266WebServer server(80);
bool isConnect = false;

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
  renderInputText("system","token","Token ngoinhaiot.com",R"({
    "newLine":true,
    "label":"Token",
    "showSplit":true
    }))",[](String key, String val){
            setValue(key,val);
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
  WiFi.mode(WIFI_AP_STA);
  setValue("sta_ip","");
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
  server.on("/recv", HTTP_POST,[]() {

    // Lọc dữ liệu
    for (int i = 0; i < server.args(); ++i)
    {
      String key = server.argName(i);
      String value = server.arg(i);
      if (key.indexOf(":") >= 0
          || value.indexOf(":") >= 0
         ) {
        addComonHeader();
        server.send(400, "text/html", "Chuỗi không hợp lệ");
        return;
      }
    }
    

    for (int i = 0; i < server.args(); ++i)
    {
      String key = server.argName(i);
      String value = server.arg(i);
      execEvent(key,value);
    }
    addComonHeader();

    server.send(200, "text/html", "Đã nhận lệnh");
  });

  // Server truyền dữ liệu cho client
  server.on("/trans", HTTP_GET, []() {
    // kiểm tra phiên bản của dữ liệu
 
    long verContClient = server.header("Version-Content-Client").toInt();
    if (verContClient == verContHost){
      addComonHeader();
      server.send(403, "text/plain", "nothing change");
      return;
    }
    addComonHeader();
    server.sendHeader("Version-Content-Host", String(verContHost));
    server.send(200, "text/plain", getValuesByString());

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

}

void loopWebserver() {
  server.handleClient();
}
