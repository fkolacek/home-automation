#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const char* WIFI_SSID = "SSID";
const char* WIFI_PASS = "PASS";

const int LED_PIN = 13;

IRsend irsend(4);

ESP8266WebServer server(80);

void setup(){
  irsend.begin();

  Serial.begin(115200);

  delay(10);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

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

void handleClient(){
  digitalWrite(LED_PIN, HIGH);

  Serial.print("Incoming connection from ");
  Serial.println(server.client().remoteIP());

  String response = "{ \"code\": \"OK\", \"response\": \"Code has been sent\" }";

  if(server.args() != 2){
    response = "{ \"code\": \"FAIL\", \"response\": \"Usage: /?vendor=[vendor]&code=[code]\" }";
  }
  else if(server.args() == 2 && server.arg("code") != "" && server.arg("vendor") != ""){
    String vendor = server.arg("vendor");
    String code = server.arg("code");

    const char *ccode = code.c_str();
    long dcode = strtol(ccode, NULL, 16);

    Serial.print("Sending code: 0x");
    Serial.print(code);
    Serial.print(" / ");
    Serial.print(dcode);
    Serial.print(", vendor ");
    Serial.println(vendor);

    if(vendor == "SAMSUNG"){
      irsend.sendSAMSUNG(dcode, 32);
    }
    else if(vendor == "NEC"){
      irsend.sendNEC(dcode, 32);
    }
    else if(vendor == "SONY"){
      irsend.sendSony(dcode, 32);
    }
    else if(vendor == "RC5"){
      irsend.sendRC5(dcode, 32);
    }
    else if(vendor == "RC6"){
      irsend.sendRC6(dcode, 32);
    }
    else{
      response = "{ \"code\": \"FAIL\", \"response\": \"Invalid vendor (supported are SAMSUNG, NEC, SONY, RC5 and RC6)\" }";
    }
  }
  else{
    response = "{ \"code\": \"FAIL\", \"response\": \"Invalid request\" }";
  }

  server.send(200, "application/json", response);

  digitalWrite(LED_PIN, LOW);
}

