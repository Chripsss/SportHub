//LIBRARY
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

//DEFINE PIN RFID.....
#define SDA_DIO 49
#define RESET_DIO 48
MFRC522 RC522(SDA_DIO, RESET_DIO);

//DEFINE PIN IR SENSOR....
#define IR_1 2
#define IR_2 3
#define IR_3 4
#define IR_4 5
#define IR_5 6

#define led1 8
#define led2 9
#define led3 10
#define led4 11
#define led5 12

#define LED_RFID 7
#define buzzerPin 14

#define buttonUp A10    //Button A
#define buttonRight A8  //Button B
#define buttonDown A11  //Button C
#define buttonLeft A9   //Button D

#define buttonDebug A7

//DEFINE STATE OF SENSOR.........
bool state_1 = 0;
bool state_2 = 0;
bool state_3 = 0;
bool state_4 = 0;
bool state_5 = 0;

//SET DISTANCE.........
int totalDist = 20;
float d5 = 20;  //meter
float d4 = 15;  //meter
float d3 = 10;  //meter
float d2 = 5;   //meter
float d1 = 0;   //meter
int distance[3] = { 0, 2, 0 };

// DEFINE VARIABLE FOR TIME.........
unsigned long time_1;
unsigned long time_2;
unsigned long time_3;
unsigned long time_4;
unsigned long time_5;
unsigned long timeout;

//DEFINE VARIABLE FOR TIME TAKEN PASSED THROUGH EACH POLES....
float travel_12;
float travel_23;
float travel_34;
float travel_45;
float travel_13;
float travel_24;
float travel_35;

//DEFINE VARIABLE FOR SPEED PASSED THROUGH EACH POLES....
float vel_1;
float vel_2;
float vel_3;
float vel_4;

//DEFINE ACCELERATION
float acc_13;
float acc_24;
float acc_35;

float finalTime;  //variable for time taken from start to finish line
float maxVel;     //variable for maximum speed
float avgVel;     //variable for maximum speed
float maxAcc;     //variable for maximum speed

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

