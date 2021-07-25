#include <ESP8266WiFi.h> //library to use wifi module of esp
#include <Adafruit_NeoPixel.h> // library to control ws2812b led strips
#include <Adafruit_MPU6050.h> // library to use gyroscope sensor
#define TOUCH_PIN D8 // init touch pin
#define LED_PIN D4 // init led pin

Adafruit_MPU6050 mpu; //init gyroscope(mpu) sensor

// WIFI Network Variables
const char* ssid = "nodeMCU";
const char* password = "password";
const char* host = "192.168.11.4";

WiFiClient client;


int input = LOW; // init touch input value
int currentEmotion; // init variable for emotion color

bool inputBlocked = false; // init variable to block the input
bool lockEmotion = false; // init variable to lock emotion color
bool lockLocked = false; // init variable to lock/unlock the lockEmotion variable

unsigned long lockTimer = 0; // init variable for lockTimer to lock/unlock lockEmotion
Adafruit_NeoPixel pixels(12, LED_PIN, NEO_GRB + NEO_KHZ800); // init of led strip
void setup() {
  //Starts wifi and connects to server
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());

  //activates neopixel strip and clears the color of pixels
  pixels.begin(); 
  pixels.clear();
  pixels.show();

  //sets touch sensor pin
  pinMode(TOUCH_PIN, INPUT);
  
  //starts mpu sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); //sets range to 8G

}

float yFinalLast = 0; //init variable of last value for yFinal
float xFinalLast = 0; //init variable of last value for xFinal
unsigned long lastDirectionUpdate = 0; //init variable for lastDirectionUpdate timer
unsigned long movementResetUpdate = 0; //init variable for movementResetUpdate timer
int ballSpeedY = 0; //init variable ballSpeedY which will be send
int ballSpeedX = 0; //init variable ballSpeedX which will be send
int xValue = 0; //init variable xValue which will be send
int yValue = 0; //init variable yValue which will be send

String messageToSend = ""; //init of string which will be send

void loop() {
  //starts receiving data of mpu sensor
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  float y = a.acceleration.x; //init value for y axis of mpu
  float x = a.acceleration.y; //init value for x axis of mpu
  float z = a.acceleration.z; //init value for z axis of mpu

  float yDegree = (y / 9.8) * 90; //init and calculation of yDegree to receive values between 0 and 90
  float yFinal = 0; //init variable yFinal which will be set in loop after calculation
  float xDegree = (x / 9.8) * 90; //init and calculation of xDegree to receive values between 0 and 90
  float xFinal = 0; //init variable xFinal which will be set in loop after calculation
  bool top = z > 0 ? true : false; //init and check if chip is upside down



  if (top) { //checks if chip is upside down
    //Values will be set to 0-90 or 270-360 depending if yDegree/xDegree is positive or negative
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
    //Values will be set to 90-180 or 180-270 depending if yDegree/xDegree is positive or negative
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

    if (lastDirectionUpdate == 0){ // Timer will start if its not already running
      lastDirectionUpdate = millis();
    }
    
    if(millis() - lastDirectionUpdate >= 50){   // check if difference of millis() and start point is bigger then 50ms
      if (movementResetUpdate == 0){ // Timer will start if its not already running
      movementResetUpdate = millis();
      }
      if(millis() - movementResetUpdate >= 2000){  // check if difference of millis() and start point is bigger then 2s
        xValue = 0; // resets xValue to 0
        yValue = 0; // resets xValue to 0
        movementResetUpdate = 0; // resets timer so it can be activated in the next iteration of the loop
      }
    
         
      if(yFinalLast - yFinal > 20 && yFinalLast - yFinal < 90){ //checks which direction the sensors y axis is moving and calculates speed by difference of lastValue and current value
        //Serial.println("Rechts");
        xValue = 1;
        ballSpeedY = (yFinalLast - yFinal) / 10;
      }
      if(yFinalLast - yFinal < -20 && yFinalLast - yFinal > -90){ //checks which direction the sensors y axis is moving and calculates speed by difference of lastValue and current value
        //Serial.println("Links");
        xValue = 2;
        ballSpeedY = abs(yFinalLast - yFinal) / 10;
      }

      if(xFinalLast - xFinal > 20 && xFinalLast - xFinal < 90){ //checks which direction the sensors x axis is moving and calculates speed by difference of lastValue and current value
        //Serial.println("Zurück");
        yValue = 1;
        ballSpeedX = (xFinalLast - xFinal) / 10;
      }
      if(xFinalLast - xFinal < -20 && xFinalLast - xFinal > -90){ //checks which direction the sensors x axis is moving and calculates speed by difference of lastValue and current value
        //Serial.println("Vor");
        yValue = 2;
        ballSpeedX = abs(xFinalLast - xFinal) / 10;
      }

      
      

      ballSpeedX = ballSpeedX > 5 ? 5 : ballSpeedX; //sets the rotation speed of the x axis
      ballSpeedY = ballSpeedY > 5 ? 5 : ballSpeedY; //sets the rotation speed of the y axis
     
      
      lastDirectionUpdate = 0; // resets timer so it can be activated in the next iteration of the loop
      yFinalLast = yFinal; // sets the lastValue variable to actual value
      xFinalLast = xFinal; // sets the lastValue variable to actual value
      
      messageToSend = String(xValue) + "," + String(ballSpeedX) + "," + String(yValue) + "," + String(ballSpeedY) + "," + String(currentEmotion); //building string which will be send via wifi to server

      Serial.println(messageToSend); //debug messageToSend
      
      if(client.connect(host, 80)){ //if connected to server
        String url = "/update?value=" + messageToSend; //value is attached to url
        client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: keep-alive\r\n\r\n"); // message is send to server
      }
      
    }

    

  input = digitalRead(TOUCH_PIN); //reads touch sensor state

  if (input == HIGH && !inputBlocked && !lockEmotion) { //if touch sensor is pressed and not blocked by variables
    if (currentEmotion == 5) { //resets color counter if 5 is reached
      currentEmotion = 0;
    }


    else { //increases color counter
      currentEmotion++;
    }
    setEmotion(currentEmotion); // method call to set the color of the neopixel strip
    inputBlocked = true; //input will be blocked 
  }

  if (input == LOW) { //checks if touch is not pressed anymore
    inputBlocked = false; //input is unblocked
    lockTimer = 0; // resets timer so it can be activated in the next iteration of the loop
    lockLocked = false; //unlocks the lock of colorlock
  }

  if (input == HIGH) { //checks if touch sensor is pressed
    if (lockTimer == 0) { //starts timer for locking
      lockTimer = millis();
    }

    if (millis() - lockTimer >= 3000 && !lockLocked) { //checks if touch sensor is pressed for longer then 3s and color is not already locked
      lockEmotion = lockEmotion ? false : true; //locks color
      lockLocked = true; //locks the color lock
      Serial.println(lockEmotion); //Debug of lockEmotion state
    }
  }

}

// method to set the neopixel strip color by colorNumber
void setEmotion(int colorNumber) {
  Serial.println(colorNumber);
  switch (colorNumber) {
    case 0:
      pixels.clear();
      pixels.show();
      break;
    case 1:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 100, 0, 0); //red 
        pixels.show();
      }
      break;
    case 2:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 100, 100, 0); //yellow
        pixels.show();
      }
      break;
    case 3:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 0, 0, 100); //blue
        pixels.show();
      }
      break;
    case 4:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 0, 100, 0); //green
        pixels.show();
      }
      break;
    case 5:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 100, 0, 50); //purple
        pixels.show();
      }
      break;
  }

}
