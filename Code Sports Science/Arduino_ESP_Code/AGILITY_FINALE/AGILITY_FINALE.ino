#include <SPI.h>
#include <MFRC522.h>

#define SDA_DIO 49
#define RESET_DIO 48
MFRC522 RC522(SDA_DIO, RESET_DIO);

#define startPin A0
#define finishPin A1
#define ledRFID A2
#define ledStart A3
#define ledFinish A4
#define buzzerPin 12
#define buzzerButton 6
#define resetButton 40

//DEFINE REMOTE PIN
#define buttonA A10
#define buttonB A8
#define buttonC A11
#define buttonD A9

bool start_state = 0;
bool finish_state = 0;
bool stateButtonB = 0;
bool prevstateButtonB = 0;

unsigned long start_time;
unsigned long finish_time;

float final_time;
float speed;
int penalty = 0;

String uid = "";
bool state_RFID = 0;
bool state_buzzer = 0;

void setup() {
  SPI.begin();
  RC522.PCD_Init();

  Serial.begin(115200);
  pinMode(startPin, INPUT_PULLUP);
  pinMode(finishPin, INPUT_PULLUP);
  pinMode(buzzerButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonC, INPUT_PULLUP);
  pinMode(buttonD, INPUT_PULLUP);

  pinMode(ledRFID, OUTPUT);
  pinMode(ledStart, OUTPUT);
  pinMode(ledFinish, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}
void loop() {
  while (state_RFID == 0) {
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
    }

    if (digitalRead(startPin) == 1) {
      digitalWrite(ledStart, HIGH);
    }
    if (digitalRead(finishPin) == 1) {
      digitalWrite(ledFinish, HIGH);
    }
    if (digitalRead(startPin) == 0) {
      digitalWrite(ledStart, LOW);
    }
    if (digitalRead(finishPin) == 0) {
      digitalWrite(ledFinish, LOW);
    }
  }

  if (state_RFID == 1 && state_buzzer == 0) {
    state_buzzer = 1;
    digitalWrite(ledStart, LOW);
    digitalWrite(ledFinish, LOW);
    digitalWrite(buzzerPin, HIGH);
    delay(400);
    digitalWrite(buzzerPin, LOW);
  }

  stateButtonB = digitalRead(buttonB);
  if (state_RFID == 1 && stateButtonB == 0 && prevstateButtonB == 1) {
    penalty++;
    digitalWrite(buzzerPin, HIGH);
    delay(400);
    digitalWrite(buzzerPin, LOW);
  }
  prevstateButtonB = stateButtonB;

  if (state_RFID == 1 && digitalRead(startPin) == 0 && start_state == 0) {
    start_time = millis();
    start_state = 1;
    digitalWrite(ledStart, HIGH);
  }

  if (start_state == 1 && digitalRead(finishPin) == 0 && finish_state == 0) {
    finish_time = millis();
    finish_state = 1;
    digitalWrite(ledFinish, HIGH);
  }

  if (start_state == 1 && finish_state == 1) {

    final_time = abs(finish_time - start_time) / 1000.00;
    speed = 20.00/final_time;
    String finalData = String(uid) + "," + String(final_time) + "," + String(speed) + "," + String(penalty);
    Serial.println(finalData);
  }
  if (finish_state == 1) {
    delay(100);
    digitalWrite(buzzerPin, HIGH);
    delay(800);
    digitalWrite(buzzerPin, LOW);
    reset();
    turn_off();
  }
}

void turn_off() {
  digitalWrite(ledRFID, LOW);
  digitalWrite(ledStart, LOW);
  digitalWrite(ledFinish, LOW);
}

void reset() {
  start_state = 0;
  finish_state = 0;
  state_RFID = 0;
  state_buzzer = 0;
  penalty = 0;
  uid = "";
}