#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Address of your receiver 
//uint8_t broadcastAddress1[] = {0xA0, 0xB7, 0x65, 0xDC, 0x29, 0x84}; // Mainboard
uint8_t broadcastAddress1[] = {0xB4, 0xE6, 0x2D, 0xF6, 0xCB, 0xA5}; // Button1 

typedef struct struct_message {
  int id;
  int task;
  int wrong;
  int right;
} struct_message;

struct_message incomingState;

struct_message board1;

struct_message boardsStruct[1] = {board1};

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

float data[200] = { 0 };  //an array capable of holding 200 entries numbered 0 to 199
int dataIndex = 0;

unsigned long startGame;

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
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

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

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  buttonOff();

  gameStatus = 0;

  commandSent1 = 0;
  commandSent2 = 0;

  delay(1000);
}
 
void loop() {
  //while (keyPress == 0) {
  //}
  gameReset();
  delay(3000);
  Serial.println("Game Start!");
  buttonOff();
  delay(3000);
  // BUZZER
  
  startGame = millis();
  while (((millis() - startGame) / 1000) < 20) {
    playGame();
  }

  gameEnd();

  for (int count = 0; count < dataIndex; count++) {
    // Serial.println(data[count], 3);
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

  Serial.println(String(right) + "," + String(wrong) + "," + String(fastest, 3) + "," + String(slowest, 3) + "," + String(avg, 3) + "," + String(accuracy, 2));

  for (int count = 0; count < dataIndex; count++) {
    data[count] = 0;
  }
  dataIndex = 0;
  right = 0;
  wrong = 0;
  dummyFast = 10.00;
  dummySlow = 0.00;
  avg = 0;
  sum = 0;
  accuracy = 0;

  delay(5000);  
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
        Serial.println("send1");
        while (boardsStruct[0].task == 0) {
        }
      }
      if (boardsStruct[0].task == 1 && commandSent1 == 1 && commandSent2 == 0) {
        esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandOff, sizeof(commandOff));
        commandSent2 = 1;
        Serial.println("send2");
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
  }

  delay(1000);
}

void randomize() {
  randomButton = 1;
}

void buttonOff() {
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandOff, sizeof(commandOff));
}

void gameReset() {
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandReset, sizeof(commandReset));
}

void gameEnd() {
  gameStatus = 0;
  commandSent1 = 0;
  commandSent2 = 0;
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &commandEnd, sizeof(commandEnd));
  delay(1000);

  wrong = boardsStruct[0].wrong;
  right = boardsStruct[0].right;
}