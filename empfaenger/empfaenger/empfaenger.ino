/* Example sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board and Arduino UNO. More info: https://www.makerguides.com */
// Include the Arduino Stepper.h library:
#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

// WIFI VARIABLES
char * ssid_ap = "nodeMCU";
char * password_ap = "password";
IPAddress ip(192,168,11,4);
IPAddress gateway(192,168,11,1);
IPAddress subnet(255,255,255,0);
ESP8266WebServer server;
String dataWhichIsSend = "";

// OTHER VARIABLES

// Define number of steps per rotation:
int stepsPerRevolutionStepper = 2000;
int xDir = 1;
int yDir = 1;
int motorSpeed = 14;

// Wiring:
// Pin 8 to IN1 on the ULN2003 driver
// Pin 9 to IN2 on the ULN2003 driver
// Pin 10 to IN3 on the ULN2003 driver
// Pin 11 to IN4 on the ULN2003 driver
// Create stepper object called 'myStepper', note the pin order:
Stepper myStepperX = Stepper(stepsPerRevolutionStepper, D1, D3, D2, D4);
Stepper myStepperY = Stepper(stepsPerRevolutionStepper, D5, D7, D6, D8);

Adafruit_NeoPixel pixels(10, D10, NEO_KHZ800 + NEO_GRB);

unsigned long lastChange = 0;
//String input = "1,5,2,5,3";
String input = "0,0,0,0,0";

int delayForUpdate = 200;
bool stepperXActive = false;
int stepperXSpeed = 5000;
bool stepperYActive = false;
int stepperYSpeed = 5000;
bool unblockReceive = false;
unsigned long unblockReceiveTimer = 0;
int unblockDuration = 30000;

void setup() {
  
  // WIFI SETUP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(ssid_ap,password_ap);

  Serial.begin(115200);
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/update", handleUpdate);
  server.begin();

  // OTHER SETUP
  // Set the speed to 5 rpm:
  myStepperX.setSpeed(motorSpeed);
  myStepperY.setSpeed(motorSpeed);

  pinMode(D9, INPUT);

  pixels.begin();
                                  
}

void handleUpdate() {
  if(unblockReceive){
    input = server.arg("value");
  }
  else{
    input = "0,0,0,0,0";
  }
  
  //Serial.println(input);
//  server.send(200,"text/plain","Updated");
}

void loop() {  
  // WIFI LOOP
  server.handleClient();


  // OTHER LOOP
  if(digitalRead(D9) == LOW && !unblockReceive){
    Serial.println("Unblocked");
    unblockReceive = true;
  }

  if(unblockReceive){
    if(unblockReceiveTimer == 0){
      unblockReceiveTimer = millis();
    }
    if(millis() - unblockReceiveTimer >= unblockDuration){
      unblockReceive = false;
      unblockReceiveTimer = 0;
      Serial.println("Blocked");
    }
  }
  
  if(lastChange == 0){
    lastChange = millis();
  }

  if(millis() - lastChange >= delayForUpdate){
    Serial.println(input);
    
    if(getValue(input, ',', 0) == "0"){
      stepperXActive = false;
    }
    if(getValue(input, ',', 0) == "1"){
      xDir = 1;
      stepperXActive = true;
    }
    if(getValue(input, ',', 0) == "2"){
      xDir = -1;
      stepperXActive = true;
    }

    if(getValue(input, ',', 2) == "0"){
      stepperYActive = false;
    }
    if(getValue(input, ',', 2) == "1"){
      yDir = 1;
      stepperYActive = true;
    }
    if(getValue(input, ',', 2) == "2"){
      yDir = -1;
      stepperYActive = true;
    }
    if(getValue(input, ',', 1) != "0"){
      stepperXSpeed = getValue(input, ',', 1).toInt() * 1000;
    } else {
      stepperXSpeed = 1;
    }
    if(getValue(input, ',', 3) != "0"){
      stepperYSpeed = getValue(input, ',', 3).toInt() * 1000;
    } else {
      stepperYSpeed = 1;
    }
    
    lastChange = 0;
    setEmotion(getValue(input, ',', 4).toInt());
    
    //myStepperX.setSpeed(stepperXSpeed);
    //myStepperY.setSpeed(stepperYSpeed);
    
  }
    
    if(stepperXActive){
      myStepperX.step(xDir);
    }
    if(stepperYActive){
      myStepperY.step(yDir);
    }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void detectMagnet() {
  Serial.println("Magnet detected");
}


void setEmotion(int colorNumber) {
  //Serial.println(colorNumber);
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
