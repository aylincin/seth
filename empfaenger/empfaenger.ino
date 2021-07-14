/* Example sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board and Arduino UNO. More info: https://www.makerguides.com */
// Include the Arduino Stepper.h library:
#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

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
int stepsPerRevolutionStepperX = 4;
int stepsPerRevolutionStepperY = 4;
int motorSpeed = 5000;

// Wiring:
// Pin 8 to IN1 on the ULN2003 driver
// Pin 9 to IN2 on the ULN2003 driver
// Pin 10 to IN3 on the ULN2003 driver
// Pin 11 to IN4 on the ULN2003 driver
// Create stepper object called 'myStepper', note the pin order:
Stepper myStepperX = Stepper(stepsPerRevolutionStepperX, D1, D3, D2, D4);
Stepper myStepperY = Stepper(stepsPerRevolutionStepperY, D5, D7, D6, D8);

unsigned long lastChange = 0;
//String input = "1,5,2,5";
String input = "0,0,0,0";

int delayForUpdate = 1000;
bool stepperXActive = false;
int stepperXSpeed = 5000;
bool stepperYActive = false;
int stepperYSpeed = 5000;

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
  
}

void handleUpdate() {
  input = server.arg("value");
  Serial.println(input);
//  server.send(200,"text/plain","Updated");
}

void loop() {  
  // WIFI LOOP
  server.handleClient();

  // OTHER LOOP
  /*if(digitalRead(D9) == LOW){
    Serial.println("MAGNET POWER");
  }*/
  
  if(lastChange == 0){
    lastChange = millis();
  }

  if(millis() - lastChange >= delayForUpdate){
    Serial.println(input);
    
    if(getValue(input, ',', 0) == "0"){
      stepperXActive = false;
    }
    if(getValue(input, ',', 0) == "1"){
      if(stepsPerRevolutionStepperX <= 0){
        stepsPerRevolutionStepperX = stepsPerRevolutionStepperX * -1;
      }
      stepperXActive = true;
    }
    if(getValue(input, ',', 0) == "2"){
      if(stepsPerRevolutionStepperX >= 0){
        stepsPerRevolutionStepperX = stepsPerRevolutionStepperX * -1;
      }
      stepperXActive = true;
    }

    if(getValue(input, ',', 2) == "0"){
      stepperYActive = false;
    }
    if(getValue(input, ',', 2) == "1"){
      if(stepsPerRevolutionStepperY <= 0){
        stepsPerRevolutionStepperY = stepsPerRevolutionStepperY * -1;
      }
      stepperYActive = true;
    }
    if(getValue(input, ',', 2) == "2"){
      if(stepsPerRevolutionStepperY >= 0){
        stepsPerRevolutionStepperY = stepsPerRevolutionStepperY * -1;
      }
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

    
    //myStepperX.setSpeed(stepperXSpeed);
    //myStepperY.setSpeed(stepperYSpeed);
    
  }
  
  if(stepperXActive){
    myStepperX.step(stepsPerRevolutionStepperX);
  }
  if(stepperYActive){
    myStepperY.step(stepsPerRevolutionStepperY);
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
