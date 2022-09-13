/**
 * @file main.cpp
 * @author dominic gasperini
 * @brief example code for using ESP-NOW, with this device acting as the reciver of a message
 * @version 0.1
 * @date 2022-09-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */


// --- defines --- // 


// --- includes --- // 
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>


// --- global variables --- //
int messageCounter = 0;
int messageLength = 0;

// data struct
struct DataStruct
{
  int counterTimer0 = 0;
  int counterLoop = 0;
  bool buttonState = false;
}; 
DataStruct data;

// ESP-Now Connection
uint8_t deviceMacAddress[] = {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};


// --- function headers --- //
void onDataArrived(const uint8_t * mac, const uint8_t *incomingData, int len);


// --- setup --- // 
void setup()
{
  // initialize serial connection for the serial monitor & debugging
  Serial.begin(9600);
  Serial.println("Serial Initalized!");

  // --- initialize ESP-NOW ---//
  // turn on wifi access point 
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &deviceMacAddress[0]);
  Serial.printf("DEVICE MAC ADDRESS: %s\n", WiFi.macAddress());

  // init ESP-NOW service
  esp_err_t initResult = esp_now_init();
  Serial.printf("ESP-NOW INIT [ %s ]\n", initResult == ESP_OK ? "SUCCESS" : "FAILED");
  
  // setup device for recieving messages
  esp_now_register_recv_cb(onDataArrived);
}


// --- loop --- // 
void loop()
{
  // print updated data
  Serial.printf("DEVICE MAC ADDRESS: ");
  Serial.println(WiFi.macAddress());
  // Serial.printf("messages received: %d\r", messageCounter);
  // Serial.printf("message size: %d\r", messageLength);
  // Serial.printf("Button State: %s\r", data.buttonState ? "pressed" : "not pressed");
  // Serial.printf("Loop Counter: %d\r", data.counterLoop);
  // Serial.printf("Timer Counter: %d", data.counterTimer0);
}


/**
 * @brief 
 * 
 * @param mac 
 * @param incomingData 
 * @param len 
 */
void onDataArrived(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  // copy the data to the local data structure
  memcpy(&data, incomingData, sizeof(data));

  // update message trackers
  messageCounter++;
  messageLength = len;
}
