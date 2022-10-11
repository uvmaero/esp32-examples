/**
 * @file main.cpp
 * @author dominic gasperini
 * @brief example file for blinking an LED attached to the ESP32
 * @version 0.1
 * @date 2022-09-23
 */


// --- defines --- // 
#define LED_PIN     4          // this is the GPIO pin the LED is attached to


// --- includes --- // 
#include <Arduino.h>


// --- global variables --- //
bool ledState = false;          // setting the LED state to be off initially

// --- function headers --- //


// --- setup --- // 
void setup() {
  // initialize the serial monitor with a baud rate of 9600
  Serial.begin(9600);

  // initialize the LED as an output
  pinMode(LED_PIN, OUTPUT);
}


// --- loop --- //
void loop() {

  // toggle LED
  ledState = !ledState;     // this flips the LED state
  digitalWrite(LED_PIN, ledState);

  // print the led state to the serial monitor
  Serial.printf("LED State: %s\n", ledState ? "on" : "off");

  // delay
  delay(1000);      // wait 1 second (in milliseconds) before changing LED state
}