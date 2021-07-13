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
unsigned long movementResetUpdate = 0;
int ballSpeedY = 0;
int ballSpeedX = 0;
int xValue = 0;
int yValue = 0;

String messageToSend = "";

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
      if (movementResetUpdate == 0){
      movementResetUpdate = millis();
      }
      if(millis() - movementResetUpdate >= 2000){ 
        xValue = 0;
        yValue = 0;
        movementResetUpdate = 0;
        ballSpeedX = 0;
        ballSpeedY = 0;
      }
    
         
      if(yFinalLast - yFinal > 10 && yFinalLast - yFinal < 90){
        //Serial.println("Rechts");
        xValue = 1;
        ballSpeedY = (yFinalLast - yFinal) / 10;
      }
      if(yFinalLast - yFinal < -10 && yFinalLast - yFinal > -90){
        //Serial.println("Links");
        xValue = 2;
        ballSpeedY = abs(yFinalLast - yFinal) / 10;
      }

      if(xFinalLast - xFinal > 10 && xFinalLast - xFinal < 90){
        //Serial.println("Zurück");
        yValue = 1;
        ballSpeedX = (xFinalLast - xFinal) / 10;
      }
      if(xFinalLast - xFinal < -10 && xFinalLast - xFinal > -90){
        //Serial.println("Vor");
        yValue = 2;
        ballSpeedX = abs(xFinalLast - xFinal) / 10;
      }

      
      

      ballSpeedX = ballSpeedX > 5 ? 5 : ballSpeedX;
      ballSpeedY = ballSpeedY > 5 ? 5 : ballSpeedY;
     
      
      lastDirectionUpdate = 0;
      yFinalLast = yFinal;
      xFinalLast = xFinal;
      
      messageToSend = String(xValue) + "," + String(ballSpeedX) + "," + String(yValue) + "," + String(ballSpeedY) + "," + String(currentEmotion);

      Serial.println(messageToSend);
      
    }


     


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
