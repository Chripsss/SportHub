#define sensor 23
#define led 22
#define led_count 24

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(led_count, led, NEO_GRB + NEO_KHZ800);

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
int buttonPress;

esp_now_peer_info_t peerInfo;

unsigned long lastPressTime;
const unsigned long tapThreshold = 500;

uint32_t color[] = {
  pixels.Color(0, 0, 255),
  pixels.Color(255, 0, 0),
  pixels.Color(0, 255, 0),
  pixels.Color(255, 255, 0)
};
int colorIndex = 0;
uint32_t currentColor = color[colorIndex];

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(sensor, INPUT_PULLUP);

  pixels.begin();
  pixels.setBrightness(80);

  if (esp_now_init() != ESP_OK) {
    return;
  }

  sendingState.id = 2; // Change according to button number
  sendingState.task = 0;
  sendingState.wrong = 0;
  sendingState.right = 0;
  incomingCommand = 5;
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
}

void loop() {

  if (incomingCommand == 0) {
    turnOff();
    sendingState.task = 0;
    sensorState = digitalRead(sensor);
    if (sensorState == 0 && prev_sensorState == 1) {
      sendingState.wrong++;
    }
    prev_sensorState = sensorState;
    delay(10);
  }

  if (incomingCommand == 1) {
    turnOn();
    while (incomingCommand == 1) {
      sensorState = digitalRead(sensor);
      if (sensorState == 0 && prev_sensorState == 1) {
        turnOff();
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
    turnOn();
    sendingState.task = 0;
    sendingState.wrong = 0;
    sendingState.right = 0;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == 3 && incomingCommand != oldCommand) {
    turnOn();
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == 4 && incomingCommand != oldCommand) {
    turnOn();
    sendingState.task = 1;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
    while (incomingCommand == 4) {
      sensorState = digitalRead(sensor);
      if (sensorState == 0) {
        lastPressTime = millis();
        while (millis() - lastPressTime < tapThreshold) {
          sensorState = digitalRead(sensor);
          if (sensorState == 0 && prev_sensorState == 1) {
            buttonPress++;
          }
          prev_sensorState = sensorState;
          if (buttonPress == 2) {
            colorChange();
            buttonPress = 0;
          }
          delay(10);
        }
        buttonPress = 0;
      }
    }
  }

  oldCommand = incomingCommand;
}

void colorChange() {
  colorIndex = (colorIndex + 1) % (sizeof(color) / sizeof(color[0]));
  currentColor = color[colorIndex];
  turnOn();
}

void turnOn() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, currentColor);
  }
  pixels.show();
}

void turnOff() {
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}