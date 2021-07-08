/* Example sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board and Arduino UNO. More info: https://www.makerguides.com */
// Include the Arduino Stepper.h library:
#include <Stepper.h>
// Define number of steps per rotation:
int stepsPerRevolution = 4;
int motorSpeed = 5000;
// Wiring:
// Pin 8 to IN1 on the ULN2003 driver
// Pin 9 to IN2 on the ULN2003 driver
// Pin 10 to IN3 on the ULN2003 driver
// Pin 11 to IN4 on the ULN2003 driver
// Create stepper object called 'myStepper', note the pin order:
//Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
Stepper myStepper2 = Stepper(stepsPerRevolution, D3, D5, D4, D6);
void setup() {
  Serial.begin(115200);
  // Set the speed to 5 rpm:
  //myStepper.setSpeed(motorSpeed);
  myStepper2.setSpeed(motorSpeed);
  
  // Begin Serial communication at a baud rate of 9600:
  
}

unsigned long lastChange = 0;
String input = "1,3,1,0";
bool stepper2Active = false;
int stepperSpeed = 5000;

int testFakeString = 0;

void loop() {
  
  if(lastChange == 0){
    lastChange = millis();
  }

  if(millis() - lastChange >= 3000){
    Serial.println(input);
    
    if(getValue(input, ',', 0) == "0"){
      stepper2Active = false;
    }
    if(getValue(input, ',', 0) == "1"){
      if(stepsPerRevolution <= 0){
        stepsPerRevolution = stepsPerRevolution * -1;
      }
      stepper2Active = true;
    }
    if(getValue(input, ',', 0) == "2"){
      if(stepsPerRevolution >= 0){
        stepsPerRevolution = stepsPerRevolution * -1;
      }
      stepper2Active = true;
    }
    stepperSpeed = getValue(input, ',', 1).toInt() * 1000;
    
    lastChange = 0;

    testFakeString++;
    
    if(testFakeString == 0){
      input = "1,2,1,0";
    }
    if(testFakeString == 1){
      input = "2,5,1,0";
    }
    if(testFakeString == 2){
      input = "0,5,1,0";
      testFakeString = -1;
    }
    
    myStepper2.setSpeed(stepperSpeed);
    
  }
  
  // Step one revolution in one direction:
  //Serial.println("clockwise");
  if(stepper2Active){
    myStepper2.step(stepsPerRevolution);
  }
  
  //myStepper2.step(stepsPerRevolution);
  
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
