#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#define TOUCH_PIN D8
#define LED_PIN D4

Adafruit_MPU6050 mpu;

const char* ssid = "nodeMCU";
const char* password = "password";
const char* host = "192.168.11.4";

WiFiClient client;


int input = LOW;
int currentEmotion;

bool inputBlocked = false;
bool lockEmotion = false;
bool lockLocked = false;

unsigned long lockTimer = 0;
Adafruit_NeoPixel pixels(12, LED_PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());



  

  pixels.begin();
  pixels.clear();
  pixels.show();

  pinMode(TOUCH_PIN, INPUT);
  
 //Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

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
      if (movementResetUpdate == 0){
      movementResetUpdate = millis();
      }
      if(millis() - movementResetUpdate >= 2000){ 
        xValue = 0;
        yValue = 0;
        movementResetUpdate = 0;
//        ballSpeedX = 0;
//        ballSpeedY = 0;
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
      
      if(client.connect(host, 80)){
        String url = "/update?value=" + messageToSend;
        client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: keep-alive\r\n\r\n");
      }
      
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
      pixels.clear();
      pixels.show();
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
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 100, 0, 50); //wut
        pixels.show();
      }
      break;
  }

}
