/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Adafruit_NeoPixel.h>

#include "Zmq_lib.h"

#define LED_PIN    D1
#define LED_COUNT  10
#define BRIGHTNESS 150

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

String WebsAdress = "ceniproductions.be";
#define PushPort 24041
#define subPort 24042

#define Delay10Sec 10000

u64 TimeSend = 0;

Zmq_lib push;
Zmq_lib sub;

void setup() {
    Serial.begin(115200);

    WiFiManager wifiManager;

    wifiManager.autoConnect("AutoConnectAP");

    Serial.println("Connected.");

    strip.begin();
    strip.show();
    strip.setBrightness(BRIGHTNESS);

    Serial.println("start connecting to zmq sockets");

    push.settings(WebsAdress, PushPort, push.push);
    sub.settings(WebsAdress, subPort, push.sub);

    push.connect();
    sub.connect();

    sub.subscribe("Service>PingPong>\0");
    Serial.println("setup done");
}

void loop() {
    if(millis() > TimeSend + Delay10Sec){
        push.sendMessage("Service>PingPong>ping\0");
        TimeSend = millis();
    }
    String ReturnString;
    sub.readMessage(ReturnString);
    if(!ReturnString.isEmpty()){
        Serial.println(ReturnString);
    }
}