#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#define TOUCH_PIN D8
#define LED_PIN D4

Adafruit_MPU6050 mpu;


int input = LOW;
int currentEmotion;

bool inputBlocked = false;
bool lockEmotion = false;
bool lockLocked = false;

unsigned long lockTimer = 0;
Adafruit_NeoPixel pixels(12, LED_PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  Serial.begin(9600);

  pixels.begin();
  pixels.show();
//  pixels.setBrightness(100);  

  pinMode(TOUCH_PIN, INPUT);
  
// Try to initialize!
//  if (!mpu.begin()) {
//    Serial.println("Failed to find MPU6050 chip");
//    while (1) {
//      delay(10);
//    }
//  }
//  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

}

float yFinalLast = 0;
float xFinalLast = 0;
unsigned long lastDirectionUpdate = 0;

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  float y = a.acceleration.x;
  float x = a.acceleration.y;
  float z = a.acceleration.z;

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
  Serial.println(colorNumber);
  switch (colorNumber) {
    case 0:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 100, 0, 50); //wut
        pixels.show();
      }
      break;
    case 1:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 100, 0, 0); //wut
        pixels.show();
      }
      break;
    case 2:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 100, 100, 0); //wut
        pixels.show();
      }
      break;
    case 3:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 100, 0, 100); //wut
        pixels.show();
      }
      break;
    case 4:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 50, 0, 100); //wut
        pixels.show();
      }
      break;
    case 5:
      pixels.clear();
      pixels.show();
      break;
  }

}
