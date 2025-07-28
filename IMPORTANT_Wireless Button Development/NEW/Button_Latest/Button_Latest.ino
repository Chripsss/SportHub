#define sensor 23
#define led 22

#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0xDC, 0x29, 0x84}; // Mainboard

String success;

typedef struct struct_messsage {
  int id;
  int task;
  int wrong;
  int right;
} struct_message;

struct_message sendingState;

int incomingCommand;
int oldCommand;
bool commandSent;

int sensorState;
int prev_sensorState;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  /*Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }*/
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
  /*Serial.print("Bytes received: ");
  Serial.println(len);*/
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(sensor, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }

  sendingState.id = 3;
  sendingState.task = 0;
  sendingState.wrong = 0;
  sendingState.right = 0;
  incomingCommand = 0;
  oldCommand = 0;

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    //Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {

  if (incomingCommand == 0) {
    digitalWrite(led, LOW);
    sendingState.task = 0;
    sensorState = digitalRead(sensor);
    if (sensorState == 0 && prev_sensorState == 1) {
      sendingState.wrong++;
    }
    prev_sensorState = sensorState;
    delay(10);
  }

  /*if (incomingCommand == 1 && incomingCommand != oldCommand) {
    digitalWrite(led, HIGH);
    while (incomingCommand == 1 && sendingState.task == 0) {
      sensorState = digitalRead(sensor);
      if (sensorState == 0 && prev_sensorState == 1) {
        digitalWrite(led, LOW);
        sendingState.task = 1;
        sendingState.right++;
      }
      prev_sensorState = sensorState;
      delay(10);
      if (incomingCommand == 0) {
        sendingState.task = 0;
        break;
      }
    }
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
    while (incomingCommand == 1) {
    }
  }*/

  if (incomingCommand == 1) {
    digitalWrite(led, HIGH);
    while (incomingCommand == 1) {
      sensorState = digitalRead(sensor);
      if (sensorState == 0 && prev_sensorState == 1) {
        digitalWrite(led, LOW);
        sendingState.task = 1;
        sendingState.right++;
      }
      prev_sensorState = sensorState;
      delay(10);
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
    
      if (incomingCommand == 0) {
        sendingState.task = 0;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
        break;
      }
    }
  }
  
  if (incomingCommand == 2 && incomingCommand != oldCommand) {
    digitalWrite(led, HIGH);
    sendingState.task = 0;
    sendingState.wrong = 0;
    sendingState.right = 0;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == 3 && incomingCommand != oldCommand) {
    digitalWrite(led, HIGH);
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  if (incomingCommand == 4 & incomingCommand != oldCommand) {
    digitalWrite(led, HIGH);
    sendingState.task = 1;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendingState, sizeof(sendingState));
  }

  oldCommand = incomingCommand;
}