#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>

// RFID pins for BW16
#define SDA_DIO 9
#define RESET_DIO 7
MFRC522 RC522(SDA_DIO, RESET_DIO);

#define ledRFID 8
#define startButton 3
#define buzzerPin 2

WiFiUDP udp;

IPAddress local_IP(192, 168, 0, 100);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
const int localUdpPort = 4210;

IPAddress poleIPs[5] = {
  IPAddress(192, 168, 0, 101),
  IPAddress(192, 168, 0, 102),
  IPAddress(192, 168, 0, 103),
  IPAddress(192, 168, 0, 104),
  IPAddress(192, 168, 0, 105)
};

int availablePole = 0;
int maxPole = 5;

struct struct_message {
  int id;
  int pass;
  int wrong;
};

struct_message boardsStruct[5];
struct_message incoming;

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

String uid = "";
bool gameStatus = false;
bool state_RFID = false;
unsigned long timeout;
bool debugMode = false;
bool debugRun = false;

unsigned long start_time;
unsigned long finish_time;
float final_time;
float speed;
float totalDist = 10.0;  // total distance in meters

// WiFi credentials
char* ssid = "sportHub";
const char* password = "sportScience123";

// PHP server IP
const char* server_host = "192.168.0.199";

void setup() {
  SPI.begin();
  RC522.PCD_Init();
  Serial.begin(115200);

  pinMode(ledRFID, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(startButton, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected. IP:");
  Serial.println(WiFi.localIP());

  udp.begin(localUdpPort);
  delay(5000);
}

void loop() {
  while (!keyPress()) {
    listenForUpdates();
  }
  while (digitalRead(startButton) == LOW) {
    listenForUpdates();
  }

  poleCount();
  delay(1000);

  buzzThreeTimes();
  poleIdle();
  buzz(600);

  sendCommand(poleIPs[0], commandCheck);
  while (boardsStruct[0].pass == 0) {
    listenForUpdates();
  }

  Serial.println("Play!");
  start_time = millis();
  value = boardsStruct[0].pass;
  path = value;
  gameStart(path);
  buzz(500);

  finish_time = millis();
  final_time = (finish_time - start_time) / 1000.0;
  speed = totalDist / final_time;

  Serial.println(String(uid) + "," + String(final_time) + "," + String(speed) + "," + String(wrong));

  // Send result to server
  sendToServer(uid, final_time, speed, wrong);

  // Reset
  final_time = 0;
  speed = 0;
  wrong = 0;
  uid = "";
  state_RFID = false;
  digitalWrite(ledRFID, LOW);
  delay(5000);
  gameEnd();
}

void sendToServer(String uid, float final_time, float speed, int penalty) {
  WiFiClient client;

  if (!client.connect(server_host, 80)) {
    Serial.println("Connection to server failed");
    return;
  }

  String url = "/database/AGILITY32.php";
  url += "?a=" + uid;
  url += "&b=" + String(final_time, 2);
  url += "&c=" + String(speed, 2);
  url += "&d=" + String(penalty);

  String request = "GET " + url + " HTTP/1.1\r\n";
  request += "Host: " + String(server_host) + "\r\n";
  request += "Connection: close\r\n\r\n";

  client.print(request);

  // Optional: print server response
  while (client.connected()) {
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }

  client.stop();
}

void listenForUpdates() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    udp.read((uint8_t*)&incoming, sizeof(incoming));
    if (incoming.id >= 1 && incoming.id <= maxPole) {
      boardsStruct[incoming.id - 1] = incoming;
    }
  }
}

bool keyPress() {
  while (!state_RFID) {
    if (digitalRead(startButton) == LOW) {
      Serial.println("Start button pressed...");
      timeout = millis();
      while (digitalRead(startButton) == LOW && !debugMode) {
        if ((millis() - timeout) >= 3000) {
          debugMode = true;
          debugRun = false;
          Serial.println("Entering DEBUG mode.");
          buzz(500);
        }
      }
      while (digitalRead(startButton) == LOW) {}
      while (debugMode) {
        debug();
      }
    }

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

    if (uid.length() > 0) {
      state_RFID = true;
      poleWait();
      digitalWrite(ledRFID, HIGH);
      buzz(400);
    }

    delay(50);
  }

  return digitalRead(startButton) == LOW;
}

void debug() {
  if (!debugRun) {
    polePairing();
    timeout = millis();
    while ((millis() - timeout) <= 1500) {
      listenForUpdates();
    }

    for (int i = 0; i < maxPole; i++) {
      if (boardsStruct[i].pass == 1) {
        availablePole++;
      }
    }

    Serial.println("Available pole: ");
    Serial.print(availablePole);
    debugRun = true;
  }

  if (digitalRead(startButton) == LOW) {
    timeout = millis();
    while (digitalRead(startButton) == LOW && debugMode) {
      if ((millis() - timeout) >= 3000) {
        debugMode = false;
        gameEnd();
        buzz(500);
        Serial.println("Quitting DEBUG mode.");
      }
    }
    while (digitalRead(startButton) == LOW) {}
  }
}

void gameStart(int path) {
  a = (path == 1) ? 2 : 1;
  b = (path == 1) ? 1 : 2;

  for (int i = 1; i < availablePole; i++) {
    sendCommand(poleIPs[i], commandCheck);
    while (boardsStruct[i].pass == 0) {
      listenForUpdates();
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

    sendCommand(poleIPs[i - 1], commandOn);
    if (i == availablePole - 1) {
      sendCommand(poleIPs[i], commandOn);
    }
    delay(100);
  }
}

void sendCommand(IPAddress ip, int command) {
  udp.beginPacket(ip, localUdpPort);
  udp.write((uint8_t*)&command, sizeof(command));
  udp.endPacket();
}

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

void polePairing() {
  availablePole = 0;
  for (int i = 0; i < maxPole; i++) {
    sendCommand(poleIPs[i], commandDeb);
  }
}

void poleWait() {
  for (int i = 0; i < availablePole; i++) {
    sendCommand(poleIPs[i], commandWait);
  }
}

void poleCount() {
  for (int i = 0; i < maxPole; i++) {
    sendCommand(poleIPs[i], commandCount);
  }
}

void poleIdle() {
  for (int i = 0; i < maxPole; i++) {
    sendCommand(poleIPs[i], commandIdle);
  }
}

void gameEnd() {
  delay(500);
  for (int i = 0; i < availablePole; i++) {
    sendCommand(poleIPs[i], commandEnd);
    listenForUpdates();
    wrong += boardsStruct[i].wrong;
  }
}