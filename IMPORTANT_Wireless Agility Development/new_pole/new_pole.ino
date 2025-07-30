#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "WS2812B.h"

#define sensor1 2
#define sensor2 3
#define led_count 55

// Adjust these as needed for your Ameba board
#define LED_PIN SPI_MOSI

WS2812B led(LED_PIN, led_count);

Adafruit_MPU6050 mpu;
WiFiUDP udp;
const int localUdpPort = 4210;

IPAddress local_IP(192, 168, 0, 103);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress stationIP(192, 168, 0, 100);

int sensor1State, sensor2State, prev_sensor1State, prev_sensor2State;
int incomingCommand = 7;
int oldCommand = 7;
bool alreadyPassed = false;
int wrong = 0;

struct struct_message {
  int id;
  int pass;
  int wrong;
};

struct_message sendingState;

int commandIdle = 0;
int commandCheck = 1;
int commandOn = 2;
int commandEnd = 3;
int commandDeb = 4;
int commandWait = 5;
int commandCount = 6;

void setup() {
  Serial.begin(115200);

  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin("sportHub", "sportScience123");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  udp.begin(localUdpPort);
  mpu.begin();

  pinMode(sensor1, INPUT_PULLUP);
  pinMode(sensor2, INPUT_PULLUP);

  led.begin();
  turnOff();

  sendingState.id = 3;
  sendingState.pass = 0;
  sendingState.wrong = 0;
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    udp.read((char*)&incomingCommand, sizeof(incomingCommand));
    Serial.print("Received command: ");
    Serial.println(incomingCommand);
  }

  if (incomingCommand == commandIdle) {
    turnRed();
  }

  if (incomingCommand == commandCheck) {
    while (incomingCommand == commandCheck) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      sensor1State = digitalRead(sensor1);
      sensor2State = digitalRead(sensor2);

      if (sensor1State == LOW && prev_sensor1State == HIGH) {
        sendingState.pass = 1;
      }

      if (sensor2State == LOW && prev_sensor2State == HIGH) {
        sendingState.pass = 2;
      }

      if (a.acceleration.z > 2.00 && wrong == 0) {
        sendingState.wrong++;
        wrong = 1;
      }

      prev_sensor1State = sensor1State;
      prev_sensor2State = sensor2State;

      sendState();
      delay(10);

      int packetSize = udp.parsePacket();
      if (packetSize) {
        udp.read((char*)&incomingCommand, sizeof(incomingCommand));
        Serial.print("Changed command to: ");
        Serial.println(incomingCommand);
      }
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
    sendState();
  }

  if (incomingCommand == commandDeb && incomingCommand != oldCommand) {
    turnBlue();
    sendingState.pass = 1;
    sendState();
  }

  if (incomingCommand == commandWait) {
    turnBlue();
  }

  if (incomingCommand == commandCount && incomingCommand != oldCommand) {
    for (int j = 0; j < 4; j++) {
      for (int i = 0; i < led_count; i++) {
        led.setPixelColor(i, 0, 0, 255);
        led.show();
        delay(950 / led_count);
      }
      turnOff();
    }
  }

  oldCommand = incomingCommand;
}

void sendState() {
  udp.beginPacket(stationIP, localUdpPort);
  udp.write((uint8_t*)&sendingState, sizeof(sendingState));
  udp.endPacket();
}

// LED Helper Functions using WS2812B library
void turnOff() {
  led.fill(0, 0, 0, 0, led_count);
  led.show();
}

void turnRed() {
  led.fill(255, 0, 0, 0, led_count);
  led.show();
}

void turnGreen() {
  led.fill(0, 255, 0, 0, led_count);
  led.show();
}

void turnBlue() {
  led.fill(0, 0, 255, 0, led_count);
  led.show();
}