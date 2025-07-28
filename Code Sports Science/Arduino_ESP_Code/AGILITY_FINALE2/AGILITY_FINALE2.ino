#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SDA_DIO 49
#define RESET_DIO 48
MFRC522 RC522(SDA_DIO, RESET_DIO);

#define startPin A15
#define finishPin A14
#define ledRFID A13
#define ledStart A12
#define ledFinish A11
#define buzzerPin 12

//DEFINE REMOTE PIN
#define buttonDebug 8
#define buttonB A8
#define buttonUp 6    
#define buttonRight 3  
#define buttonDown 5
#define buttonLeft 4 

bool start_state = 0;
bool finish_state = 0;
bool stateButtonB = 0;
bool prevstateButtonB = 0;

unsigned long start_time;
unsigned long finish_time;
unsigned long timeout;

float final_time;
float speed;
int penalty = 0;

String uid = "";
bool state_RFID = 0;
bool state_buzzer = 0;

int totalColumns = 16;
int totalRows = 2;
LiquidCrystal_I2C lcd(0x27, totalColumns, totalRows);
int cursorPosition;

bool debugMode = 0;
bool stateUp = 1;
bool stateDown = 1;
bool stateLeft = 1;
bool stateRight = 1;
bool lastStateLeft = 1;
bool lastStateRight = 1;
bool lastStateUp = 1;
bool lastStateDown = 1;

int totalDist = 20;
int distance[3] = { 0, 2, 0 };

void setup() {
  SPI.begin();
  RC522.PCD_Init();
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);
  pinMode(startPin, INPUT_PULLUP);
  pinMode(finishPin, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);

  pinMode(ledRFID, OUTPUT);
  pinMode(ledStart, OUTPUT);
  pinMode(ledFinish, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.setCursor(4, 0);
  lcd.print("SportsHub");
  lcd.setCursor(1, 1);
  lcd.print("Sprint Device");
  delay(2000);
  lcd.clear();
}
void loop() {
  while (state_RFID == 0) {
    lcd.setCursor(4, 0);
    lcd.print("Scan your");
    lcd.setCursor(2, 1);
    lcd.print("RFID Card/Tag");
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

    if (digitalRead(buttonDebug) == 0) {  //to enter debug mode, hold button for 3 seconds
      timeout = millis();
      while (digitalRead(buttonDebug) == 0 && debugMode == 0) {
        if ((millis() - timeout) >= 2500) {
          debugMode = 1;
          digitalWrite(buzzerPin, HIGH);
          delay(500);
          digitalWrite(buzzerPin, LOW);
        }
      }
      while (digitalRead(buttonDebug) == 0) {
      }
      while (debugMode == 1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set Distance:");
        lcd.setCursor(0, 1);
        lcd.print("Distance: ");
        lcd.setCursor(14, 1);
        lcd.print("m");

        debug();

        totalDist = distance[0] * 100 + distance[1] * 10 + distance[2];
        Serial.println(totalDist);
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Distance Set!");
        lcd.setCursor(5, 1);
        lcd.print(totalDist);
        lcd.setCursor(9, 1);
        lcd.print("m");
        delay(1000);
        lcd.clear();
      }
    }
  }

  if (state_RFID == 1 && state_buzzer == 0) {
    state_buzzer = 1;
    lcd.clear();
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
    digitalWrite(buzzerPin, HIGH);
    delay(800);
    digitalWrite(buzzerPin, LOW);
  }

  if (start_state == 1 && finish_state == 1) {

    final_time = abs(finish_time - start_time) / 1000.00;
    speed = totalDist / final_time;
    String finalData = String(uid) + "," + String(final_time) + "," + String(speed) + "," + String(penalty);
    Serial.println(finalData);
    lcd.setCursor(0, 0);
    lcd.print("Result");
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.setCursor(6, 1);
    lcd.print(final_time);
    lcd.setCursor(11, 1);
    lcd.print("s");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Velocity: ");
    lcd.setCursor(0, 1);
    lcd.print(speed);
    lcd.setCursor(6, 1);
    lcd.print("m/s");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Penalty: ");
    lcd.setCursor(0, 1);
    lcd.print(penalty);
    lcd.setCursor(6, 1);
    lcd.print("points");
    delay(2000);
    lcd.clear();
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

void debug() {
  int num;
  int n = 10;

  while (debugMode == 1) {
    stateUp = digitalRead(buttonUp);
    stateDown = digitalRead(buttonDown);
    stateLeft = digitalRead(buttonLeft);
    stateRight = digitalRead(buttonRight);

    for (int i = 0; i < 3; i++) {
      lcd.setCursor(10 + i, 1);
      lcd.print(distance[i]);
    }

    lcd.setCursor(14, 1);
    lcd.print("m");

    if (stateLeft == 0 && lastStateLeft == 1) {
      n--;
      if (n < 10) {
        n = 10;
      }
    }

    if (stateRight == 0 && lastStateRight == 1) {
      n++;
      if (n > 12) {
        n = 12;
      }
    }

    lcd.setCursor(n, 1);
    lcd.blink();
    delay(300);
    lcd.blink_off();

    if (stateUp == 0 && lastStateUp == 1) {
      num = distance[n - 10];
      num++;
      distance[n - 10] = num % 10;
    }

    if (stateDown == 0 && lastStateDown == 1) {
      num = distance[n - 10];
      num--;
      if (num < 0) {
        num = 0;
      }
      distance[n - 10] = num % 10;
    }

    lastStateUp = stateUp;
    lastStateDown = stateDown;
    lastStateLeft = stateLeft;
    lastStateRight = stateRight;

    if (digitalRead(buttonDebug) == 0) {  //to quit debug mode, hold Button for 3 seconds
      timeout = millis();
      while (digitalRead(buttonDebug) == 0 && debugMode == 1) {
        if ((millis() - timeout) >= 2500) {
          debugMode = 0;
          digitalWrite(buzzerPin, HIGH);
          delay(500);
          digitalWrite(buzzerPin, LOW);
        }
      }
      while (digitalRead(buttonDebug) == 0) {
      }
    }
  }
}