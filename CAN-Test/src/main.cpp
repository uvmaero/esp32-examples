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
#include "driver/can.h"


/*
===============================================================================================
                                    Definitions
===============================================================================================
*/

#define CAN_TX_PIN                        21
#define CAN_RX_PIN                        22
#define MSG_ID                            0x555       // 11 bit standard format ID
#define NUM_OF_MSGS                       10      

#define CAN_UPDATE_INTERVAL               500000      // 0.5 seconds in microseconds
#define TASK_STACK_SIZE                   4096        // in bytes
#define MAIN_LOOP_DELAY                   1


/*
===============================================================================================
                                  Global Variables
===============================================================================================
*/

// CAN Interface
can_general_config_t canConfig = CAN_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX_PIN, (gpio_num_t)CAN_RX_PIN, CAN_MODE_NO_ACK);   // set pins controller will use
can_timing_config_t canTimingConfig = CAN_TIMING_CONFIG_500KBITS();         // the timing of the CAN bus
can_filter_config_t canFilterConfig = CAN_FILTER_CONFIG_ACCEPT_ALL();       // filter so we only receive certain messages


/*
===============================================================================================
                                    Function Declarations 
===============================================================================================
*/

// callbacks
void CANCallback(void* args);

// tasks
void CANReadTask(void* pvParameters);
void CANWriteTask(void* pvParameters);


/*
===============================================================================================
                                            Setup 
===============================================================================================
*/

void setup() {
  // start serial
  Serial.begin(9600);

  // delay startup by 5 seconds
  vTaskDelay(5000);

  // --------------------- initialize CAN Controller -------------------------- //
  if (can_driver_install(&canConfig, &canTimingConfig, &canFilterConfig) == ESP_OK) {
    Serial.printf("CAN INIT [ SUCCESS ]\n");

    // start CAN interface
    if (can_start() == ESP_OK) {
      Serial.printf("CAN STARTED [ SUCCESS ]\n");

      // track all alerts
      if (can_reconfigure_alerts(CAN_ALERT_ALL, NULL) == ESP_OK) {
        Serial.printf("CAN ALERTS [ SUCCESS ]\n");
      } 
      else {
        Serial.printf("CAN ALERTS [ FAILED ]\n");
      }
    }
  }
  else {
    Serial.printf("CAN INIT [ FAILED ]\n");
  }

  // ------------------------ initialize timers ------------------------------- //
  // CAN Update
  const esp_timer_create_args_t timer1_args = {
    .callback = &CANCallback,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "CAN Read-Write Timer"
  };
  esp_timer_handle_t timer1;
  ESP_ERROR_CHECK(esp_timer_create(&timer1_args, &timer1));

  // start CAN timer
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer1, CAN_UPDATE_INTERVAL));
}


/*
===============================================================================================
                                    Callback Functions
===============================================================================================
*/


/**
 * @brief callback function for creating a new CAN Update task
 * 
 * @param args arguments to be passed to the task
 */
void CANCallback(void* args) {
  // init task parameters
  static uint8_t ucParameterToPass;
  TaskHandle_t xHandle = NULL;

  // queue read and write tasks
  xTaskCreate(CANWriteTask, "CAN-Update", configMINIMAL_STACK_SIZE, &ucParameterToPass, 5, &xHandle);
  xTaskCreate(CANReadTask, "CAN-Update", configMINIMAL_STACK_SIZE, &ucParameterToPass, 5, &xHandle);
}


/*
===============================================================================================
                                FreeRTOS Task Functions
===============================================================================================
*/


/**
 * @brief sends messages onto the can bus
 * 
 * @param arg - argument passed via function pointer
 */
void CANWriteTask(void *arg)
{
  // init can message to send
  can_message_t tx_msg = {
    .flags = CAN_MSG_FLAG_SELF,
    .identifier = MSG_ID, 
    .data_length_code = 1, 
  };

  // send messages
  for (int i = 0; i < NUM_OF_MSGS; i++) {
    // transmit messages using self reception request
    tx_msg.data[0] = i;
    ESP_ERROR_CHECK(can_transmit(&tx_msg, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  
  // end task
  vTaskDelete(NULL);
}


/**
 * @brief receives messages from the can bus
 * 
 * @param arg - argument passed via function pointer
 */
void CANReadTask(void *arg)
{
  // init
  can_message_t rx_message;

  // receive messages
  for (int i = 0; i < NUM_OF_MSGS; i++) {
    // receive message and print message data
    ESP_ERROR_CHECK(can_receive(&rx_message, portMAX_DELAY));
    Serial.printf("Msg received - Data = %d", rx_message.data[0]);
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
