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

//uint8_t broadcastAddress1[] = {0xA0, 0xB7, 0x65, 0xDC, 0x29, 0x84}; // Mainboard
uint8_t broadcastAddress1[] = {0xA0, 0xB7, 0x65, 0xDC, 0x0F, 0x20}; // Button1 
uint8_t broadcastAddress2[] = {0xB4, 0xE6, 0x2D, 0xF6, 0xCB, 0xA5}; // Button2
uint8_t broadcastAddress3[] = {0xA4, 0xCF, 0x12, 0x45, 0xBD, 0x94}; // Button3 

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

struct_message boardsStruct[3] = {board1,board2,board3};

bool commandSent1;
bool commandSent2;

String success;

esp_now_peer_info_t peerInfo;

bool gameStatus;

int randomButton;
int commandOff = 0;
int commandOn = 1;
int commandReset = 2;
int commandEnd = 3;
int commandDeb = 4;

int right;
int wrong;
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

String uid = "";
bool state_RFID = 0;

bool i;
bool debugMode;
int timeout;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  /*char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");*/
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&incomingState, incomingData, sizeof(incomingState));
  boardsStruct[incomingState.id-1].task = incomingState.task;
  boardsStruct[incomingState.id-1].wrong = incomingState.wrong;
  boardsStruct[incomingState.id-1].right = incomingState.right;
  /*char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);  
  Serial.printf("Board ID %u: %u bytes\n", incomingState.id, len);
  Serial.printf("Task status: %d \n", boardsStruct[incomingState.id-1].task);
  Serial.println();*/
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
  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  gameReset();

  gameStatus = 0;
  commandSent1 = 0;
  commandSent2 = 0;

  delay(1000);
}
 
void loop() {
  while (keyPress() == 0) {
  }
  gameReset();
  while (digitalRead(startButton) == 0) {
  }
  buttonOff();

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

  Serial.println(String(uid) + "," + String(right) + "," + String(wrong) + "," + String(fastest, 3) + "," + String(slowest, 3) + "," + String(avg, 3) + "," + String(accuracy, 2));

  for (int count = 0; count < dataIndex; count++) {
    data[count] = 0;
  }
  dataIndex = 0;
  right = 0;
  wrong = 0;
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
          i = 0;
          digitalWrite(buzzerPin, HIGH);
          delay(500);
          digitalWrite(buzzerPin, LOW);
        }
      }
      while (digitalRead(startButton) == 0){
      }
      while (debugMode == 1) {
        Debug();
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
  }

  if (gameStatus == 1) {
    if (randomButton == 1) {
      if (commandSent1 == 0) {
        esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandOn, sizeof(commandOn));
        commandSent1 = 1;
        while (boardsStruct[0].task == 0) {
        }
      }
      if (boardsStruct[0].task == 1 && commandSent1 == 1 && commandSent2 == 0) {
        esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandOff, sizeof(commandOff));
        commandSent2 = 1;
        realTime = (millis() - realTime) / 1000.00;
        data[dataIndex] = realTime;
        dataIndex++;
        while (boardsStruct[0].task == 1) {
        }
        commandSent1 = 0;
        commandSent2 = 0;
        gameStatus = 0;
      }
    }

    if (randomButton == 2) {
      if (commandSent1 == 0) {
        esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &commandOn, sizeof(commandOn));
        commandSent1 = 1;
        while (boardsStruct[1].task == 0) {
        }
      }
      if (boardsStruct[1].task == 1 && commandSent1 == 1 && commandSent2 == 0) {
        esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &commandOff, sizeof(commandOff));
        commandSent2 = 1;
        realTime = (millis() - realTime) / 1000.00;
        data[dataIndex] = realTime;
        dataIndex++;
        while (boardsStruct[1].task == 1) {
        }
        commandSent1 = 0;
        commandSent2 = 0;
        gameStatus = 0;
      }
    }
  
    if (randomButton == 3) {
      if (commandSent1 == 0) {
        esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &commandOn, sizeof(commandOn));
        commandSent1 = 1;
        while (boardsStruct[2].task == 0) {
        }
      }
      if (boardsStruct[2].task == 1 && commandSent1 == 1 && commandSent2 == 0) {
        esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &commandOff, sizeof(commandOff));
        commandSent2 = 1;
        realTime = (millis() - realTime) / 1000.00;
        data[dataIndex] = realTime;
        dataIndex++;
        while (boardsStruct[2].task == 1) {
        }
        commandSent1 = 0;
        commandSent2 = 0;
        gameStatus = 0;
      }
    }
  }
  delay(100);
}

void Debug() {
  //debug mode: when each button is pressed, the respective button's LED light up
  while (i == 0) {
    buttonDebug();
    i = 1;
  }

  if (digitalRead(startButton) == 0) { //to quit debug mode, hold startButton for 3 seconds
    timeout = millis();
    while (digitalRead(startButton) == 0 && debugMode == 1) {
      if ((millis() - timeout) >= 3000) {
        debugMode = 0;
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
        gameReset();
      }
    }
    while (digitalRead(startButton)==0) {
    }
  }
}

void randomize() {
  randomButton = random(1,4);
}

void buttonOff() {
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandOff, sizeof(commandOff));
  esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &commandOff, sizeof(commandOff));
  esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &commandOff, sizeof(commandOff));
}

void gameReset() {
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandReset, sizeof(commandReset));
  esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &commandReset, sizeof(commandReset));
  esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &commandReset, sizeof(commandReset));
}

void gameEnd() {
  gameStatus = 0;
  commandSent1 = 0;
  commandSent2 = 0;
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandEnd, sizeof(commandEnd));
  esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &commandEnd, sizeof(commandEnd));
  esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &commandEnd, sizeof(commandEnd));
  digitalWrite(buzzerPin, HIGH);
  delay(400);
  digitalWrite(buzzerPin, LOW);
  delay(1000);

  wrong = boardsStruct[0].wrong + boardsStruct[1].wrong + boardsStruct[2].wrong;
  right = boardsStruct[0].right + boardsStruct[1].right + boardsStruct[2].right;
}

void buttonDebug() {
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandDeb, sizeof(commandDeb));
  esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &commandDeb, sizeof(commandDeb));
  esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &commandDeb, sizeof(commandDeb));
}