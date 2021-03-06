#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <stdio.h>
  
//#define SERVER_IP "10.0.1.1:9080" // PC address with emulation on host
#define SERVER_IP "cceserver.ddns.net"


#ifndef STASSID
#define STASSID "[ACCESS POINT]"
#define STAPSK  "[AP PASSWORD]"
#endif

String mac = WiFi.macAddress();
//digital pin interfacing
const int IN_PINS=5;
//int d_pins[]= {5};
//int pin_count[] = {0}; 
//bool pin_rise[] = {false}; 
int d_pins[IN_PINS]= {5,4,14,12,13};
int pin_count[IN_PINS] = {0,0,0,0,0}; 
bool pin_rise[IN_PINS] = {false,false,false,false}; 
 

unsigned long postTime = 0; 

void setup() {
  Serial.begin(115200);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  for(int i = 0; i <IN_PINS; i++) { 
    pinMode(d_pins[i],INPUT);
  }
  //Output Pins for Live Monitor
  //  pinMode(2,OUTPUT); 
  //  pinMode(0,OUTPUT); 
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
      String payload = "mac="+mac+"&usr=webuser&pass=webmaster&cu_pin=" + pin_count[0]+ "&zn_pin="+ pin_count[1] +"&wht_pin="+ pin_count[2]+"&rjct_pin="+ pin_count[3]+"&cycle_pin="+ pin_count[4];
      Serial.println(payload);
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
    for(int i=0; i<IN_PINS ;i++) { 
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
