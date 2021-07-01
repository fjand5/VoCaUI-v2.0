#pragma once
#include <Arduino.h>
#include "./event.h"
#include "./json/ArduinoJson.h"
DynamicJsonDocument docRender(8192);
JsonObject objRender = docRender.to<JsonObject>();
void renderComponent(String compt, String tab, String key, String name, String option){
  DynamicJsonDocument subDoc(1024);
  deserializeJson(subDoc, option); 
  objRender[tab][key]["name"]=name;
  objRender[tab][key]["option"]=subDoc.as<JsonObject>();
  objRender[tab][key]["type"]=compt;

}
void renderChartView(String tab, String key, String name, String option){
  Serial.println(option);
  renderComponent("chart-view", tab, key, name, option);
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
  serializeJson(objRender,ret);
  return ret;
}
