#include "./src/webserver.h"
#include "./src/config.h"
#include "./src/mqtt.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  setupConfig();
  

  // render
  renderSlider("PWM Led","pwm FULL","Led FULL",R"({
    "newLine":true,
    "min":0,
    "max":1023,
    "step":1
  })",[](String key, String val){
  setValue(key,val);
    setValue("pwmD1",val);
    setValue("pwmD2",val);
    setValue("pwmD5",val);
    setValue("pwmD7",val);
    analogWrite(D1, val.toInt());
    analogWrite(D2, val.toInt());
    analogWrite(D5, val.toInt());
    analogWrite(D7, val.toInt());
  });
  
  renderSlider("PWM Led","pwmD1","Led D1",R"({
    "newLine":true,
    "min":0,
    "max":1023,
    "step":1
  })",[](String key, String val){
    setValue(key,val);
    analogWrite(D1, val.toInt());
  });
  renderSlider("PWM Led","pwmD2","Led D2",R"({
    "newLine":true,
    "min":0,
    "max":1023,
    "step":1
  })",[](String key, String val){
    setValue(key,val);
    analogWrite(D2, val.toInt());
  });
  renderSlider("PWM Led","pwmD5","Led D5",R"({
    "newLine":true,
    "min":0,
    "max":1023,
    "step":1
  })",[](String key, String val){
    setValue(key,val);
    analogWrite(D5, val.toInt());
  });
  renderSlider("PWM Led","pwmD7","Led D7",R"({
    "newLine":true,
    "min":0,
    "max":1023,
    "step":1
  })",[](String key, String val){
    setValue(key,val);
    analogWrite(D7, val.toInt());
  });
  setupWebserver();
  setupMqtt();
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D7, OUTPUT);

  analogWrite(D1, getValue("pwmD1").toInt());
  analogWrite(D2, getValue("pwmD2").toInt());
  analogWrite(D5, getValue("pwmD5").toInt());
  analogWrite(D7, getValue("pwmD7").toInt());
}
void loop() {
  loopWebserver();
  loopMqtt();
}
