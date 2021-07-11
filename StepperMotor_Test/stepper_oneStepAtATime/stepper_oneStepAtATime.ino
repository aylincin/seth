/* Example sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board and Arduino UNO. More info: https://www.makerguides.com */
// Include the Arduino Stepper.h library:
#include <Stepper.h>
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

void setup() {
  Serial.begin(115200);
  // Set the speed to 5 rpm:
  myStepperX.setSpeed(motorSpeed);
  myStepperY.setSpeed(motorSpeed);
  
  // Begin Serial communication at a baud rate of 9600:
  
}

unsigned long lastChange = 0;
String input = "1,3,1,2";

int delayForUpdate = 3000;
bool stepperXActive = false;
int stepperXSpeed = 5000;
bool stepperYActive = false;
int stepperYSpeed = 5000;

int testFakeString = 0;

void loop() {
  
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
    
    stepperXSpeed = getValue(input, ',', 1).toInt() * 1000;
    stepperYSpeed = getValue(input, ',', 1).toInt() * 1000;
    
    lastChange = 0;

    testFakeString++;
    
    if(testFakeString == 0){
      input = "1,2,2,5";
    }
    if(testFakeString == 1){
      input = "2,5,1,2";
    }
    if(testFakeString == 2){
      input = "0,5,0,5";
      testFakeString = -1;
    }
    
    myStepperX.setSpeed(stepperXSpeed);
    myStepperY.setSpeed(stepperYSpeed);
    
  }
  
  // Step one revolution in one direction:
  //Serial.println("clockwise");
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
