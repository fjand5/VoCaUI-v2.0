#include "./src/webserver.h"
#include "./src/config.h"
#include "./src/mqtt.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  setupConfig();
  

  // render
  renderChartView("Control Led Button","sin-cos-value","Sin Cos Value",R"({
        "labels":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30],
        "yMin":-1,
        "yMax":1
  })");
  renderButton("Control Led Button","on-build-led","On Led","{}",[](String key, String val){
    digitalWrite(D0, LOW);
    setValue("on-off-build-led","true");
  });
  renderButton("Control Led Button","off-build-led","Off Led","{}",[](String key, String val){
    digitalWrite(D0, HIGH);
    setValue("on-off-build-led","false");
  });

  renderToggle("Control Led Toggle","on-off-build-led","On/Off Led",R"({
        "on":"Bật",
        "off":"Tắt"
  })",[](String key, String val){
    setValue(key,val);
    if(val == "true"){
      digitalWrite(D0, LOW);
    }if(val == "false"){
      digitalWrite(D0, HIGH);
    }
  });
  setupWebserver();
  setupMqtt();
  pinMode(D0, OUTPUT);
  digitalWrite(D0, getValue("device") == "true" ? LOW : HIGH);
}
uint32_t mainTimer = millis();

float count =0;
float sig = 0.05;
void loop() {
  loopWebserver();
  loopMqtt();
  if(millis() - mainTimer > 2000){
    mainTimer = millis();
    addData(sin(count),"sin-cos-value","Sin Value",false,"#ff0000",30);
    // addData(cos(count),"sin-cos-value","Cos Value",false,"#0000ff",30);
    count+=sig;
    if(count>=3.1)
      sig=-0.05;
    if(count<=-3.1)
      sig=+0.05;
  }
}
