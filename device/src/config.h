#pragma once
#define CONFIG_FILE "config.txt"
#include "./json/ArduinoJson.h"
#include <LittleFS.h>
#include <map>

long verContHost = 1;
std::map < String, String > ConfigContent;


void (*onConfigChange)(String key, String value) = NULL;
void setOnConfigChange(void (*func)(String key, String value)){
  onConfigChange = func;
}
// Mỗi dòng là một phần tử (một cặp key value) (key):(value)\n
void loadFileIntoConfig(String content) {
  while (content.indexOf("\n") >= 0) {
    String curLine = content.substring(0, content.indexOf("\n"));
    String key = curLine.substring(0, content.indexOf(":"));
    String value = curLine.substring(content.indexOf(":") + 1);
    ConfigContent[key] = value;
    content.remove(0, curLine.length() + 1);
  }
}
// Kiểm tra key có tồn tại không
bool checkKey(String key) {
  if (ConfigContent.find(key) != ConfigContent.end())
  {
    return true;
  }
  return false;
}

// Lấy giá trị của Key
String getValue(String key, String def = "") {
  if (checkKey(key))
    return ConfigContent[key];
  else
    return def;
}

// Lấy toàn bộ file content
String getValuesByString() {
  String ret = "";
  for (std::pair < String, String > e : ConfigContent) {
    String k = e.first;
    String v = e.second;
    ret += k + ":" + v + "\n";
  }
  return ret;
}
String getValuesByJson(){
  DynamicJsonDocument doc(8192);
  JsonObject obj = doc.to<JsonObject>();
  String ret;
  for (std::pair < String, String > e : ConfigContent) {
    String k = e.first;
    String v = e.second;
    obj[k]=v;
  }
  serializeJson(obj,ret);
  return ret;
}
// Gán giá trị cho key
void setValue(String key, String value, bool save = true) {
  ConfigContent[key] = value;
  verContHost ++;
  // nếu không yêu cầu lưu vào flash
  if (!save)
    return;

  File cfg_file = LittleFS.open(CONFIG_FILE, "w");
  if (!cfg_file) {
    cfg_file.close();
    return;
  }

  for (std::pair < String, String > e : ConfigContent) {
    String k = e.first;
    String v = e.second;
    cfg_file.print(k + ":" + v + "\n");
  }

  cfg_file.close();
  if(onConfigChange != NULL){
    onConfigChange(key,value);
  }
}

// Khởi tạo
void setupConfig() {
  if (!LittleFS.begin()) {
    return;
  }

  File cfg_file = LittleFS.open(CONFIG_FILE, "r");
  if (cfg_file) {
    String tmp = cfg_file.readString();
    loadFileIntoConfig(tmp);
  }
  cfg_file.close();

}

void loopConfig() {

}
