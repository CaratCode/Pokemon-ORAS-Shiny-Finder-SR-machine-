#include<EEPROM.h>
void setup() {
 Serial.begin(9600);
 //setup "chain" by writing initial values into memory
 EEPROM.write(0,0/255);
 EEPROM.write(1,0%255);

}

void loop() {
  int s=EEPROM.read(0)*255+EEPROM.read(1);
  Serial.println(s);// put your main code here, to run repeatedly:
  delay(1000);
}
