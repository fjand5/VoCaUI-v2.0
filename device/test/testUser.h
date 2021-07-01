#include "../src/user.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("khoi tao he thong");

  setupUser();
//   removeUser("root");
  String token = createTokenFromUser("guest");
  Serial.println(String("token: ") + token);
  Serial.println(String("pms: ")+getPermisionFromToken(token));
  



    
  

}
void loop() {
}
