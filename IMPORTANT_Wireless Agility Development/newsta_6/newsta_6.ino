#include <WiFi.h>        // Wi-Fi networking library for BW16 (AmebaD Arduino core)
#include <WiFiUdp.h>     // UDP protocol library (for sending/receiving fast network messages)
#include <SPI.h>         // Serial Peripheral Interface library (for RFID)
#include <MFRC522.h>     // Library for MFRC522 RFID reader

// RFID pins for BW16
#define SDA_DIO 9        // SPI Slave Select (SDA/SS) pin for RFID
#define RESET_DIO 7      // Reset pin for RFID
MFRC522 RC522(SDA_DIO, RESET_DIO); // RFID reader object using those pins

#define ledRFID 8        // LED pin for RFID feedback
#define startButton 3    // Button pin to start sequence
#define buzzerPin 2      // Buzzer pin for audio feedback

WiFiUDP udp;             // UDP object for wireless network messages

// Static network configuration
IPAddress local_IP(192, 168, 0, 100);   // This device's (controller's) static IP
IPAddress gateway(192, 168, 0, 1);      // Router/gateway address
IPAddress subnet(255, 255, 255, 0);     // Subnet mask for local network
const int localUdpPort = 4210;          // UDP port for all wireless comm

// IP addresses for the pole devices (the endpoints in the system)
IPAddress poleIPs[5] = {
  IPAddress(192, 168, 0, 101),
  IPAddress(192, 168, 0, 102),
  IPAddress(192, 168, 0, 103),
  IPAddress(192, 168, 0, 104),
  IPAddress(192, 168, 0, 105)
};

int availablePole = 0;   // Counter for active poles
int maxPole = 5;         // Maximum poles supported

// Struct definition for network messages (status, commands)
struct struct_message {
  int id;      // Pole or sender ID
  int pass;    // Pass status (used for checking, timing, etc)
  int wrong;   // Number of wrong/penalty actions
};

struct_message boardsStruct[5]; // State of each pole
struct_message incoming;        // Incoming network message buffer

// Command codes for UDP messages (custom protocol)
int commandIdle = 0;
int commandCheck = 1;
int commandOn = 2;
int commandEnd = 3;
int commandDeb = 4;
int commandWait = 5;
int commandCount = 6;

int value = 0;
int path = 0;
int a = 0;
int b = 0;
int wrong = 0;

String uid = "";              // UID from RFID tag
bool gameStatus = false;
bool state_RFID = false;
unsigned long timeout;
bool debugMode = false;
bool debugRun = false;

unsigned long start_time;     // Game timing
unsigned long finish_time;
float final_time;
float speed;
float totalDist = 10.0;       // Total distance in meters

// WiFi credentials
char* ssid = "sportHub";         // Wi-Fi SSID (network name)
const char* password = "sportScience123"; // Wi-Fi password

// PHP server IP for data logging
const char* server_host = "192.168.0.199";

