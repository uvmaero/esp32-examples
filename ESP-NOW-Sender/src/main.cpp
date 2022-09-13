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
#define TIMER_0_INTERVAL                1000000     // 1 second in microseconds
#define BUTTON_PIN                      3           // button pin, GPIO 36, ADC1_CH0


// --- includes --- // 
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>


// --- global variables --- //
// Hardware ISR Timers
hw_timer_t* timer0 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// data struct
struct DataStruct
{
  int counterTimer0 = 0;
  int counterLoop = 0;
  bool buttonState = false;
}; 
DataStruct data;

// ESP-Now Connection
uint8_t deviceMacAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA1};
uint8_t targetMacAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};       // change this to the target address!
esp_now_peer_info reciverInfo;


// --- function headers --- //
void sendData();
void timer0ISR();


// --- setup --- // 
void setup()
{
  // initialize serial connection for the serial monitor & debugging
  Serial.begin(9600);

  // initialize button pin
  pinMode(BUTTON_PIN, INPUT);

  // initialize timer interrupts 0 - 3
  timer0 = timerBegin(0, TIMER_INTERRUPT_PRESCALER, true);
  timerAttachInterrupt(timer0, &timer0ISR, true);
  timerAlarmWrite(timer0, TIMER_0_INTERVAL, true);
  timerAlarmEnable(timer0);

  // --- initialize ESP-NOW ---//
  // turn on wifi access point 
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &deviceMacAddress[0]);
  Serial.printf("DEVICE MAC ADDRESS: %s\n", WiFi.macAddress());

  // init ESP-NOW service
  esp_err_t initResult = esp_now_init();
  Serial.printf("ESP-NOW INIT [ %s ]\n", initResult == ESP_OK ? "SUCCESS" : "FAILED");
  
  // get peer informtion about reciver
  memcpy(reciverInfo.peer_addr, targetMacAddress, sizeof(targetMacAddress));
  reciverInfo.channel = 0;
  reciverInfo.encrypt = false;

  // add receiver as a peer
  esp_err_t peerConnectionResult = esp_now_add_peer(&reciverInfo);
  Serial.printf("ESP-NOW PEER CONNECTION [ %s ]\n", peerConnectionResult == ESP_OK ? "SUCCESS" : "FAILED");
}


// --- loop --- // 
void loop()
{
  // increment loop counter
  data.counterLoop++;
  
  // read button state
  data.buttonState = digitalRead(BUTTON_PIN);

  // transmit updated message
  sendData();
}


/**
 * @brief function to encapsulate the message sending process
 * 
 */
void sendData()
{
  // send message
  esp_err_t result = esp_now_send(targetMacAddress, (uint8_t *) &data, sizeof(data));

  // print result
  Serial.printf("Message Send Status [ %s ]\n", result == ESP_OK ? "SUCCESS" : "FAILED");
}


/**
 * @brief Timer 0 ISR - increments timer 0 counter
 * 
 */
void timer0ISR() 
{
  // disable interrupts
  portENTER_CRITICAL_ISR(&timerMux);
  
  data.counterTimer0++;

  // re-enable interrupts
  portEXIT_CRITICAL_ISR(&timerMux);
}