void setup() {
  SPI.begin();
  RC522.PCD_Init();
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);
  pinMode(IR_1, INPUT_PULLUP);
  pinMode(IR_2, INPUT_PULLUP);
  pinMode(IR_3, INPUT_PULLUP);
  pinMode(IR_4, INPUT_PULLUP);
  pinMode(IR_5, INPUT_PULLUP);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);

  pinMode(LED_RFID, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);

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
      digitalWrite(LED_RFID, HIGH);
      lcd.clear();
    }

    if (digitalRead(IR_1) == 1) {
      digitalWrite(led1, HIGH);
    }
    if (digitalRead(IR_2) == 1) {
      digitalWrite(led2, HIGH);
    }
    if (digitalRead(IR_3) == 1) {
      digitalWrite(led3, HIGH);
    }
    if (digitalRead(IR_4) == 1) {
      digitalWrite(led4, HIGH);
    }
    if (digitalRead(IR_5) == 1) {
      digitalWrite(led5, HIGH);
    }

    if (digitalRead(IR_1) == 0) {
      digitalWrite(led1, LOW);
    }
    if (digitalRead(IR_2) == 0) {
      digitalWrite(led2, LOW);
    }
    if (digitalRead(IR_3) == 0) {
      digitalWrite(led3, LOW);
    }
    if (digitalRead(IR_4) == 0) {
      digitalWrite(led4, LOW);
    }
    if (digitalRead(IR_5) == 0) {
      digitalWrite(led5, LOW);
    }

    if (digitalRead(buttonDebug) == 0) {  //to enter debug mode, hold button for 3 seconds
      timeout = millis();
      while (digitalRead(buttonDebug) == 0 && debugMode == 0) {
        if ((millis() - timeout) >= 2500) {
          lcd.clear();
          debugMode = 1;
          digitalWrite(buzzerPin, HIGH);
          delay(500);
          digitalWrite(buzzerPin, LOW);
        }
      }
      while (digitalRead(buttonDebug) == 0) {
      }
      while (debugMode == 1) {
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

        d2 = 1 / 4 * totalDist;
        d3 = 2 / 4 * totalDist;
        d4 = 3 / 4 * totalDist;
        d5 = totalDist;
      }
    }
  }

  if (state_RFID == 1 && state_buzzer == 0) {
    state_buzzer = 1;
    lcd.clear();
    digitalWrite(buzzerPin, HIGH);
    delay(400);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);
  }

  if (state_RFID == 1 && digitalRead(IR_1) == 0 && state_1 == 0) {
    time_1 = millis();
    state_1 = 1;
    digitalWrite(led1, HIGH);
  }
  if (digitalRead(IR_2) == 0 && state_1 == 1 && state_2 == 0) {
    time_2 = millis();
    state_2 = 1;
    digitalWrite(led2, HIGH);
  }
  if (digitalRead(IR_3) == 0 && state_2 == 1 && state_3 == 0) {
    time_3 = millis();
    state_3 = 1;
    digitalWrite(led3, HIGH);
  }
  if (digitalRead(IR_4) == 0 && state_3 == 1 && state_4 == 0) {
    time_4 = millis();
    state_4 = 1;
    digitalWrite(led4, HIGH);
  }
  if (digitalRead(IR_5) == 0 && state_4 == 1 && state_5 == 0) {
    time_5 = millis();
    state_5 = 1;
    digitalWrite(led5, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(800);
    digitalWrite(buzzerPin, LOW);
  }

  if (state_1 == 1 && state_2 == 1 && state_3 == 1 && state_4 == 1 && state_5 == 1) {

    //Measure time taken to reach each poles and convert from ms to s
    travel_12 = (time_2 - time_1) / 1000.0;
    travel_23 = (time_3 - time_2) / 1000.0;
    travel_34 = (time_4 - time_3) / 1000.0;
    travel_45 = (time_5 - time_4) / 1000.0;

    travel_13 = (time_3 - time_1) / 1000.0;
    travel_24 = (time_4 - time_2) / 1000.0;
    travel_35 = (time_5 - time_3) / 1000.0;

    //Calculate Time taken from START to FINISH Line....
    finalTime = (time_5 - time_1) / 1000.0;

    //Calculate Velocity
    vel_1 = (d2 - d1) / (travel_12) * (3600.0 / 1000.0);  // km/h
    vel_2 = (d3 - d2) / (travel_23) * (3600.0 / 1000.0);  // km/h
    vel_3 = (d4 - d3) / (travel_34) * (3600.0 / 1000.0);  // km/h
    vel_4 = (d5 - d4) / (travel_45) * (3600.0 / 1000.0);  // km/h

    //Find the maximum velocity among vel_1A, vel_2A, vel_3A, and vel_4A
    maxVel = max(vel_1, max(vel_2, max(vel_3, vel_4)));
    avgVel = (vel_1 + vel_2 + vel_3 + vel_4) / 4.0;

    //Calculate Acceleration
    acc_13 = abs(vel_2 - vel_1) * (1000.0 / 3600.0) / (travel_13);  // m/s^2
    acc_24 = abs(vel_3 - vel_2) * (1000.0 / 3600.0) / (travel_24);  // m/s^2
    acc_35 = abs(vel_4 - vel_3) * (1000.0 / 3600.0) / (travel_35);  // m/s^2

    //Find the maximum acceleration among acc_12A, acc_23A, acc_34A
    maxAcc = max(acc_13, max(acc_24, acc_35));

    String finalData = String(uid) + "," + String(finalTime) + "," + String(avgVel) + "," + String(maxVel) + "," + String(maxAcc);
    Serial.println(finalData);
    lcd.setCursor(0, 0);
    lcd.print("Result");
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.setCursor(6, 1);
    lcd.print(finalTime);
    lcd.setCursor(11, 1);
    lcd.print("s");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Velocity: ");
    lcd.setCursor(0, 1);
    lcd.print(avgVel);
    lcd.setCursor(6, 1);
    lcd.print("m/s");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Acceleration: ");
    lcd.setCursor(0, 1);
    lcd.print(maxAcc);
    lcd.setCursor(6, 1);
    lcd.print("m/s2");
    delay(2000);
    lcd.clear();
  }

  if (state_5 == 1) {
    reset_IRState();
    turn_off();
  }
}

void turn_off() {
  digitalWrite(LED_RFID, LOW);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
}

void reset_IRState() {
  state_1 = 0;
  state_2 = 0;
  state_3 = 0;
  state_4 = 0;
  state_5 = 0;
  state_RFID = 0;
  state_buzzer = 0;
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