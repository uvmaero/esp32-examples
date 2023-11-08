/**
 * @file main.cpp
 * @author dominic gasperini
 * @brief example file for using timers on the esp32-wroom on the espressif dev module
 * @version 1.0
 * @date 2023-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */


// --- defines --- // 
#define CLOCK_PRESCALER                 80          // this is based off to the clock speed (assuming 80 MHz)
#define TIMER_1_INTERVAL                500000      // 0.5 seconds in microseconds
#define TIMER_2_INTERVAL                1000000     // 1 second in microseconds
#define TIMER_3_INTERVAL                2000000     // 2 second in microseconds
#define TIMER_4_INTERVAL                5000000     // 5 seconds in microseconds


// --- includes --- // 
#include <Arduino.h>
#include <esp_timer.h>


// --- global variables --- //

// timer counters, to keep track of how many times an ISR has been called
int counter1 = 0;
int counter2 = 0;
int counter3 = 0;
int counter4 = 0;


// Hardware Timers
hw_timer_t *timer1 = NULL;
hw_timer_t *timer2 = NULL;
hw_timer_t *timer3 = NULL;
hw_timer_t *timer4 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


// --- function headers --- //
void callbackFunction1();
void callbackFunction2();
void callbackFunction3();
void callbackFunction4();


// --- setup --- // 
void setup()
{
  // startup delay
  delay(3000);

  // initialize serial connection for the serial monitor & debugging
  Serial.begin(9600);

  // --- initialize timer interrupts --- //

  // timer 1 - Sensor Update
  timer1 = timerBegin(0, CLOCK_PRESCALER, true);
  timerAttachInterrupt(timer1, &callbackFunction1, true);
  timerAlarmWrite(timer1, TIMER_1_INTERVAL, true);

  timer2 = timerBegin(1, CLOCK_PRESCALER, true);
  timerAttachInterrupt(timer2, &callbackFunction2, true);
  timerAlarmWrite(timer2, TIMER_2_INTERVAL, true);

  timer3 = timerBegin(2, CLOCK_PRESCALER, true);
  timerAttachInterrupt(timer3, &callbackFunction3, true);
  timerAlarmWrite(timer3, TIMER_3_INTERVAL, true);

  timer4 = timerBegin(3, CLOCK_PRESCALER, true);
  timerAttachInterrupt(timer4, &callbackFunction4, true);
  timerAlarmWrite(timer4, TIMER_4_INTERVAL, true);

  // start timers
  timerAlarmEnable(timer1);
  timerAlarmEnable(timer2);
  timerAlarmEnable(timer3);
  timerAlarmEnable(timer4);

  Serial.printf("TIMER 1 STATUS: %s\n", timerAlarmEnabled(timer1) ? "RUNNING" : "DISABLED");
  Serial.printf("TIMER 2 STATUS: %s\n", timerAlarmEnabled(timer2) ? "RUNNING" : "DISABLED");
  Serial.printf("TIMER 3 STATUS: %s\n", timerAlarmEnabled(timer3) ? "RUNNING" : "DISABLED");
  Serial.printf("TIMER 4 STATUS: %s\n", timerAlarmEnabled(timer4) ? "RUNNING" : "DISABLED");
}


// --- loop --- // 
void loop()
{
  // print timer interrupt counts 
  Serial.printf("timer 1 (0.5 sec): %d | timer 2 (1 sec): %d | timer 3 (2 sec): %d |  timer 4 (5 sec): %d\r", counter1, counter2, counter3, counter4);
}


/**
 * @brief Timer 1 ISR - increments timer 1 counter
 * 
 */
void callbackFunction1() 
{  
  portENTER_CRITICAL_ISR(&timerMux);

  counter1++;

  portEXIT_CRITICAL_ISR(&timerMux);
}


/**
 * @brief Timer 2 ISR - increments timer 2 counter
 * 
 */
void callbackFunction2() 
{  
  portENTER_CRITICAL_ISR(&timerMux);

  counter2++;

  portEXIT_CRITICAL_ISR(&timerMux);
}


/**
 * @brief Timer 3 ISR - increments timer 3 counter
 * 
 */
void callbackFunction3() 
{  
  portENTER_CRITICAL_ISR(&timerMux);

  counter3++;

  portEXIT_CRITICAL_ISR(&timerMux);
}


/**
 * @brief Timer 4 ISR - increments timer 4 counter
 * 
 */
void callbackFunction4() 
{  
  portENTER_CRITICAL_ISR(&timerMux);

  counter4++;

  portEXIT_CRITICAL_ISR(&timerMux);
}