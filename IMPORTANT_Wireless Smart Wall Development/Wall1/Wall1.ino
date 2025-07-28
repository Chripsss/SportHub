#define sensor 33
#define led 25
#define led_count 24

#include <esp_now.h>
#include <WiFi.h>908U7Y6
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(led_count, led, NEO_GRB + NEO_KHZ800);

uint8_t broadcastAddress[] = {0xC4, 0xD8, 0xD5, 0x95, 0xD6, 0x5C}; // Mainboard

typedef struct struct_messsage {
  int id;
  int task;
  int wrong;
  int right;
} struct_message;

struct_message sendingState;

int commandIdle = 0;
int commandCheck = 1;
int commandEnd = 2;
int commandWait = 3;
int commandDeb = 4;
int commandCount = 5;

int incomingCommand;
int oldCommand;
bool commandSent;

int param;
int samples = 4;
int offset = -50;
int buttonPress;

unsigned long timer;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pixels.begin();
  pixels.setBrightness(80);

  if (esp_now_init() != ESP_OK) {
    return;
  }

  sendingState.id = 4; // Change according to wall number
  sendingState.task = 0;
  sendingState.wrong = 0;
  sendingState.right = 0;
  incomingCommand = 6;
  oldCommand = 0;

  turnOff();

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  delay(1000);
}

void loop() {
  if (incomingCommand == commandIdle) {
    turnRed();
    sendingState.task = 0;
    if (analogRead(sensor) > 2550) {
      if (millis() - timer > 1000) {
        sendingState.wrong++;
        timer = millis();
      }
    }
    delay(10);
  }

  if (incomingCommand == commandCheck) {
    turnGreen();
    while (incomingCommand == commandCheck) {
      if (analogRead(sensor) > 2550) {
        if (millis() - timer > 1000) {
          turnRed();
          sendingState.task = 1;
          sendingState.right++;
          timer = millis();
        }
      }
      delay(10);
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
    
      if (incomingCommand == commandIdle) {
        sendingState.task = 0;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
      }
    }
  }
  
  if (incomingCommand == commandEnd && incomingCommand != oldCommand) {
    turnOff();
    sendingState.task = 0;
    sendingState.wrong = 0;
    sendingState.right = 0;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == commandWait && incomingCommand != oldCommand) {
    turnBlue();
  }

  if (incomingCommand == commandDeb && incomingCommand != oldCommand) {
    turnBlue();
    Serial.println("DEBUG");
    sendingState.task = 1;
    for (int i = 0; i < samples; i++) {
      param += analogRead(sensor);
    }
    param /= samples;
    param = param + offset;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == commandCount && incomingCommand != oldCommand) {
    for (int j = 0; j < 4; j++) { 
      for (int i = 0; i < led_count; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 255));
        pixels.show();
        delay(950/led_count);
      }
      turnOff();
    }
  }

  oldCommand = incomingCommand;
}

void turnOff() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

void turnBlue() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));
  }
  pixels.show();
}

void turnRed() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
  pixels.show();
}

void turnGreen() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
  }
  pixels.show();
}