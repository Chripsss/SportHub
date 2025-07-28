#define sensor1 27
#define led 26
#define sensor2 25
#define led_count 55

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

Adafruit_NeoPixel pixels(led_count, led, NEO_GRB + NEO_KHZ800);

uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0xDC, 0x49, 0x04}; // Mainboard

typedef struct struct_message {
  int id;
  int pass;
  int wrong;
} struct_message;

struct_message sendingState;

int incomingCommand;
int oldCommand;

int commandIdle = 0;
int commandCheck = 1;
int commandOn = 2;
int commandEnd = 3;
int commandDeb = 4;
int commandWait = 5;
int commandCount = 6;

int sensor1State;
int sensor2State;
int prev_sensor1State;
int prev_sensor2State;
int wrong;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  mpu.begin();
  pinMode(sensor1, INPUT_PULLUP);
  pinMode(sensor2, INPUT_PULLUP);

  turnOff();

  if (esp_now_init() != ESP_OK) {
    return;
  }

  sendingState.id = 4; // Change according to pole number
  sendingState.pass = 0;
  sendingState.wrong = 0;
  incomingCommand = 7;
  oldCommand = 0;

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
  }

  if (incomingCommand == commandCheck) {
    while (incomingCommand == commandCheck) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      sensor1State = digitalRead(sensor1);
      sensor2State = digitalRead(sensor2);
      if (sensor1State == 0 && prev_sensor1State == 1) {
        sendingState.pass = 1;
      }
      if (sensor2State == 0 && prev_sensor2State == 1) {
        sendingState.pass = 2;
      }
      if (a.acceleration.z > 2.00 && wrong == 0) {
        sendingState.wrong++;
        wrong = 1;
      }
      prev_sensor1State = sensor1State;
      prev_sensor2State = sensor2State;
      delay(10);
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
    }
  }

  if (incomingCommand == commandOn) {
    turnGreen();
  }

  if (incomingCommand == commandEnd && incomingCommand != oldCommand) {
    turnOff();
    sendingState.pass = 0;
    sendingState.wrong = 0;
    wrong = 0;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == commandDeb && incomingCommand != oldCommand) {
    turnBlue();
    sendingState.pass = 1;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == commandWait) {
    turnBlue();
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