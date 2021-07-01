#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "./json/ArduinoJson.h"

String IpAddress2String(const IPAddress& ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}

//"[{\"data\": [11, 70, 95, 100, 120, 257, 271, 300, 321, 383, 450],\"label\": \"Cây lương thực\",\"borderColor\": \"#3e95cd\"},{\"data\": [22, 22, 33, 44, 257, 271, 300, 321, 66, 44],\"label\": \"Cây cảnh\",\"borderColor\": \"#3e9500\"}]"
void addData(float data, String key, String label, bool save = false, String color = "#ff0000", int maxSize = 30){
  bool isAddData = false;
  DynamicJsonDocument docData(2048);
  DeserializationError err = deserializeJson(docData, getValue(key));
  if(err) 
    docData.to<JsonArray>();
  JsonArray array = docData.as<JsonArray>();
  for(JsonVariant v : array) {
      JsonObject obj = v.as<JsonObject>();
      if(obj["label"].as<String>() == label){
        obj["borderColor"] = color;
        JsonArray dataArray = obj["data"].as<JsonArray>();
        dataArray.add(data);
        if(dataArray.size()>maxSize){
          dataArray.remove(0);
        }
        isAddData = true;
        break;
      }
  }

  if(!isAddData){
    JsonObject nested = array.createNestedObject();
    nested["label"] = label;
    nested["borderColor"] = color;
    nested.createNestedArray("data").add(data);
  }
  String ret;
  serializeJson(docData, ret);
  // serializeJsonPretty(docData, Serial);
  
  setValue(key, ret, save);
}
String splitString(String str, String delim, uint16_t pos){
  String tmp = str;
  for(int i=0; i<pos; i++){
    tmp = tmp.substring(tmp.indexOf(delim)+1);

    
    if(tmp.indexOf(delim)== -1 
    && i != pos -1 )
      return "";
  }
  return tmp.substring(0,tmp.indexOf(delim));
}
// Stream dữ liệu từ bộ nhớ
// class ProgMemVar: public Stream {
//   private:
//     char* _addrMem;
//     int _len = 0;
//     int _availableLen = 0;
//   public:
//     ProgMemVar(char* addrMem) {
//       _addrMem = addrMem;

//       while (pgm_read_byte_near(_addrMem + _len)) {
//         _len++;
//       }
//       _availableLen = _len;
//     };
//     uint32_t length() {
//       return _len;
//     };
//     int available(void)
//     {
//       return _availableLen;
//     };
//     int read() {
//       int ret;
//       if (_availableLen > 0)
//         ret = pgm_read_byte_near(_addrMem + _len - _availableLen--);
//       else
//         ret = -1;

//       return ret;
//     };
//     int peek() {
//       int ret;
//       if (_availableLen > 0)
//         ret = pgm_read_byte_near(_addrMem + _len - _availableLen);
//       else
//         ret = -1;

//       return ret;
//     };
//     String readString() {
//       String ret;
//       for (int i = (_len - _availableLen); i < _len; i++) {
//         ret += char(pgm_read_byte_near(_addrMem + i));
//       }

//       return ret;
//     };
//     size_t readBytes(char *buffer, size_t length) {
//       size_t got = 0;

//       for (int i = (_len - _availableLen); i < (_len - _availableLen + length); i++) {
//         if (i > _len)
//           return got;
//         got++;
//         buffer[i] = char(pgm_read_byte_near(_addrMem + i));
//       }
//       return got;
//     };
//     size_t write(const uint8_t *buffer, size_t size) {
//       return 0;
//     };
//     size_t write(uint8_t data) {
//       return 0;
//     };
//     void flush() {};
// };
