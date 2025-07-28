#define sensor 23
#define led 22
#define led_count 24

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(led_count, led, NEO_RGB + NEO_KHZ800);

uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0xDC, 0x29, 0x84}; // Mainboard

String success;

typedef struct struct_messsage {
  int id;
  int task;
  int wrong;
  int right;
} struct_message;

struct_message sendingState;

int incomingCommand;
int oldCommand;
bool commandSent;

int sensorState;
int prev_sensorState;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  /*Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }*/
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
  /*Serial.print("Bytes received: ");
  Serial.println(len);*/
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(sensor, INPUT_PULLUP);

  pixels.begin();
  pixels.setBrightness(80);

  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }

  sendingState.id = 1;
  sendingState.task = 0;
  sendingState.wrong = 0;
  sendingState.right = 0;
  incomingCommand = 0;
  oldCommand = 0;

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    //Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {

  if (incomingCommand == 0) {
    turnOffPixels();
    sendingState.task = 0;
    sensorState = digitalRead(sensor);
    if (sensorState == 0 && prev_sensorState == 1) {
      sendingState.wrong++;
    }
    prev_sensorState = sensorState;
    delay(10);
  }

  /*if (incomingCommand == 1 && incomingCommand != oldCommand) {
    digitalWrite(led, HIGH);
    while (incomingCommand == 1 && sendingState.task == 0) {
      sensorState = digitalRead(sensor);
      if (sensorState == 0 && prev_sensorState == 1) {
        digitalWrite(led, LOW);
        sendingState.task = 1;
        sendingState.right++;
      }
      prev_sensorState = sensorState;
      delay(10);
      if (incomingCommand == 0) {
        sendingState.task = 0;
        break;
      }
    }
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
    while (incomingCommand == 1) {
    }
  }*/

  if (incomingCommand == 1) {
    turnOnPixels();
    while (incomingCommand == 1) {
      sensorState = digitalRead(sensor);
      if (sensorState == 0 && prev_sensorState == 1) {
        turnOffPixels();
        sendingState.task = 1;
        sendingState.right++;
      }
      prev_sensorState = sensorState;
      delay(10);
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
    
      if (incomingCommand == 0) {
        sendingState.task = 0;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
        break;
      }
    }
  }
  
  if (incomingCommand == 2 && incomingCommand != oldCommand) {
    turnOnPixels2();
    sendingState.task = 0;
    sendingState.wrong = 0;
    sendingState.right = 0;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == 3 && incomingCommand != oldCommand) {
    turnOnPixels2();
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == 4 & incomingCommand != oldCommand) {
    turnOffPixels();
    while (incomingCommand == 4) {
      if (digitalRead(sensor) == 0) {
        turnOnPixels();
      }
      if (digitalRead(sensor) == 1) {
        turnOffPixels();
      }
      if (incomingCommand != 4) {
        turnOnPixels2();
        break;
      }
    }
  }

  oldCommand = incomingCommand;
}

void turnOnPixels() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 171, 85)); // Set to red color
  }
  pixels.show();  // Update the strip with new colors
}

void turnOnPixels2() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255)); // Set to red color
  }
  pixels.show();  // Update the strip with new colors
}

void turnOffPixels() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Turn off each pixel
  }
  pixels.show();  // Update the strip to turn off the pixels
}