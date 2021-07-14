#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

char * ssid_ap = "nodeMCU";
char * password_ap = "password";
IPAddress ip(192,168,11,4);
IPAddress gateway(192,168,11,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server;

String dataWhichIsSend = "hallo";

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(ssid_ap,password_ap);

  Serial.begin(115200);
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleIndex);
  server.on("/update", handleUpdate);
  server.begin();

}

void loop() {
  server.handleClient();

}

void handleIndex() {
  server.send(200,"text/plain",String(dataWhichIsSend));
}

void handleUpdate() {
  dataWhichIsSend = server.arg("value");
  server.send(200,"text/plain","Updated");
}
