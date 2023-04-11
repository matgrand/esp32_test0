#include <Arduino.h>
#include "ota.h"
#include "credentials.h"

//set led pin
const int ledPin = 2;
#define DELAY 50

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  setupOTA("ESP32-Test-OTA", ssid, pass);

  // Your setup code

  //set led pin as output
  pinMode(ledPin, OUTPUT);

}

void loop() {
  ArduinoOTA.handle(); // Handle OTA events, must be called often

  //blink led
  digitalWrite(ledPin, HIGH);
  delay(DELAY);
  digitalWrite(ledPin, LOW);
  delay(DELAY);

}