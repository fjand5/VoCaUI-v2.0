#include "./src/webserver.h"
#include "./src/config.h"
#include "./src/mqtt.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  setupConfig();
  

  // render
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
void loop() {
  loopWebserver();
  loopMqtt();
}
