#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <stdio.h>
  
//#define SERVER_IP "10.0.1.1:9080" // PC address with emulation on host
#define SERVER_IP "*SERVER IP"

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

String mac = WiFi.macAddress();
String mac_status = "INACTIVE";
//digital pin interfacing
int d_pins[9]= {16,5,4,0,2,14,12,13,15};
int pin_count[9] = {0,0,0,0,0,0,0,0,0}; 
bool pin_rise[9] = {false,false,false,false,false,false,false,false,false}; 
 

unsigned long postTime = 0; 

void setup() {
  Serial.begin(115200);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  pinMode(15,INPUT); 

}

void loop() {
  //--------------------------Data Posting Logic-----------------------------
  if(millis() >= postTime)  { 
    if ((WiFi.status() == WL_CONNECTED)) {
      WiFiClient client;
      HTTPClient http;
      //Serial.print("[HTTP] begin...\n");
      //configure traged server and url
      http.begin(client, "http://" SERVER_IP "/update.php"); 
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
      //Serial.print("[HTTP] POST...\n");
      // start connection and send HTTP header and body
      String payload = "mac="+mac+"&pin0=" + pin_count[0]+ "&pin1="+ pin_count[1] +"&pin2="+ pin_count[2]+"&pin3="+ pin_count[3]+"&pin4="+ pin_count[4]+"&pin5="+ pin_count[5]+"&pin6="+ pin_count[6]+"&pin7="+ pin_count[7]+"&pin8="+ pin_count[8];
      //Serial.println(payload);
      int httpCode = http.POST(payload);
  
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      }   
       
      int timing = (millis()-postTime);
      Serial.println(timing);
      postTime = millis() + 10000;
      http.end();
      
    } else { 
      Serial.println("Wifi is disconnected");
    } 
    
  //-------------------Coin Counting Logic------------------------------------------
  } else { 
    for(int i =0; i<9;i++) { 
      if(digitalRead(d_pins[i])){ 
        if(!pin_rise[i]){ 
          //rising edge on pin
          pin_rise[i] = true;
          pin_count[i]++;
          Serial.print(i);
          Serial.print(":");
          Serial.println(pin_count[i]);
        }
      } else { 
        if(pin_rise[i]){ 
          //falling edge on pin
          pin_rise[i] = false;
        }
      }
    }
  }
}
