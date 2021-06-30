
                      
void setup() {
  pinMode(2, INPUT);                                 
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), detectMagnet, FALLING);
}


void loop() {

}


void detectMagnet() {
  Serial.println("Magnet detected");
}
