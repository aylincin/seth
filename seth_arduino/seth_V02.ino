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

float yFinalLast = 0;
float xFinalLast = 0;
unsigned long lastDirectionUpdate = 0;

void loop() {
  float y = CircuitPlayground.motionY();
  float x = CircuitPlayground.motionX();
  float z = CircuitPlayground.motionZ();

  float yDegree = (y / 9.8) * 90;
  float yFinal = 0;
  float xDegree = (x / 9.8) * 90;
  float xFinal = 0;
  bool top = z > 0 ? true : false;


  if (top) {    
    if (yDegree < 0) {
      yFinal = yDegree + 360;
    }
    else {
      yFinal = yDegree;
    }

    if (xDegree < 0) {
      xFinal = xDegree + 360;
    }
    else {
      xFinal = xDegree;
    }
  }
  else {
    if (yDegree > 0) {
      yFinal = (90 - yDegree) + 90;
    }
    else {
      yFinal = (yDegree * -1) + 180;
    }

    if (xDegree > 0) {
      xFinal = (90 - xDegree) + 90;
    }
    else {
      xFinal = (xDegree * -1) + 180;
    }
  }

    if (lastDirectionUpdate == 0){
      lastDirectionUpdate = millis();
    }
    if(millis() - lastDirectionUpdate >= 50){      
      if(yFinalLast - yFinal > 10 && yFinalLast - yFinal < 90){
        Serial.println("Rechts");
      }
      if(yFinalLast - yFinal < -10 && yFinalLast - yFinal > -90){
        Serial.println("Links");
      }

      if(xFinalLast - xFinal > 10 && xFinalLast - xFinal < 90){
        Serial.println("Zurück");
      }
      if(xFinalLast - xFinal < -10 && xFinalLast - xFinal > -90){
        Serial.println("Vor");
      }
      
      lastDirectionUpdate = 0;
      yFinalLast = yFinal;
      xFinalLast = xFinal;
    }
     


  //Serial.println(yFinal);


  input = digitalRead(TOUCH_PIN);

  if (input == HIGH && !inputBlocked && !lockEmotion) {
    if (currentEmotion == 5) {
      currentEmotion = 0;
    }


    else {
      currentEmotion++;
    }

    setEmotion(currentEmotion);
    inputBlocked = true;
  }

  if (input == LOW) {
    inputBlocked = false;
    lockTimer = 0;
    lockLocked = false;
  }

  if (input == HIGH) {
    if (lockTimer == 0) {
      lockTimer = millis();
    }

    if (millis() - lockTimer >= 3000 && !lockLocked) {
      lockEmotion = lockEmotion ? false : true;
      lockLocked = true;
      Serial.println(lockEmotion);
    }
  }

}


void setEmotion(int colorNumber) {
  switch (colorNumber) {
    case 0:
      for (int i = 0; i <= 10; i++) {
        CircuitPlayground.setPixelColor(i, 100, 0, 50); //wut
      }
      break;
    case 1:
      for (int i = 0; i <= 10; i++) {
        CircuitPlayground.setPixelColor(i, 100, 0, 0); //wut
      }
      break;
    case 2:
      for (int i = 0; i <= 10; i++) {
        CircuitPlayground.setPixelColor(i, 100, 100, 0); //wut
      }
      break;
    case 3:
      for (int i = 0; i <= 10; i++) {
        CircuitPlayground.setPixelColor(i, 100, 0, 100); //wut
      }
      break;
    case 4:
      for (int i = 0; i <= 10; i++) {
        CircuitPlayground.setPixelColor(i, 50, 0, 100); //wut
      }
      break;
    case 5:
      CircuitPlayground.clearPixels();
      break;
  }

}
