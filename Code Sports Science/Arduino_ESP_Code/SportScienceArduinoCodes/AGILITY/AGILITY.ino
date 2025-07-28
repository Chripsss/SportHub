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

bool start_state = 0;
bool finish_state = 0;
bool reset_state = 0;

unsigned long start_time;
unsigned long finish_time;

float final_time;
float speed;
int penalty;

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

  pinMode(ledRFID, OUTPUT);
  pinMode(ledStart, OUTPUT);
  pinMode(ledFinish, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}
void loop() {
  bool currentResetState = digitalRead(resetButton);
  // Check if the button state has changed
  if (currentResetState != reset_state) {
    // Update the button state
    reset_state = currentResetState;
    if (!reset_state) {
      // Select and display a random color
      Serial.print("Delete");
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
  }

  if (state_RFID == 1 && state_buzzer == 0) {
    state_buzzer = 1;
    digitalWrite(buzzerPin, HIGH);
    delay(400);
    digitalWrite(buzzerPin, LOW);
  }



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
    penalty = 5;
    String finalData = String(uid) + "," + String(final_time) + "," + String(speed) + "," + String(penalty);
    Serial.println(finalData);
  }
  if (finish_state == 1) {
    delay(100);
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
  uid = "";
}