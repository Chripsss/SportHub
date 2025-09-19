#include <WiFi.h>
#include <SPI.h>
#include <WiFiUdp.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "WS2812B.h"


#define sensor1 2
#define sensor2 3
#define led_count 55

#define LED_PIN SPI_MOSI



WS2812B led(LED_PIN, led_count);

Adafruit_MPU6050 mpu;
WiFiUDP udp;
const int localUdpPort = 4210;

// Static IP config
IPAddress local_IP(192, 168, 0, 105);  // <-- Set to static IP (change for each pole)
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress stationIP(192, 168, 0, 100);  // <-- Station IP

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

int status = WL_IDLE_STATUS;

// ---- DHT periodic read timer (non-blocking) ----
unsigned long lastDhtMs = 0;           // DHT <<<
const unsigned long DHT_PERIOD = 2000; // 2 seconds between reads

void setup() {
  Serial.begin(115200);

  // Set static IP before WiFi.begin
  WiFi.config(local_IP, gateway, subnet);
 

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin("sportHub", "sportScience123");
    delay(500);
    Serial.print(".");
  }


  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  udp.begin(localUdpPort);
  mpu.begin();

  pinMode(sensor1, INPUT_PULLUP);
  pinMode(sensor2, INPUT_PULLUP);

  led.begin();
  
  if (wifi.status() = WL_CONNECTED) {
    turngreen();
  }
  
  delay(100);
  turnOff();

  // // --- DHT init ---
  // dht.begin();                                  // DHT <<<
  // Serial.println("DHT11 initialized on PA13");  // DHT <<<

  sendingState.id = 5;  // Change this for each pole (e.g., 2, 3, 4, ...)
  sendingState.pass = 0;
  sendingState.wrong = 0;
}

void loop() {

  // // ---- DHT periodic read (non-blocking) ----
  // if (millis() - lastDhtMs >= DHT_PERIOD) {     // DHT <<<
  //   lastDhtMs = millis();
  //   float tempC = dht.readTemperature();        // Celsius
  //   float hum   = dht.readHumidity();

  //   if (isnan(tempC) || isnan(hum)) {
  //     Serial.println("[DHT] Read failed");
  //   } else {
  //     Serial.print("[DHT] Temp: ");
  //     Serial.print(tempC);
  //     Serial.print(" °C  |  Hum: ");
  //     Serial.print(hum);
  //     Serial.println(" %");
  //   }
  // }
  // // ------------------------------------------

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

  // ----------- DEBUG/RECONNECT BLOCK -----------
  if (incomingCommand == commandDeb && incomingCommand != oldCommand) {
    turnBlue();
    sendingState.pass = 0;  // Not ready yet

    // Disconnect from WiFi
    WiFi.disconnect();
    delay(1000);

    // Reconfigure static IP and reconnect
    WiFi.config(local_IP, gateway, subnet);
    WiFi.begin("sportHub", "sportScience123");
    Serial.println("Reconnecting to WiFi...");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Reconnected! My IP: ");
      Serial.println(WiFi.localIP());

      // Print MAC address (AmebaD style)
      uint8_t mac[6];
      WiFi.macAddress(mac);
      Serial.print("My MAC: ");
      for (int i = 0; i < 6; i++) {
        if (mac[i] < 16) Serial.print("0");
        Serial.print(mac[i], HEX);
        if (i < 5) Serial.print(":");
      }
      Serial.println();

      // *** MOST IMPORTANT: Re-initialize UDP socket ***
      udp.begin(localUdpPort);

      // Now notify the station we are back online!
      sendingState.pass = 1; // now debug complete
      sendState();
      Serial.println("Sent debug complete after reconnect.");
    } else {
      Serial.println("WiFi reconnection failed!");
    }
  }
  // ---------------------------------------------

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