/**
 * @file main.cpp
 * @author Dominic Gasperini - UVM '23
 * @brief CAN testing code
 * @version 1.0
 * @date 2023-02-21
 */

/*
===============================================================================================
                                    Includes 
===============================================================================================
*/
// standard includes 
#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include <esp_timer.h>


/*
===============================================================================================
                                    Definitions
===============================================================================================
*/

#define IMD_FAULT_PIN                     32
#define BMS_FAULT_PIN                     33
#define FAN_ENABLE_PIN                    25
#define BRAKE_LIGHT_ENABLE_PIN            26

#define GPIO_UPDATE_INTERVAL              1500000     // 1.5 seconds in microseconds
#define TASK_STACK_SIZE                   4096        // in bytes
#define MAIN_LOOP_DELAY                   1


/*
===============================================================================================
                                  Global Variables
===============================================================================================
*/

// structure to keep track of GPIO states
struct GPIOData
{
  bool imdFaultActive = false;
  bool bmsFaultActive = false;
  bool fanEnableActive = false;
  bool brakeLightEnableActive = false;

  int cycleCounter = 1;
};
GPIOData data;


/*
===============================================================================================
                                    Function Declarations 
===============================================================================================
*/

// callbacks
void GPIOCallback(void* args);

// tasks
void GPIOTask(void* pvParameters);


/*
===============================================================================================
                                            Setup 
===============================================================================================
*/

void setup() {
  // delay startup by 5 seconds
  vTaskDelay(5000);

  // start serial
  Serial.begin(9600);
  Serial.printf("\n\n|--- STARTING SETUP ---|\n\n");

  // ------------------------- initialize GPIO -------------------------------- //

  // setup outputs
  ESP_ERROR_CHECK(gpio_set_direction((gpio_num_t)IMD_FAULT_PIN, GPIO_MODE_OUTPUT));
  ESP_ERROR_CHECK(gpio_set_direction((gpio_num_t)BMS_FAULT_PIN, GPIO_MODE_OUTPUT));
  ESP_ERROR_CHECK(gpio_set_direction((gpio_num_t)FAN_ENABLE_PIN, GPIO_MODE_OUTPUT));
  ESP_ERROR_CHECK(gpio_set_direction((gpio_num_t)BRAKE_LIGHT_ENABLE_PIN, GPIO_MODE_OUTPUT));
  Serial.printf("GPIO INIT OUTPUTS [ SUCCESS ]\n");


  // ------------------------ initialize timers ------------------------------- //
  // GPIO Update
  const esp_timer_create_args_t timer1_args = {
    .callback = &GPIOCallback,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "GPIO Update Timer"
  };
  esp_timer_handle_t timer1;
  ESP_ERROR_CHECK(esp_timer_create(&timer1_args, &timer1));

  // start CAN timer
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer1, GPIO_UPDATE_INTERVAL));
  Serial.printf("GPIO TIMER INIT [ SUCCESS ]\n");

  // end setup
  Serial.printf("\n\n|--- END SETUP ---|\n\n");
}


/*
===============================================================================================
                                    Callback Functions
===============================================================================================
*/


/**
 * @brief callback function for creating a new GPIO Update task
 * 
 * @param args arguments to be passed to the task
 */
void GPIOCallback(void* args) {
  // init task parameters
  static uint8_t ucParameterToPass;
  TaskHandle_t xHandle = NULL;

  // queue GPIO task for execution
  xTaskCreate(GPIOTask, "GPIO-Update", TASK_STACK_SIZE, &ucParameterToPass, 10, &xHandle);
}


/*
===============================================================================================
                                FreeRTOS Task Functions
===============================================================================================
*/


/**
 * @brief updates gpio data and pins
 * 
 * @param arg - argument passed via function pointer
 */
void GPIOTask(void *arg)
{
  // flip data states based on the cycle counter
  switch (data.cycleCounter) {
    case 1:
      data.imdFaultActive = !data.imdFaultActive;
    break;

    case 2:
      data.bmsFaultActive = !data.bmsFaultActive;
    break;

    case 3:
      data.fanEnableActive = !data.fanEnableActive;
    break;

    case 4:
      data.brakeLightEnableActive = !data.brakeLightEnableActive;
    break;
    
    default:
      data.cycleCounter = 0;
    break;
  }

  // update gpio states
  gpio_set_level((gpio_num_t)IMD_FAULT_PIN, data.imdFaultActive);
  gpio_set_level((gpio_num_t)BMS_FAULT_PIN, data.bmsFaultActive);
  gpio_set_level((gpio_num_t)FAN_ENABLE_PIN, data.fanEnableActive);
  gpio_set_level((gpio_num_t)BRAKE_LIGHT_ENABLE_PIN, data.brakeLightEnableActive);
  
  // print update
  Serial.printf("cycle: %d | imd: %d | bms: %d | fan: %d | brake: %d\r", data.cycleCounter, data.imdFaultActive, data.bmsFaultActive, data.fanEnableActive, data.brakeLightEnableActive);

  // update cycle counter
  if (data.cycleCounter >= 4) {
    data.cycleCounter = 1;
  }
  else {
    data.cycleCounter++;
  }

  // end task
  vTaskDelete(NULL);
}


/*
===============================================================================================
                                    Main Loop
===============================================================================================
*/

void loop() {
  // prevent watchdog from getting upset
  vTaskDelay(MAIN_LOOP_DELAY);   
}