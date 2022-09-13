/**
 * @file main.cpp
 * @author dominic gasperini
 * @brief example file for using I/O on the esp32-wroom on the espressif dev module
 * @version 0.1
 * @date 2022-09-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */


// --- defines --- // 
#define LED_1_PIN                 4           // physical pin the LED is connected to
#define LED_2_PIN                 6           // physical pin the LED is connected to 
#define LED_3_PIN                 7           // physical pin the LED is connected to 
#define BUTTON_PIN                5           // physical pin the push button is connected to 
#define POTENTIOMETER_PIN         3           // physical pin the potentiometer is connected to 


// --- includes --- // 
#include <Arduino.h>


// --- global variables --- //
bool pushButtonState = false;
int potentiometerPosition = 0;


// --- setup --- // 
void setup()
{
  // initialize serial connection for the serial monitor & debugging
  Serial.begin(9600);

  // initialize input pins
  pinMode(BUTTON_PIN, INPUT);
  pinMode(POTENTIOMETER_PIN, INPUT);

  // initalize output pins
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
}


// --- loop --- // 
void loop()
{
  // read push button state
  pushButtonState = digitalRead(BUTTON_PIN);

  // print push button state to serial monitor
  Serial.printf("push button state: %s\n", pushButtonState ? "pressed" : "not pressed");

  // read potentiometer position
  potentiometerPosition = analogRead(POTENTIOMETER_PIN);

  // light LEDs according to potentiometer position
  if (potentiometerPosition >= 900)
  {
    digitalWrite(LED_1_PIN, true);
    digitalWrite(LED_2_PIN, true);
    digitalWrite(LED_3_PIN, true);
  }

  else if (potentiometerPosition >= 500 && potentiometerPosition < 900)
  {
    digitalWrite(LED_1_PIN, true);
    digitalWrite(LED_2_PIN, true);
    digitalWrite(LED_3_PIN, false);
  }
  
  else if (potentiometerPosition >= 200 && potentiometerPosition < 500)
  {
    digitalWrite(LED_1_PIN, true);
    digitalWrite(LED_2_PIN, true);
    digitalWrite(LED_3_PIN, false);
  }

  else 
  {
    digitalWrite(LED_1_PIN, false);
    digitalWrite(LED_2_PIN, false);
    digitalWrite(LED_3_PIN, false);
  }
}