void setup() {
  SPI.begin();                 // Initialize SPI bus for RFID
  RC522.PCD_Init();            // Initialize RFID hardware
  Serial.begin(115200);        // Begin serial monitor for debug

  pinMode(ledRFID, OUTPUT);    // Set up LED pin
  pinMode(buzzerPin, OUTPUT);  // Set up buzzer pin
  pinMode(startButton, INPUT_PULLUP); // Set up start button (with pull-up resistor)

  WiFi.config(local_IP, gateway, subnet);   // Set up static IP configuration for Wi-Fi
  WiFi.begin(ssid, password);               // Connect to Wi-Fi using SSID and password
  while (WiFi.status() != WL_CONNECTED) {   // Wait until connected
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected. IP:");
  Serial.println(WiFi.localIP());           // Print the assigned IP address

  udp.begin(localUdpPort);                  // Start UDP on the defined port for wireless messaging
  delay(5000);                              // Delay for network stabilization (optional)
}

void loop() {
  while (!keyPress()) {                     // Wait until start button pressed or RFID detected
    listenForUpdates();                     // Keep listening for wireless updates from poles
  }
  while (digitalRead(startButton) == LOW) { // Wait until start button is released
    listenForUpdates();
  }

  poleCount();          // Send command to all poles to report in (wireless)
  delay(1000);

  buzzThreeTimes();     // Buzzer feedback before start
  poleIdle();           // Set all poles to idle (wireless)
  buzz(600);

  sendCommand(poleIPs[0], commandCheck);    // Ask first pole if it's ready (wireless)
  boardsStruct[0].pass = 0;
  while (boardsStruct[0].pass == 0) {      // Wait for pass confirmation from pole
    listenForUpdates();
  }

  Serial.println("Play!");
  start_time = millis();                   // Start timing
  value = boardsStruct[0].pass;
  path = value;
  gameStart(path);                         // Begin the pole sequence
  buzz(500);

  finish_time = millis();                  // End timing
  final_time = (finish_time - start_time) / 1000.0; // Calculate elapsed time (s)
  speed = totalDist / final_time;          // Calculate speed (m/s)

  Serial.println(String(uid) + "," + String(final_time) + "," + String(speed) + "," + String(wrong));

  sendToServer(uid, final_time, speed, wrong); // Send results to backend server (wireless HTTP)

  final_time = 0;
  speed = 0;
  wrong = 0;
  uid = "";
  state_RFID = false;
  digitalWrite(ledRFID, LOW);
  delay(5000);
  gameEnd();                               // End of game, send end command to poles (wireless)
}

// Sends data to PHP server over Wi-Fi HTTP (logs the run result)
void sendToServer(String uid, float final_time, float speed, int penalty) {
  WiFiClient client; // TCP client for HTTP communication

  if (!client.connect(server_host, 80)) { // Connect to server (port 80 = HTTP)
    Serial.println("Connection to server failed");
    return;
  }

  // Compose GET request with result data in the URL
  String url = "/database/AGILITY32.php";
  url += "?a=" + uid;
  url += "&b=" + String(final_time, 2);
  url += "&c=" + String(speed, 2);
  url += "&d=" + String(penalty);

  String request = "GET " + url + " HTTP/1.1\r\n";
  request += "Host: " + String(server_host) + "\r\n";
  request += "Connection: close\r\n\r\n";

  client.print(request); // Send request

  // Read server's reply (optional)
  while (client.connected()) {
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }

  client.stop(); // Close connection
}

// Receives and processes incoming UDP packets from pole devices
void listenForUpdates() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    IPAddress remoteIp = udp.remoteIP();
    int remotePort = udp.remotePort();
    udp.read((uint8_t*)&incoming, sizeof(incoming));
    Serial.print("Received from pole ID: ");
    Serial.print(incoming.id);
    Serial.print(" | From port: ");
    Serial.print(remotePort);
    Serial.print(" | pass: ");
    Serial.print(incoming.pass);
    Serial.print(" | wrong: ");
    Serial.print(incoming.wrong);
    Serial.print(" | actual sender IP: ");
    Serial.println(remoteIp);
    if (incoming.id >= 1 && incoming.id <= maxPole) {
      boardsStruct[incoming.id - 1] = incoming;
    }
  }
}


