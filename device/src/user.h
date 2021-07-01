#pragma once
#define USER_FILE "user.txt"

#include <LittleFS.h>
#include "./jwt/ArduinoJWT.h"
#include "./json/ArduinoJson.h"
#include "Hash.h"
#include "utils.h"
ArduinoJWT jwt("random");
DynamicJsonDocument docUser(2048);

JsonObject objUser = docUser.to<JsonObject>();

bool saveUserContent() {
  File usr_file = LittleFS.open(USER_FILE, "w");
  if (!usr_file) {
    usr_file.close();
    return false;
  }
  serializeJson(objUser,usr_file);
  usr_file.close();
  return true;
}
bool createUser(String user, String pwd, int pms){
  DynamicJsonDocument subDocUser(256);
  JsonObject subObj = subDocUser.to<JsonObject>();
  if(objUser["user"].containsKey(user))
    return false;
  subObj["pwd"] = pwd;
  subObj["pms"] = pms;

  objUser["user"][user] = subObj;
  return saveUserContent();
}
bool changePassword(String user, String opwd,  String npwd){
  if(objUser["user"][user]["pwd"] == opwd){
    objUser["user"][user]["pwd"] = npwd;
    return saveUserContent();
  }
  return false;
}
bool checkHasRoot(){
  return objUser["user"].containsKey("root");
}
int getPermission(String user, String pwd){
  if(!objUser["user"].containsKey(user))
    return -1; // can't auth
  JsonObject theObj = objUser["user"][user].as<JsonObject>();
  if(theObj["pwd"].as<String>() == pwd)
    return theObj["pms"]; // user's permission
  return -1;
}
String createTokenFromUser(String user){
  JsonObject theObj = objUser["user"][user].as<JsonObject>();
  String pwd = theObj["pwd"];
  String hashPwd  = sha1(pwd);
  String theJwt = user + "." +hashPwd;

  return jwt.encodeJWT(theJwt);
}
int getPermisionFromToken(String token){
  String user;
  String hashPwd;
  String data;
  jwt.decodeJWT(token, data);
  user = splitString(data,".",0);
  hashPwd = splitString(data,".",1);
  JsonObject theObj = objUser["user"][user].as<JsonObject>();
  String pwd = theObj["pwd"].as<String>();
  if (hashPwd == sha1(pwd)){
    return getPermission(user, pwd);
  }
  return -1;
}
bool removeUser(String user){
  objUser["user"].as<JsonObject>().remove(user);
  return saveUserContent();
}
void setPermissionRead(String key, int pms){
  JsonObject readKeys;
  if(objUser.containsKey("readKeys"))
    readKeys = objUser["readKeys"].as<JsonObject>();
  else
    readKeys = objUser["readKeys"].to<JsonObject>();
  readKeys[key] = pms;
  saveUserContent();
}
void setPermissionWrite(String key, int pms){
  JsonObject writeKeys;
  if(objUser.containsKey("writeKeys"))
    writeKeys = objUser["writeKeys"].as<JsonObject>();
  else
    writeKeys = objUser["writeKeys"].to<JsonObject>();
  writeKeys.getOrAddMember(key).set(pms);

  saveUserContent();
}
bool checkPermissionRead(String key, int pms){
  JsonObject readKeys = objUser["readKeys"];
  if(!readKeys.containsKey(key))
    return true;
  if(readKeys[key].as<int>() <= pms)
    return true;
  else
    return false;
}
bool checkPermissionWrite(String key, int pms){
  JsonObject writeKeys = objUser["writeKeys"].as<JsonObject>();
  if(!writeKeys.containsKey(key))
    return true;
  if(writeKeys[key].as<int>() <= pms)
    return true;
  else
    return false;
}
String getUsersData(){
  String ret;
  serializeJson(docUser,ret);
  return ret;
}
void setupUser() {
  if (!LittleFS.begin()) {
    return;
  }
  File usr_file = LittleFS.open(USER_FILE, "r");
  if (usr_file) {
    String tmp = usr_file.readString();
    deserializeJson(docUser, tmp);
    serializeJsonPretty(docUser, Serial);

  }
  usr_file.close();
  if(!checkHasRoot())
    createUser("root","123",0);
  
}