#include <ESP8266WiFi.h>

const char* ssid = "nodeMCU";
const char* password = "password";
const char* host = "192.168.11.4";

WiFiClient client;

void setup() {
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());

  if(client.connect(host, 80)){
    String url = "/update?value=test";
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: keep-alive\r\n\r\n");
  }
  delay(10);
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
