#include <Arduino.h>
const int ledPin = 15;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.print("readyy");
  digitalWrite(ledPin, 0);
  delay(2000);
  Serial.print("open");
  digitalWrite(ledPin, 1);
  delay(2000);
}