// Handles button press and RFID scan logic to trigger start
bool keyPress() {
  while (!state_RFID) {
    if (digitalRead(startButton) == LOW) { // Start button pressed
      Serial.println("Start button pressed...");
      timeout = millis();
      while (digitalRead(startButton) == LOW && !debugMode) {
        if ((millis() - timeout) >= 3000) { // Hold for 3s to enter debug mode
          debugMode = true;
          debugRun = false;
          Serial.println("Entering DEBUG mode.");
          buzz(500);
        }
      }
      while (digitalRead(startButton) == LOW) {} // Wait for button release
      while (debugMode) {
        debug(); // Enter debug function if in debug mode
      }
    }

    // RFID scan detected
    if (!state_RFID && RC522.PICC_IsNewCardPresent() && RC522.PICC_ReadCardSerial()) {
      uid = "";
      for (byte i = 0; i < RC522.uid.size; i++) {
        uid += String(RC522.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(RC522.uid.uidByte[i], HEX);
      }
      uid.toUpperCase();
      Serial.println("RFID detected: " + uid);
      RC522.PICC_HaltA();
    }

    // If UID read, set system to ready state
    if (uid.length() > 0) {
      state_RFID = true;
      poleWait();                 // Tell all poles to enter wait state (wireless)
      digitalWrite(ledRFID, HIGH);// Turn on RFID LED
      buzz(400);                  // Sound buzzer
    }

    delay(50); // Small delay to debounce
  }

  return digitalRead(startButton) == LOW;
}

// Debug mode: pole pairing, system diagnostics (using wireless commands)
void debug() {
  if (!debugRun) {
    availablePole = 0;
    // Sequentially send commandDeb to each pole and wait for its response
    for (int i = 0; i < maxPole; i++) {
      Serial.print("Sending debug command to pole ");
      Serial.print(i + 1);
      Serial.print(" at IP ");
      Serial.println(poleIPs[i]);

      sendCommand(poleIPs[i], commandDeb);

      // Wait for this pole to reply with pass == 1, up to 15 seconds
      unsigned long startWait = millis();
      bool responded = false;
      while (millis() - startWait < 10000) {
        listenForUpdates();
        if (boardsStruct[i].pass == 1) {
          responded = true;
          Serial.print("Pole ");
          Serial.print(i + 1);
          Serial.println(" completed debug/reset.");
          availablePole++;
          break;
        }
      }
      if (!responded) {
        Serial.print("Pole ");
        Serial.print(i + 1);
        Serial.println(" did not respond in time.");
        break;
        debugRun = true;
      }
      delay(1000); // Small gap before next pole
    }

    debugRun = true;
  }

  // Quitting debug mode if button is held
  if (digitalRead(startButton) == LOW) {
    timeout = millis();
    while (digitalRead(startButton) == LOW && debugMode) {
      if ((millis() - timeout) >= 3000) {
        debugMode = false;
        gameEnd();              // End game, notify all poles (wireless)
        buzz(500);
        Serial.println("\nQuitting DEBUG mode.");
      }
    }
    while (digitalRead(startButton) == LOW) {}
  }
}


// Main pole sequence logic, controls wireless commands and scoring
void gameStart(int path) {
  a = (path == 1) ? 2 : 1;
  b = (path == 1) ? 1 : 2;

  for (int i = 1; i < availablePole; i++) {
    sendCommand(poleIPs[i], commandCheck);     // Ask pole if ready (wireless)
    boardsStruct[i].pass = 0;
    while (boardsStruct[i].pass == 0) {
      listenForUpdates();                      // Wait for pole reply (wireless)
      delay(10);
    }
    value = boardsStruct[i].pass;

    bool isOdd = (i % 2 == 1);
    bool correct = (isOdd && value == a) || (!isOdd && value == b);

    if (!correct) {
      wrong++;
      while (boardsStruct[i].pass == value) {
        listenForUpdates();
        delay(10);
      }
    }

    sendCommand(poleIPs[i - 1], commandOn);    // Activate previous pole (wireless)
    if (i == availablePole - 1) {
      sendCommand(poleIPs[i], commandOn);      // Activate last pole if needed (wireless)
    }
    delay(100);
  }
}

// Sends a single command (int) to another device over UDP
void sendCommand(IPAddress ip, int command) {
  udp.beginPacket(ip, localUdpPort);           // Start UDP packet to target IP
  udp.write((uint8_t*)&command, sizeof(command)); // Write command as raw bytes
  udp.endPacket();                            // Send the UDP packet
}

// Buzzer utility functions
void buzz(int duration) {
  digitalWrite(buzzerPin, HIGH);
  delay(duration);
  digitalWrite(buzzerPin, LOW);
}
void buzzThreeTimes() {
  for (int i = 0; i < 3; i++) {
    buzz(250);
    delay(750);
  }
}

// Command utilities for poles (all use wireless/UDP)
// void polePairing() {
//   availablePole = 0;
//   for (int i = 0; i < maxPole; i++) {
//     sendCommand(poleIPs[i], commandDeb); // Debug/pair command
//   }
// }

void poleWait() {
  for (int i = 0; i < availablePole; i++) {
    sendCommand(poleIPs[i], commandWait); // Set pole to wait state
  }
}
void poleCount() {
  for (int i = 0; i < maxPole; i++) {
    sendCommand(poleIPs[i], commandCount); // Request pole to count/respond
  }
}
void poleIdle() {
  for (int i = 0; i < maxPole; i++) {
    sendCommand(poleIPs[i], commandIdle); // Set pole to idle
  }
}

// Sends end command to all poles, accumulates total wrongs from each pole (wireless)
void gameEnd() {
  delay(500);
  for (int i = 0; i < availablePole; i++) {
    sendCommand(poleIPs[i], commandEnd); // End command
    listenForUpdates();
    wrong += boardsStruct[i].wrong;      // Add up penalty count
    }
}
