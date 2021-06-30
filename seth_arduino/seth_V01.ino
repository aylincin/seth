#include <Adafruit_NeoPixel.h>

#include <Adafruit_CircuitPlayground.h>

int TOUCH_PIN = 9;
int input = LOW;
int currentEmotion;

bool inputBlocked = false;
bool lockEmotion = false;
bool lockLocked = false;

unsigned long lockTimer = 0;

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();

  pinMode(TOUCH_PIN, INPUT);
  CircuitPlayground.strip.setBrightness(100);
  
}

void loop() {
 input = digitalRead(TOUCH_PIN);

 if(input == HIGH && !inputBlocked && !lockEmotion) {
  if(currentEmotion == 5){
    currentEmotion = 0;    
  }
  
  
  else{
    currentEmotion++;
  }
  
  setEmotion(currentEmotion);
  inputBlocked = true;
 } 
 
 if(input == LOW){
  inputBlocked = false;
  lockTimer = 0;
  lockLocked = false;
 }

 if(input == HIGH){
  if(lockTimer == 0){
    lockTimer = millis();
  }
  
  if(millis() - lockTimer >= 3000 && !lockLocked){
    lockEmotion = lockEmotion ? false : true;
    lockLocked = true;
    Serial.println(lockEmotion);
  }
 }
 
}


void setEmotion(int colorNumber) {
  switch(colorNumber){
    case 0:
      for(int i=0; i <= 10; i++){
        CircuitPlayground.setPixelColor(i, 100,0,50); //wut
      }
      break;
    case 1:
      for(int i=0; i <= 10; i++){
        CircuitPlayground.setPixelColor(i, 100,0,0); //wut
      }
      break;
    case 2:
      for(int i=0; i <= 10; i++){
        CircuitPlayground.setPixelColor(i, 100,100,0); //wut
      }
      break;
    case 3:
      for(int i=0; i <= 10; i++){
        CircuitPlayground.setPixelColor(i, 100,0,100); //wut
      }
      break;
    case 4:
      for(int i=0; i <= 10; i++){
        CircuitPlayground.setPixelColor(i, 50,0,100); //wut
      }
      break;
    case 5:
      CircuitPlayground.clearPixels();
      break;
  }
  
}
