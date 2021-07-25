#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

// WIFI Network Variables
char * ssid_ap = "nodeMCU";
char * password_ap = "password";
IPAddress ip(192,168,11,4);
IPAddress gateway(192,168,11,1);
IPAddress subnet(255,255,255,0);
ESP8266WebServer server;
String dataWhichIsSend = "";


int stepsPerRevolutionStepper = 2000; // Define number of steps per rotation:
int xDir = 1; // Stepper direction variable for x axis
int yDir = 1; // Stepper direction variable for y axis
int motorSpeed = 14; // Stepper motorspeed


Stepper myStepperX = Stepper(stepsPerRevolutionStepper, D1, D3, D2, D4); //Init of stepperX and pins
Stepper myStepperY = Stepper(stepsPerRevolutionStepper, D5, D7, D6, D8); //Init of stepperY and pins


Adafruit_NeoPixel pixels(10, D10, NEO_KHZ800 + NEO_GRB); //Init of led strip


unsigned long lastChange = 0; //LastChange variable for timer


String input = "0,0,0,0,0"; //input string which is changed by receiving data


int delayForUpdate = 200; //init of delay
bool stepperXActive = false; //sets x-stepper active variable to false, which is used in loop to activate/deactivate .step of stepper
bool stepperYActive = false; //sets y-stepper active variable to false, which is used in loop to activate/deactivate .step of stepper
int stepperXSpeed = 0; //init stepperXSpeed which will be calculated in loop
int stepperYSpeed = 0; //init stepperYSpeed which will be calculated in loop
bool unblockReceive = false; //variable to control if input string is set to received data
unsigned long unblockReceiveTimer = 0; //Timer how long data will be received and set to input string
int unblockDuration = 30000; //duration for unblockReceiveTimer

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

  // Sets the speed of stepperX and stepperY
  myStepperX.setSpeed(motorSpeed);
  myStepperY.setSpeed(motorSpeed);

  //Sets hall-sensor pin
  pinMode(D9, INPUT);

  //activates neopixel strip
  pixels.begin();
                                  
}

//Handles receiving data of sender
void handleUpdate() {
  if(unblockReceive){ //if unblockReceive is true, input string will be set to received data
    input = server.arg("value");
  }
  else{ //else all values of the string are set to 0 to deactivate the steppers
    input = "0,0,0,0,0";
  }
  
  //can be used to answer sender when receiving was successful
  //  server.send(200,"text/plain","Updated"); 
}

void loop() {  
  // WIFI LOOP
  server.handleClient();

  //reads the status of the hall-sensor
  if(digitalRead(D9) == LOW && !unblockReceive){ //if magnet on hall sensor and receiving data is currently blocked, receiving will be unblocked
    Serial.println("Unblocked");
    unblockReceive = true;
  }

  if(unblockReceive){ // if receiving is unblocked
    if(unblockReceiveTimer == 0){ // Timer will start if its not already running
      unblockReceiveTimer = millis(); // unblockReceiveTimer is set to milliseconds passed since the Arduino board began running the current program
    }
    if(millis() - unblockReceiveTimer >= unblockDuration){ // check if difference of millis() and start point is bigger then unblockDuration
      unblockReceive = false; // receiving will be blocked
      unblockReceiveTimer = 0; // resets timer so it can be activated in the next iteration of the loop
      Serial.println("Blocked");
    }
  }
  
  if(lastChange == 0){ // Timer will start if its not already running
    lastChange = millis(); // // lastChange is set to milliseconds passed since the Arduino board began running the current program
  }

  if(millis() - lastChange >= delayForUpdate){ // check if difference of millis() and start point is bigger then delayForUpdate
    Serial.println(input); // Current input string is printed for debugging
    
    //Checks which value is in the input string first position and controls direction and active state of stepper X
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

    //Checks which value is in the input string third position and controls direction and active state of stepper Y
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

    //Checks which value is in the input string second position and calculates stepper X speed
    if(getValue(input, ',', 1) != "0"){
      stepperXSpeed = getValue(input, ',', 1).toInt() * 1000;
    } else {
      stepperXSpeed = 1;
    }

    //Checks which value is in the input string fourth position and calculates stepper Y speed
    if(getValue(input, ',', 3) != "0"){
      stepperYSpeed = getValue(input, ',', 3).toInt() * 1000;
    } else {
      stepperYSpeed = 1;
    }
    
    lastChange = 0; // resets timer so it can be activated in the next iteration of the loop
    setEmotion(getValue(input, ',', 4).toInt()); // method call to set the color of the neopixel strip by reading the fifth value of the input string
    
    //deactived because stepper did not have enough speed and power for changing the stepper speed to different values
    //myStepperX.setSpeed(stepperXSpeed);
    //myStepperY.setSpeed(stepperYSpeed);
    
  }
    
    if(stepperXActive){ //controls active state of stepper X
      myStepperX.step(xDir); //actual step for stepper X, direction depending on xDir set
    }
    if(stepperYActive){ //controls active state of stepper Y
      myStepperY.step(yDir); //actual step for stepper Y, direction depending on yDir set
    }
}

//Method to read values from the input string, by using a seperator. Similar to string.split()
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

// method to set the neopixel strip color with colorNumber
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
        pixels.setPixelColor(i, 0, 0, 100); //wut
        pixels.show();
      }
      break;
    case 4:
      for (int i = 0; i <= pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 0, 100, 0); //wut
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
