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
#define TIMER_INTERRUPT_PRESCALER       80          // this is based off to the clock speed (assuming 80 MHz)
#define TIMER_0_INTERVAL                500000      // 0.5 seconds in microseconds
#define TIMER_1_INTERVAL                1000000     // 1 second in microseconds
#define TIMER_2_INTERVAL                2000000     // 2 second in microseconds
#define TIMER_3_INTERVAL                5000000     // 5 seconds in microseconds


// --- includes --- // 
#include <Arduino.h>
#include <esp_timer.h>


// --- global variables --- //

// timer counters, to keep track of how many times an ISR has been called
int counter1 = 0;
int counter2 = 0;
int counter3 = 0;
int counter4 = 0;


// --- function headers --- //
static void callbackFunction1(void* arg);
static void callbackFunction2(void* arg);
static void callbackFunction3(void* arg);
static void callbackFunction4(void* arg);


// --- setup --- // 
void setup()
{
  // initialize serial connection for the serial monitor & debugging
  Serial.begin(9600);

  // --- initialize timer interrupts --- //
  // timer 1 - sensors 
  const esp_timer_create_args_t timer1_args = {
    .callback = &callbackFunction1,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "timer 1"
  };
  esp_timer_handle_t timer1;
  ESP_ERROR_CHECK(esp_timer_create(&timer1_args, &timer1));

  // timer 2 - can write
  const esp_timer_create_args_t timer2_args = {
    .callback = callbackFunction2,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "timer 2"
  };
  esp_timer_handle_t timer2;
  ESP_ERROR_CHECK(esp_timer_create(&timer2_args, &timer2));

  // timer 3
  const esp_timer_create_args_t timer3_args = {
    .callback = callbackFunction3,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "timer 3"
  };
  esp_timer_handle_t timer3;
  ESP_ERROR_CHECK(esp_timer_create(&timer3_args, &timer3));

  // timer 4 - WCB update
  const esp_timer_create_args_t timer4_args = {
    .callback = callbackFunction4,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "timer 4"
  };
  esp_timer_handle_t timer4;
  ESP_ERROR_CHECK(esp_timer_create(&timer4_args, &timer4));

  // start timers
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer1, 500000));
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer2, 1000000));
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer3, 2000000));
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer4, 5000000));

  ESP_LOGD(TAG, "Timer 1 INIT: %s\n", esp_timer_is_active(timer1) ? "ACTIVE" : "FAILED");
  ESP_LOGD(TAG, "Timer 2 INIT: %s\n", esp_timer_is_active(timer2) ? "ACTIVE" : "FAILED");
  ESP_LOGD(TAG, "Timer 3 INIT: %s\n", esp_timer_is_active(timer3) ? "ACTIVE" : "FAILED");
  ESP_LOGD(TAG, "Timer 4 INIT: %s\n", esp_timer_is_active(timer4) ? "ACTIVE" : "FAILED");
}


// --- loop --- // 
void loop()
{
  // print timer interrupt counts 
  Serial.printf("timer 1 (0.5 sec): %d | timer 2 (1 sec): %d | timer 3 (2 sec): %d |  timer 4 (5 sec): %d\r", counter1, counter2, counter3, counter4);
}


/**
 * @brief Timer 1 ISR - increments timer 0 count
 * 
 */
static void callbackFunction1(void* arg) 
{  
  counter1++;
}


/**
 * @brief Timer 2 ISR - increments timer 1 count
 * 
 */
static void callbackFunction2(void* arg) 
{  
  counter2++;
}


/**
 * @brief Timer 3 ISR - increments timer 2 count
 * 
 */
static void callbackFunction3(void* arg) 
{  
  counter3++;
}


/**
 * @brief Timer 4 ISR - increments timer 3 count
 * 
 */
static void callbackFunction4(void* arg) 
{  
  counter4++;
}