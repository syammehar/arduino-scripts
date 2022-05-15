#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include ".\conf.h"
/*
#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif
*/

#define RELAY 0 // relay connected to  GPIO0

const char *ssid = STASSID;
const char *password = STAPSK;

unsigned long myTime = 0;

ESP8266WebServer server(80);

void setup(void) {
  //Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Serial.println("");

  pinMode(RELAY,OUTPUT);
  digitalWrite(RELAY,HIGH);
  
  // Wait for connection
  byte retries = 0;
 while (WiFi.status() != WL_CONNECTED && retries < 30) {
      delay(500);
      retries++;
 }
  if (WiFi.status() == WL_CONNECTED) {
    
  //Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
  
  if (MDNS.begin("esp8266")) {
    //Serial.println("MDNS responder started");
  }

  server.on("/updateStatus", []() {
    String resp="Request Successfull";
    if(server.args()>0){
      int timer=0;
      if(server.arg("timer")!=""){
        timer=server.arg("timer").toInt();
        resp+="\nTimer value:"+(String)timer;
        myTime = millis()+timer;
        //Serial.println("Timer Started for "+(String)timer+ " ms, Currnet ms: "+(String)millis()+ " Expires at: "+(String)myTime);
      }
      if(timer==0){
        myTime=0;
      }
      int pin=0;
      if(server.arg("pin")!=""){
        pin=server.arg("pin").toInt();
         resp+="\nPin value:"+(String)pin;
      }
      int pinStatus=0;
      if(server.arg("status")!=""){
        pinStatus=server.arg("status").toInt();
        pinStatus = pinStatus == 0 || pinStatus == 1 ? pinStatus : 0;
         resp+="\nStatus value:"+(String)pinStatus;
      }
      if(pinStatus==1){
         digitalWrite(RELAY,LOW);
      }
      else{
        digitalWrite(RELAY,HIGH);
      }
    }
    server.send(200, "text/plain", resp);
  });
  
  server.on("/", []() {
    String resp="Server Working...";
    server.send(200, "text/plain", resp);
  });

  server.begin();
  //Serial.println("HTTP server started");
  }

}

void loop(void) {
  if (WiFi.status() == WL_CONNECTED) {
  server.handleClient();
}
if(myTime>0 && millis()>=myTime){
        digitalWrite(RELAY,HIGH);
        //Serial.println("Timer Expired. Currnet ms: "+(String)millis()+ " Expires to be at: "+(String)myTime);
        myTime=0;
}
}
