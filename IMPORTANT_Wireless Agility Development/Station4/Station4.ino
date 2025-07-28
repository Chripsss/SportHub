#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

#define SDA_DIO 5
#define RESET_DIO 4
MFRC522 RC522 (SDA_DIO, RESET_DIO);

#define ledRFID 2
#define startButton 21
#define buzzerPin 27

uint8_t broadcastAddress[3][6] = {
  {0xCC, 0xDB, 0xA7, 0x2D, 0xEF, 0xC0},
  {0xB4, 0xE6, 0x2D, 0xF6, 0xD1, 0x19},
  {0xA4, 0xCF, 0x12, 0x44, 0x03, 0x9C}
};

int availablePole = 0;
int maxPole = 5;

typedef struct struct_message {
  int id;
  int pass;
  int wrong;
} struct_message;

struct_message incomingState;

struct_message board1;
struct_message board2;
struct_message board3;

struct_message boardsStruct[3] = {board1,board2,board3};

esp_now_peer_info_t peerInfo;

int commandIdle = 0;
int commandCheck = 1;
int commandOn = 2;
int commandEnd = 3;
int commandDeb = 4;

int wrong = 0;
unsigned long start_time;
unsigned long finish_time;
float final_time;
float speed;
float totalDist = 10.0;

int value = 0;
int path = 0;
int a = 0;
int b = 0;
bool gameStatus = 0;

String uid = "";
bool state_RFID = 0;

unsigned long timeout;
bool debugMode;
bool debugRun;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&incomingState, incomingData, sizeof(incomingState));
  boardsStruct[incomingState.id-1].pass = incomingState.pass;
  boardsStruct[incomingState.id-1].wrong = incomingState.wrong;
}

