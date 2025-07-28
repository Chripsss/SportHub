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

uint8_t broadcastAddress[4][6] = {
  {0xB4, 0xE6, 0x2D, 0xF6, 0xD1, 0x19},
  {0xF4, 0x65, 0x0B, 0x52, 0x13, 0x04},
  {0xF4, 0x65, 0x0B, 0x56, 0x0B, 0xDC},
  {0xF4, 0x65, 0x0B, 0xE8, 0x33, 0xBC}
};

typedef struct struct_message {
  int id;
  int task;
  int wrong;
  int right;
} struct_message;

struct_message incomingState;

struct_message board1;
struct_message board2;
struct_message board3;
struct_message board4;

struct_message boardsStruct[4] = {board1,board2,board3,board4};

bool commandSent1;
bool commandSent2;

String success;

esp_now_peer_info_t peerInfo;

bool gameStatus;

int randomWall;
int commandIdle = 0;
int commandCheck = 1;
int commandEnd = 2;
int commandWait = 3;
int commandDeb = 4;
int commandCount = 5;

int right;
int wrong;
int totalPoint;
int long ranDelay = 0;
int sensorValue = 0;
float realTime;
float fastest;
float slowest;
float dummyFast = 10.00;
float dummySlow = 0.001;
float fastValue;
float slowValue;
float avg;
float sum = 0;
float accuracy;

float data[200] = { 0 };
int dataIndex = 0;

unsigned long startGame;
unsigned long timeout;

String uid = "";
bool state_RFID = 0;

bool debugMode;
bool sent;
int debugRun;

int arraySize;
int randomArray;
int prevRandom;
int maxWall = 4;
int availableWall[4];

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&incomingState, incomingData, sizeof(incomingState));
  boardsStruct[incomingState.id-1].task = incomingState.task;
  boardsStruct[incomingState.id-1].wrong = incomingState.wrong;
  boardsStruct[incomingState.id-1].right = incomingState.right;
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

  memcpy(peerInfo.peer_addr, broadcastAddress[3], 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  gameStatus = 0;
  commandSent1 = 0;
  commandSent2 = 0;

  delay(1000);
}
 
void loop() {
  while (keyPress() == 0) {
  }
  while (digitalRead(startButton) == 0) {
  }
  countdown();
  delay(1000);

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

  wait();
  digitalWrite(buzzerPin, HIGH);
  delay(600);
  digitalWrite(buzzerPin, LOW);
  
  startGame = millis();
  while (((millis() - startGame) / 1000) < 20) {
    playGame();
  }

  gameEnd();

  for (int count = 0; count < dataIndex; count++) {
    fastValue = min(data[count], dummyFast);
    dummyFast = fastValue;
    slowValue = max(data[count], dummySlow);
    dummySlow = slowValue;
    sum = sum + data[count];
  }

  avg = sum / right;
  fastest = fastValue;
  slowest = slowValue;
  accuracy = ((right * 1.0) / (right * 1.0 + wrong * 1.0)) * 100.00;
  totalPoint = right + wrong;

  Serial.println(String(uid) + "," + String(right) + "," + String(fastest, 3) + "," + String(slowest, 3) + "," + String(avg, 3) + "," + String(accuracy, 2));
  for (int count = 0; count < dataIndex; count++) {
    data[count] = 0;
  }
  dataIndex = 0;
  right = 0;
  wrong = 0;
  totalPoint = 0;
  state_RFID = 0;
  dummyFast = 10.00;
  dummySlow = 0.00;
  avg = 0;
  sum = 0;
  accuracy = 0;
  digitalWrite(ledRFID, LOW);
  uid = "";
  delay(5000);  
}

