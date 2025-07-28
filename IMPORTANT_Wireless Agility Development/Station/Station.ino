#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[3][6] = {
  {0xCC, 0xDB, 0xA7, 0x2D, 0xEF, 0xC0},
  {0xB4, 0xE6, 0x2D, 0xF6, 0xD1, 0x19},
  {0xA4, 0xCF, 0x12, 0x44, 0x03, 0x9C}
};

int availablePole = 2;

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

int wrong;

unsigned long start_time;
unsigned long finish_time;
float final_time;
float speed;
float totalDist = 10.0;

int value;
int path;
int a;
int b;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&incomingState, incomingData, sizeof(incomingState));
  boardsStruct[incomingState.id-1].pass = incomingState.pass;
  boardsStruct[incomingState.id-1].wrong = incomingState.wrong;
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

  delay(5000);
}

void loop() {
  poleIdle();
  esp_err_t result = esp_now_send(broadcastAddress[0], (uint8_t *) &commandCheck, sizeof(commandCheck));
  while (boardsStruct[0].pass == 0) {
    Serial.println("stuck1");
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
  
  for (int i = 0; i < availablePole; i++) {
    esp_err_t result1 = esp_now_send(broadcastAddress[i+1], (uint8_t *) &commandCheck, sizeof(commandCheck));
    while (boardsStruct[i+1].pass == 0) {
      Serial.println("stuck2");
    }
    value = boardsStruct[i+1].pass;

    if ((i%2) == 0) {
      if (value == a) {
        esp_err_t result2 = esp_now_send(broadcastAddress[i], (uint8_t *) &commandOn, sizeof(commandOn));
      }
      if (value == b) {
        while (value == b) {
          value = boardsStruct[i+1].pass;
          Serial.println("wrong");
        }
        esp_err_t result2 = esp_now_send(broadcastAddress[i], (uint8_t *) &commandOn, sizeof(commandOn));
      }
    }

    if ((i%2) == 1) {
      if (value == a) {
        while (value == a) {
          value = boardsStruct[i+1].pass;
          Serial.println("wrong");
        }
        esp_err_t result2 = esp_now_send(broadcastAddress[i], (uint8_t *) &commandOn, sizeof(commandOn));
      }
      if (value == b) {
        esp_err_t result2 = esp_now_send(broadcastAddress[i], (uint8_t *) &commandOn, sizeof(commandOn));
      }
    }

    delay(100);
  }

  finish_time = millis();
  final_time = (finish_time - start_time) / 1000.00;
  speed = totalDist / final_time;

  Serial.println(final_time);
  Serial.println(speed);
  
  int o = 0;
  while (o == 0) {
  }
}

  /*for (int i = 0; i < 2; i++) {
    if (test == 1) {
      esp_err_t result1 = esp_now_send(broadcastAddress[i+1], (uint8_t *) &commandCheck, sizeof(commandCheck));
    }
    
    while (boardsStruct[i+1].pass == 0) {
      Serial.println("stuck2");
    }

    test = boardsStruct[i+1].pass;

    if (test == 1) {
      esp_err_t result2 = esp_now_send(broadcastAddress[i+1], (uint8_t *) &commandOn, sizeof(commandOn));
    }
    else if (test == 2) {
      Serial.println("wrong side");
    }
  }*/

void poleIdle() {
  for (int i = 0; i < availablePole; i++) {
    esp_err_t result = esp_now_send(broadcastAddress[i], (uint8_t *) &commandIdle, sizeof(commandIdle));
  }
}
