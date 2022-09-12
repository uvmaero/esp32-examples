/**
 * @file main.cpp
 * @author dominic gasperini
 * @brief example file for using timers on the esp32-wroom on the espressif dev module
 * @version 0.1
 * @date 2022-09-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */


// --- defines --- // 
#define TIMER_INTERRUPT_PRESCALER       80          // this is based off to the clock speed (assuming 80 MHz), gets us to microseconds
#define TIMER_0_INTERVAL                500000      // 0.5 seconds in microseconds
#define TIMER_1_INTERVAL                1000000     // 1 second in microseconds
#define TIMER_2_INTERVAL                2000000     // 2 second in microseconds
#define TIMER_3_INTERVAL                5000000     // 5 seconds in microseconds


// --- includes --- // 
#include <Arduino.h>


// --- global variables --- //
// Hardware ISR Timers
hw_timer_t* timer0 = NULL;
hw_timer_t* timer1 = NULL;
hw_timer_t* timer2 = NULL;
hw_timer_t* timer3 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int timer0Count = 0;
int timer1Count = 0;
int timer2Count = 0;
int timer3Count = 0;


// --- function headers --- //
void printMessage0();
void printMessage1();
void printMessage2();
void printMessage3();


// --- setup --- // 
void setup()
{
  // initialize serial
  Serial.begin(9600);

  // initialize timer interrupts
  timer0 = timerBegin(0, TIMER_INTERRUPT_PRESCALER, true);
  timerAttachInterrupt(timer0, &printMessage0, true);
  timerAlarmWrite(timer0, TIMER_0_INTERVAL, true);
  timerAlarmEnable(timer0);

  timer1 = timerBegin(1, TIMER_INTERRUPT_PRESCALER, true);
  timerAttachInterrupt(timer1, &printMessage1, true);
  timerAlarmWrite(timer1, TIMER_1_INTERVAL, true);
  timerAlarmEnable(timer1);

  timer2 = timerBegin(2, TIMER_INTERRUPT_PRESCALER, true);
  timerAttachInterrupt(timer2, &printMessage2, true);
  timerAlarmWrite(timer2, TIMER_2_INTERVAL, true);
  timerAlarmEnable(timer2);

  timer3 = timerBegin(3, TIMER_INTERRUPT_PRESCALER, true);
  timerAttachInterrupt(timer3, &printMessage3, true);
  timerAlarmWrite(timer3, TIMER_3_INTERVAL, true);
  timerAlarmEnable(timer3);
}


// --- loop --- // 
void loop()
{
  // print timer interrupt counts 
  Serial.printf("timer counts:\n 0: %d | 1: %d | 2: %d | 3: %d\r", timer0Count, timer1Count, timer2Count, timer3Count);
}


/**
 * @brief Timer 0 ISR - increments timer 0 count
 * 
 */
void printMessage0() 
{
  portENTER_CRITICAL_ISR(&timerMux);
  
  timer0Count++;

  portEXIT_CRITICAL_ISR(&timerMux);
}


/**
 * @brief Timer 1 ISR - increments timer 1 count
 * 
 */
void printMessage1() 
{
  portENTER_CRITICAL_ISR(&timerMux);
  
  timer1Count++;

  portEXIT_CRITICAL_ISR(&timerMux);
}


/**
 * @brief Timer 2 ISR - increments timer 2 count
 * 
 */
void printMessage2() 
{
  portENTER_CRITICAL_ISR(&timerMux);
  
  timer2Count++;

  portEXIT_CRITICAL_ISR(&timerMux);
}


/**
 * @brief Timer 3 ISR - increments timer 3 count
 * 
 */
void printMessage3() 
{
  portENTER_CRITICAL_ISR(&timerMux);
  
  timer3Count++;

  portEXIT_CRITICAL_ISR(&timerMux);
}