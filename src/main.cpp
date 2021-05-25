#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "FS.h"
 
ESP8266WebServer server(80);

byte states[] = {0, 0, 0, 0};
int debounceTimes[] = {0, 0, 0, 0};

int debounceTime = 500;


void toggleState(){
  Serial.println(server.argName(0));
  if (server.argName(0) == "light"){
    if(states[0] == 0){
      states[0] = 1;
      digitalWrite(D1, LOW);
    }
    else{
      states[0] = 0;
      digitalWrite(D1, HIGH);
    }
  }
  else if (server.argName(0) == "reverse"){
    if(states[1] == 0){
      states[1] = 1;
      digitalWrite(D2, LOW);
    }
    else{
      states[1] = 0;
      digitalWrite(D2, HIGH);
    }
  }
  else if (server.argName(0) == "kompressor"){
    if(states[2] == 0){
      states[2] = 1;
      digitalWrite(D3, LOW);
    }
    else{
      states[2] = 0;
      digitalWrite(D3, HIGH);
    }
  }
  else if (server.argName(0) == "lowspeed"){
    if(states[3] == 0){
      states[3] = 1;
      digitalWrite(D4, LOW);
    }
    else{
      states[3] = 0;
      digitalWrite(D4, HIGH);
    }
  }
  server.send(200, "text/plain", "OK");
}

void getStates(){
  server.send(200, "text/plain", (String) states[0] + ";" + (String) states[1] + ";" + (String) states[2] + ";" + (String) states[3]);
}

void setup() {

  //D5 & D8 NC

  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  // alle relais ausschalten
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);

  Serial.begin(115200);

  // SPIFFS init

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS error");
    while(true){
      delay(500);
    }
  }

  // Access point starten
  WiFi.softAP("SOFA", "mywifipass");

  // Webserver starten

  server.on("/toggle", toggleState);
  server.on("/states", getStates);

  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/index.html", SPIFFS, "/index.html");
  server.serveStatic("/stylesheet.css", SPIFFS, "/stylesheet.css");
  server.serveStatic("/toggleswitch.css", SPIFFS, "/toggleswitch.css");
  server.serveStatic("/fancybutton.css", SPIFFS, "/fancybutton.css");

  server.serveStatic("/control.svg", SPIFFS, "/control.svg");
  server.serveStatic("/couch.svg", SPIFFS, "/couch.svg");
  server.serveStatic("/home.svg", SPIFFS, "/home.svg");
  server.serveStatic("/light.svg", SPIFFS, "/light.svg");

  server.begin();
}

void checkHWButtons(){
  if(digitalRead(D5) == LOW){
    Serial.println("D5 LOW");
    if(debounceTimes[0] + debounceTime < millis()){
      debounceTimes[0] = millis();
      if(states[0] == 0){
        states[0] = 1;
        digitalWrite(D1, LOW);
      }
      else{
        states[0] = 0;
        digitalWrite(D1, HIGH);
      }
    }
  }
  else if(digitalRead(D6) == HIGH){
    Serial.println("D6 HIGH");
    if(debounceTimes[1] + debounceTime < millis()){
      debounceTimes[1] = millis();
      if(states[1] == 0){
        states[1] = 1;
        digitalWrite(D2, LOW);
      }
      else{
        states[1] = 0;
        digitalWrite(D2, HIGH);
      }
    }
  }
  else if(digitalRead(D7) == HIGH){
    Serial.println("D7 HIGH");
    if(debounceTimes[2] + debounceTime < millis()){
      debounceTimes[2] = millis();
      if(states[2] == 0){
        states[2] = 1;
        digitalWrite(D3, LOW);
      }
      else{
        states[2] = 0;
        digitalWrite(D3, HIGH);
      }
    }
  }
  else if(analogRead(A0) < 1000){
    Serial.println("A0 HIGH");
    if(debounceTimes[3] + debounceTime < millis()){
      debounceTimes[3] = millis();
      if(states[3] == 0){
        states[3] = 1;
        digitalWrite(D4, LOW);
      }
      else{
        states[3] = 0;
        digitalWrite(D4, HIGH);
      }
    }
  }
}

void loop() {  
  checkHWButtons();
  server.handleClient();
  delay(20);
}

