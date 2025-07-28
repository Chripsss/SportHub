#define sensor1 27
#define led 26
#define sensor2 25
#define led_count 10

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(led_count, led, NEO_GRB + NEO_KHZ800);

uint8_t broadcastAddress[] = {0xC4, 0xD8, 0xD5, 0x95, 0xD6, 0x5C}; // Mainboard

typedef struct struct_message {
  int id;
  int pass;
  int wrong;
} struct_message;

struct_message sendingState;

int incomingCommand;
int oldCommand;

int sensor1State;
int sensor2State;
int prev_sensor1State;
int prev_sensor2State;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(sensor1, INPUT_PULLUP);
  pinMode(sensor2, INPUT_PULLUP);

  if (esp_now_init() != ESP_OK) {
    return;
  }

  sendingState.id = 3; // Change according to button number
  sendingState.pass = 0;
  sendingState.wrong = 0;
  incomingCommand = 5;
  oldCommand = 0;

  turnBlue();

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
  if (incomingCommand == 0) {
    turnRed();
    sendingState.pass = 0;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == 1) {
    while (incomingCommand == 1) {
      sensor1State = digitalRead(sensor1);
      sensor2State = digitalRead(sensor2);
      if (sensor1State == 0 && prev_sensor1State == 1) {
        sendingState.pass = 1;
      }
      if (sensor2State == 0 && prev_sensor2State == 1) {
        sendingState.pass = 2;
      }
      prev_sensor1State = sensor1State;
      prev_sensor2State = sensor2State;
      delay(10);
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
    }
  }

  if (incomingCommand == 2) {
    turnGreen();
  }
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
