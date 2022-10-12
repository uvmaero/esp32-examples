/**
 * @file main.cpp
 * @author dominic gasperini
 * @brief example file for using ESP-NOW, with this device acting as a broadcaster and reciever
 * @version 0.1
 * @date 2022-13-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */


// --- defines --- // 
#define TIMER_INTERRUPT_PRESCALER       80          // this is based off to the clock speed (assuming 80 MHz)
#define TIMER_0_INTERVAL                1000000     // 1 second in microseconds


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
} data; 

// ESP-Now Connection C0:49:EF:46:23:B8
uint8_t targetMacAddress[] = {0xC4, 0xDE, 0xE2, 0xC0, 0x75, 0x80};       // change this to the mac address of your target device


// --- function headers --- //
void sendBroadcast();
void onDataSent(const uint8_t* macAddress, esp_now_send_status_t status);
void onDataReceived(const uint8_t* macAddress, const uint8_t* data, int dataLength);
void formatMacAddress(const uint8_t *macAddress, char *buffer, int maxLength);


// --- setup --- // 
void setup()
{
  // initialize serial connection for the serial monitor & debugging
  Serial.begin(9600);

  // initialize timer 0
  timer0 = timerBegin(0, TIMER_INTERRUPT_PRESCALER, true);
  timerAttachInterrupt(timer0, &sendBroadcast, true);
  timerAlarmWrite(timer0, TIMER_0_INTERVAL, true);
  timerAlarmEnable(timer0);

  // --- initialize ESP-NOW ---//
  // turn on wifi access point 
  WiFi.mode(WIFI_STA);
  Serial.printf("DEVICE MAC ADDRESS: %s\n", WiFi.macAddress());

  // init ESP-NOW service
  esp_err_t initResult = esp_now_init();
  Serial.printf("ESP-NOW INIT [ %s ]\n", initResult == ESP_OK ? "SUCCESS" : "FAILED");

  // setup ESP-NOW connections
  esp_now_register_recv_cb(onDataReceived);
}


// --- loop --- // 
void loop()
{
  // increment loop counter
  data.counterLoop += 2;
}


/**
 * @brief ISR function to encapsulate the message broadcast
 * 
 */
void sendBroadcast()
{
  // disable interrupts
  portENTER_CRITICAL_ISR(&timerMux);

  // get peer information
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, targetMacAddress, 6);
  if (!esp_now_is_peer_exist(targetMacAddress))
  {
    esp_now_add_peer(&peerInfo);
  }

  // send broadcast
  esp_err_t broadcastResult = esp_now_send(targetMacAddress, (const uint8_t*) &data, sizeof(data));

  // re-enable interrupts
  portEXIT_CRITICAL_ISR(&timerMux);
}


/**
 * @brief callback function for when a message is received from a broadcast
 * 
 * @param macAddress the mac address of the incoming message
 * @param data the content of the message
 * @param dataLength the size of the incoming data
 */
void onDataReceived(const uint8_t* macAddress, const uint8_t* incomingData, int dataLength)
{
  // disable interrupts
  portENTER_CRITICAL_ISR(&timerMux);

  // copy incoming data into the local data structure
  memcpy(&data, incomingData, sizeof(incomingData));

  // print the address of the incoming message
  char macStr[18];
  formatMacAddress(macAddress, macStr, 18);
  Serial.printf("Received message from: %s\n", macStr);

  // re-enable interrupts
  portEXIT_CRITICAL_ISR(&timerMux);
}


/**
 * @brief Formats MAC Address
 * 
 * @param macAddress the mac address to be converted
 * @param buffer the string buffer
 * @param maxLength the max length the output can be
 */
void formatMacAddress(const uint8_t *macAddress, char *buffer, int maxLength)
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
}