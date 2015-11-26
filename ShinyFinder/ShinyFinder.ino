#include <Servo.h>
#include <EEPROM.h>

const int sensorPin=0;
int lightLevel, high=1023, low=0;
int darkThreshold=100;/*adjust this darkness level based on lighting conditions 
                      (100 should be fine if your photoresistor is closed well from surrounding light and only exposed to d.s. screen)*/
int memStorage[]={0,1};//two "slots" being used for memory storage are 0 and 1
//before using EEPROM.read use EEPROM.write in a separate program to set memory values to 0 (when program stops whatever values are in memory remain until they are rewritten) 
//since each slot is one (unsigned) byte (0 to 255), memory slot 0 holds chain/255 and slot 1 holds chain%255 (chain is the integer below) so that your chain can exceed 255 (limit for this would be 65,026)
//memory read as memStorage[0]*255 + memStorage[1] or (chain/255)*255 + (chain%255) the modulus part is due to chain/255 being integer division and truncating the decimal (so (chain/255)*255 != chain)
int chain=EEPROM.read(memStorage[0])*255 + EEPROM.read(memStorage[1]);//int that tell us how long our "chain" is (how many encounters)

Servo A,S;
int pinA=9,pinS=7;//servo for A button attach to pin9, servo for restart button attach to pin7

void setup() {
 Serial.begin(9600);
 Serial.print("Current Chain:");Serial.println(chain);
  boolean ready=false;
  while(!ready){
    while(Serial.available()>0){
      String signal=Serial.readString();
      if(signal=="start")
        ready=true;
    }
  }
  //starts program when you type "start" into the serial, start program during intro clip of pokemon game
  Serial.println("Starting Shiny Searcher!");
  A.attach(pinA);
  A.write(90);
  S.attach(pinS);
  S.write(0);
  S.detach();
}

void loop() {
 
 startgame();
  //convert voltage to light level (low V = bright, high v = dark) 
 lightLevel= abs(analogRead(sensorPin)-1023);
 //checkLightLevels();          
  enter();

  

}
//edit this function depending on the number of interactions you have before the battle starts
void startgame(){
  press(A);//exit opening animatioin
  delay(2000);
  press(A);//go through groudon screen
  delay(4500);
  press(A);//go through menu/ START GAME
  delay(3500);
  press(A);//talk to regi


  delay(1000); //IMPORTANT GIVES THE THING TIME SINCE SCREEN IS KIND OF DIMMED DURING TALK

  
}
void checkLightLevels(){
    delay(100);
    Serial.println(lightLevel);
    if(lightLevel<darkThreshold)
      Serial.println("it's dark");
}
void enter(){
    if(lightLevel<darkThreshold){
      int blackoutTime=0;
      Serial.println("battle start!");
      while(lightLevel<darkThreshold){
          delay(10);
          blackoutTime+=10;
          lightLevel=abs(analogRead(sensorPin)-1023);
      }  
      if(blackoutTime>10000){
        checkShiny(blackoutTime);
      }else{
        do{
           delay(10);
           blackoutTime+=10;
           lightLevel=abs(analogRead(sensorPin)-1023);
        }while(lightLevel<darkThreshold);
        
        if(blackoutTime>10000)
          checkShiny(blackoutTime);
        else{
           Serial.println(lightLevel);
            Serial.println("Revoke!");
            SR();
        }
      }
    }else{
      lightLevel= abs(analogRead(sensorPin)-1023);
      enter();
    }
    
}
void checkShiny(int blackoutTime){
      if(blackoutTime<12000 && blackoutTime > 10000){
        addCount();
        Serial.println("No Shiny");
        Serial.print("\t Animation Time:");Serial.println(blackoutTime);
  
        Serial.print("\t Chain:");Serial.println(chain);
        SR();
      }else if(blackoutTime>=12000){
        addCount();
        Serial.println("Shiny encountered!");
        Serial.print("\t Soft Resets:");Serial.println(chain);
        Serial.print("\t Animation Time:");Serial.println(blackoutTime);
        S.detach();A.detach();
        while(true){}
    }


}

//uses vilros microservo
void press(Servo servo){
  servo.attach(pinA);
  for(int i=90;i>=40;i-=2){
        servo.write(i);
        delay(10);
  }
  for(int j=40;j<=90;j+=2){
        servo.write(j);
        delay(10);
  }
  servo.detach();
  //delay(1000);
}
//uses radioshack microservo
void press(Servo servo,boolean radioshack){
  Serial.println("yo");
  if(radioshack){
      servo.attach(pinS);
      for(int i=0;i<=20;i+=2){
        servo.write(i);
        delay(30);
      }
      for(int j=20;j>=0;j-=1){
        servo.write(j);
        delay(20);
      }
      servo.detach();
  }
}
void SR(){
  press(S,true);
  delay(11500);
}
void addCount(){
  chain++;
  EEPROM.write(memStorage[0],chain/255);
  EEPROM.write(memStorage[1],chain%255);
}