void setup() {
  SPI.begin();
  RC522.PCD_Init();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(ledRFID, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(startButton, INPUT_PULLUP);

  if (esp_now_init() != ESP_OK) {
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  memcpy(peerInfo.peer_addr, broadcastAddress[0], 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress[1], 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress[2], 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  for (int i = 0; i < maxPole; i++) {
    boardsStruct[i].pass == 0;
  }

  delay(5000);
}

void loop() {
  while (keyPress() == 0) {
  }
  while (digitalRead(startButton) == 0) {
  }

  digitalWrite(buzzerPin, HIGH);
  delay(250);
  digitalWrite(buzzerPin, LOW);
  delay(750);

  digitalWrite(buzzerPin, HIGH);
  delay(250);
  digitalWrite(buzzerPin, LOW);
  delay(750);

  digitalWrite(buzzerPin, HIGH);
  delay(250);
  digitalWrite(buzzerPin, LOW);
  delay(750);

  digitalWrite(buzzerPin, HIGH);
  delay(600);
  digitalWrite(buzzerPin, LOW);

  esp_err_t result = esp_now_send(broadcastAddress[0], (uint8_t *) &commandCheck, sizeof(commandCheck));
  while (boardsStruct[0].pass == 0) {
    delay(10);
  }
  start_time = millis();
  value = boardsStruct[0].pass;
  if (value == 1) {
    path = 1;
  }
  else if (value == 2) {
    path = 2;
  }
  gameStart(path);

  digitalWrite(buzzerPin, HIGH);
  delay(500);
  digitalWrite(buzzerPin, LOW);

  finish_time = millis();
  final_time = (finish_time - start_time) / 1000.00;
  speed = totalDist / final_time;

  Serial.println(String(uid) + "," + String(final_time) + "," + String(speed) + "," + String(wrong));

  final_time = 0;
  speed = 0;
  wrong = 0;
  uid = "";
  state_RFID = 0;
  digitalWrite(ledRFID, LOW);
  delay(5000);
  gameEnd();
}

boolean keyPress() {
  while (state_RFID == 0) {  
    if (digitalRead(startButton) == 0) {
      timeout = millis();
      while (digitalRead(startButton) == 0 && debugMode == 0) {
        if ((millis() - timeout) >= 3000) {
          debugMode = 1;
          debugRun = 0;
          digitalWrite(buzzerPin, HIGH);
          delay(500);
          digitalWrite(buzzerPin, LOW);
        }
      }
      while (digitalRead(startButton) == 0){
      }
      while (debugMode == 1) {
        debug();
      }
    }

    if (state_RFID == 0 && RC522.PICC_IsNewCardPresent() && RC522.PICC_ReadCardSerial()) {
      for (byte i = 0; i < RC522.uid.size; i++) {
        uid += String(RC522.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(RC522.uid.uidByte[i], HEX);
      }
      uid.toUpperCase();
      RC522.PICC_HaltA();
    }

    if (uid.length() > 0) {
      state_RFID = 1;
      poleIdle();
      digitalWrite(ledRFID, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(400);
      digitalWrite(buzzerPin, LOW);
    }
  }

  boolean pressed = 0;
  if (digitalRead(startButton) == 0) {
    pressed = 1;
  }
  return pressed;
}

void debug() {
  while (debugRun == 0) {
    polePairing();
    debugRun = 1;
    delay(1000);

    for (int i = 0; i < maxPole; i++) {
      if (boardsStruct[i].pass == 1) {
        availablePole++;
      }
    }
  }

  if (digitalRead(startButton) == 0) {
    timeout = millis();
    while (digitalRead(startButton) == 0 && debugMode == 1) {
      if ((millis() - timeout) >= 3000) {
        debugMode = 0;
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
      }
    }
    while (digitalRead(startButton)==0) {
    }
    gameEnd();
  }
}

void gameStart(int path) {
  if (path == 1) {
    a = 2;
    b = 1;
  }
  else if (path == 2) {
    a = 1;
    b = 2;
  }
  
  for (int i = 1; i < availablePole; i++) {
    esp_err_t result1 = esp_now_send(broadcastAddress[i], (uint8_t *) &commandCheck, sizeof(commandCheck));
    while (boardsStruct[i].pass == 0) {
      delay(10);
    }
    value = boardsStruct[i].pass;

    if ((i%2) == 1) {
      if (value == a) {
        esp_err_t result2 = esp_now_send(broadcastAddress[i-1], (uint8_t *) &commandOn, sizeof(commandOn));
      }
      if (value == b) {
        wrong++;
        while (value == b) {
          value = boardsStruct[i].pass;
          delay(10);
        }
        esp_err_t result2 = esp_now_send(broadcastAddress[i-1], (uint8_t *) &commandOn, sizeof(commandOn));
      }
    }

    if ((i%2) == 0) {
      if (value == a) {
        wrong++;
        while (value == a) {
          value = boardsStruct[i].pass;
          delay(10);
        }
        esp_err_t result2 = esp_now_send(broadcastAddress[i-1], (uint8_t *) &commandOn, sizeof(commandOn));
        esp_err_t result3 = esp_now_send(broadcastAddress[i], (uint8_t *) &commandOn, sizeof(commandOn));
      }
      if (value == b) {
        esp_err_t result2 = esp_now_send(broadcastAddress[i-1], (uint8_t *) &commandOn, sizeof(commandOn));
        esp_err_t result3 = esp_now_send(broadcastAddress[i], (uint8_t *) &commandOn, sizeof(commandOn));
      }
    }
    delay(100);
  }
}

void polePairing() {
  availablePole = 0;
  for (int i = 0; i < maxPole; i++) {
    esp_err_t result = esp_now_send(broadcastAddress[i], (uint8_t *) &commandDeb, sizeof(commandDeb));
  }
}

void poleIdle() {
  for (int i = 0; i < availablePole; i++) {
    esp_err_t result = esp_now_send(broadcastAddress[i], (uint8_t *) &commandIdle, sizeof(commandIdle));
  }
}

void gameEnd() {
  delay(500);
  for (int i = 0; i < availablePole; i++) {
    esp_err_t result = esp_now_send(broadcastAddress[i], (uint8_t *) &commandEnd, sizeof(commandEnd));
  }
  for (int i = 0; i < availablePole; i++) {
    wrong = wrong + boardsStruct[i].wrong;
  }
}