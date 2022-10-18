/**
 * @file main.cpp
 * @author dominic gasperini
 * @brief example file for printing "Hello, World!" to the Serial Monitor
 * @version 0.1
 * @date 2022-09-21
 */


// --- defines --- // 


// --- includes --- // 
#include <Arduino.h>


// --- global variables --- //


// --- function headers --- //


// --- setup --- // 
void setup() {
  // initialize the serial monitor with a baud rate of 9600
  Serial.begin(9600);
}


// --- loop --- //
void loop() {
  // print a message to the Serial Monitor
  Serial.println("Hello World!");

  // wait 1 second (in milliseconds)
  delay(1000);
}