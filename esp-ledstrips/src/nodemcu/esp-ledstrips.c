#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* WIFI_SSID = "SSID";
const char* WIFI_PASS = "PASS";

unsigned int r = 0;
unsigned int g = 0;
unsigned int b = 0;
unsigned int w = 0;
unsigned int state = 0;

#define LED_RED 13
#define LED_GREEN 15
#define LED_BLUE 12
#define LED_WHITE 14

ESP8266WebServer server(80);

void setup(){
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);

  analogWrite(LED_RED, 0);
  analogWrite(LED_GREEN, 0);
  analogWrite(LED_BLUE, 0);
  analogWrite(LED_WHITE, 0);

  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected (local IP ");
  Serial.print(WiFi.localIP());
  Serial.println(")");

  server.on("/", handleClient);
  server.begin();
  Serial.println("Server started");
}

void loop(){
  server.handleClient();
}

unsigned int getInRange(int input){
  if(input < 0)
    input = 0;

  return (input > 255)? 255 : input;
}

void handleClient(){
  Serial.print("Incoming connection from ");
  Serial.println(server.client().remoteIP());

  String response = "";
  if(server.args() == 0){
    response = "{ \"status\": \"FAIL\", \"response\": \"Usage: /?action=[on|off|set|get]&r=[red]&g=[green]&b=[blue]&w=[white]\" }";
  }
  else if(server.arg("action") == "on"){
    state = 1;

    analogWrite(LED_RED, r);
    analogWrite(LED_GREEN, g);
    analogWrite(LED_BLUE, b);
    analogWrite(LED_WHITE, w);

    response = "{ \"status\": \"OK\", \"response\": \"State has been changed to: 1\" }";
  }
  else if(server.arg("action") == "off"){
    state = 0;

    analogWrite(LED_RED, 0);
    analogWrite(LED_GREEN, 0);
    analogWrite(LED_BLUE, 0);
    analogWrite(LED_WHITE, 0);

    response = "{ \"status\": \"OK\", \"response\": \"State has been changed to: 0\" }";
  }
  else if(server.arg("action") == "set"){
    if(server.arg("r") != "") r = getInRange(server.arg("r").toInt());
    if(server.arg("g") != "") g = getInRange(server.arg("g").toInt());
    if(server.arg("b") != "") b = getInRange(server.arg("b").toInt());
    if(server.arg("w") != "") w = getInRange(server.arg("w").toInt());

    Serial.print("Setting color: rgbw(");
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.print(b);
    Serial.print(",");
    Serial.print(w);
    Serial.println(")");

    if((r+g+b+w) == 0)
      state = 0;
    else
      state = 1;

    analogWrite(LED_RED, r);
    analogWrite(LED_GREEN, g);
    analogWrite(LED_BLUE, b);
    analogWrite(LED_WHITE, w);

    response = "{ \"status\": \"OK\", \"response\": \"Color rgbw(";
    response += String(r) + ",";
    response += String(g) + ",";
    response += String(b) + ",";
    response += String(w);
    response += ") has been set\" }";
  }
  else if(server.arg("action") == "get"){
    response = "{ \"status\": \"OK\", \"response\": {";
    response += "\"state\": " + String(state) + ", ";
    response += "\"r\": " + String(r) + ", ";
    response += "\"g\": " + String(g) + ", ";
    response += "\"b\": " + String(b) + ", ";
    response += "\"w\": " + String(w) + " } }";
  }
  else{
    response = "{ \"status\": \"FAIL\", \"response\": \"Invalid request\" }";
  }

  server.send(200, "application/json", response);
}
