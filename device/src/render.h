#pragma once
#include <Arduino.h>
#include "./event.h"
#include "./json/ArduinoJson.h"
DynamicJsonDocument doc(8192);
JsonObject obj = doc.to<JsonObject>();
void renderComponent(String compt, String tab, String key, String name, String option){
  DynamicJsonDocument subDoc(256);
  deserializeJson(subDoc, option); 
  obj[tab][key]["name"]=name;
  obj[tab][key]["option"]=subDoc.as<JsonObject>();
  obj[tab][key]["type"]=compt;

}
void renderSlider(String tab, String key, String name, String option, EventFunc event = NULL){
  addEvent(key, event);
  renderComponent("input-slider", tab, key, name, option);
}
void renderToggle(String tab, String key, String name, String option, EventFunc event = NULL){
  addEvent(key, event);
  renderComponent("toggle", tab, key, name, option);
}
void renderButton(String tab, String key, String name, String option, EventFunc event = NULL){
  addEvent(key,event);
  renderComponent("button", tab, key, name, option);
}
void renderInputText(String tab, String key, String name, String option, EventFunc event = NULL){
  addEvent(key,event);
  renderComponent("input-text", tab, key, name, option);
}
void renderPassword(String tab, String key, String name, String option, EventFunc event = NULL){
  addEvent(key,event);
  renderComponent("password", tab, key, name, option);

}

String getRender() {
  String ret;
  serializeJson(obj,ret);
  return ret;
}