boolean keyPress() {
  while (state_RFID == 0) {  
    if (digitalRead(startButton) == 0) { //to enter debug mode, hold startButton for 3 seconds
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
        //RC522.uid.size
        uid += String(RC522.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(RC522.uid.uidByte[i], HEX);  // Convert the UID to a single String
      }
      uid.toUpperCase();   // Convert to uppercase
      RC522.PICC_HaltA();  // Stop reading
      //RC522.PICC_HaltA();       // Halt PICC operation
      //RC522.PCD_StopCrypto1();  // Stop encryption on PCD
    }

    if (uid.length() > 0) {
      state_RFID = 1;
      wait();
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

void playGame() {
  if (gameStatus == 0) {
    randomize();
    gameStatus = 1;
    realTime = millis();
    idle();
  }

  if (gameStatus == 1) {
    if (commandSent1 == 0) {
      esp_err_t result = esp_now_send(broadcastAddress[randomWall], (uint8_t *) &commandCheck, sizeof(commandCheck));
      commandSent1 = 1;
      while (boardsStruct[randomWall].task == 0) {
      }
    }
    if (boardsStruct[randomWall].task == 1 && commandSent1 == 1 && commandSent2 == 0) {
      esp_err_t result = esp_now_send(broadcastAddress[randomWall], (uint8_t *) &commandIdle, sizeof(commandIdle));
      commandSent2 = 1;
      realTime = (millis() - realTime) / 1000.00;
      data[dataIndex] = realTime;
      dataIndex++;
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      while (boardsStruct[randomWall].task == 1) {
      }
      commandSent1 = 0;
      commandSent2 = 0;
      gameStatus = 0;
    }
  }
  delay(100);
}

void debug() {
  //debug mode: when each button is pressed, the respective button's LED light up
  while (debugRun == 0) {
    pairing();
    debugRun = 1;
    delay(1000);

    for (int i = 0; i < maxWall; i++) {
      if (boardsStruct[i].task == 1) {
        availableWall[arraySize] = i;
        arraySize++;
      }
    }
  }

  if (digitalRead(startButton) == 0) { //to quit debug mode, hold startButton for 3 seconds
    timeout = millis();
    while (digitalRead(startButton) == 0 && debugMode == 1) {
      if ((millis() - timeout) >= 3000) {
        debugMode = 0;
        gameReset();
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
      }
    }
    while (digitalRead(startButton)==0) {
    }
  }
}

void randomize() {
  randomArray = random(0,arraySize);
  while (randomArray == prevRandom) {
    randomArray = random(0,arraySize);
  }
  randomWall = availableWall[randomArray];
  prevRandom = randomArray;
}

void idle() {
  for (int i = 0; i < arraySize; i++) {
    int j = availableWall[i];
    esp_err_t result = esp_now_send(broadcastAddress[j], (uint8_t *) &commandIdle, sizeof(commandIdle));
  }
}

void wait() {
  for (int i = 0; i < arraySize; i++) {
    int j = availableWall[i];
    esp_err_t result = esp_now_send(broadcastAddress[j], (uint8_t *) &commandWait, sizeof(commandWait));
  }
}

void countdown() {
  for (int i = 0; i < arraySize; i++) {
    int j = availableWall[i];
    esp_err_t result = esp_now_send(broadcastAddress[j], (uint8_t *) &commandCount, sizeof(commandCount));
  }
}

void gameReset() {
  for (int i = 0; i < arraySize; i++) {
    int j = availableWall[i];
    esp_err_t result = esp_now_send(broadcastAddress[j], (uint8_t *) &commandEnd, sizeof(commandEnd));
  }
}

void gameEnd() {
  gameStatus = 0;
  commandSent1 = 0;
  commandSent2 = 0;
  for (int i = 0; i < arraySize; i++) {
    int j = availableWall[i];
    esp_err_t result = esp_now_send(broadcastAddress[j], (uint8_t *) &commandEnd, sizeof(commandEnd));
  }
  for (int i = 0; i < arraySize; i++) {
    int j = availableWall[i];
    wrong = wrong + boardsStruct[j].wrong;
    right = right + boardsStruct[j].right;
  }
  digitalWrite(buzzerPin, HIGH);
  delay(400);
  digitalWrite(buzzerPin, LOW);
  delay(1000);
}

void pairing() {
  arraySize = 0;
  for (int i = 0; i < maxWall; i++) {
    esp_err_t result = esp_now_send(broadcastAddress[i], (uint8_t *) &commandDeb, sizeof(commandDeb));
  }
}