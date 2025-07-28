//LIBRARY RFID........
#include <SPI.h>
#include <MFRC522.h>

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
#define buzzerButton 15
#define buzzerPin 14

//DEFINE STATE OF SENSOR.........
bool state_1 = 0;
bool state_2 = 0;
bool state_3 = 0;
bool state_4 = 0;
bool state_5 = 0;

//SET DISTANCE.........
float d5 = 20;  //meter
float d4 = 15;  //meter
float d3 = 10;  //meter
float d2 = 5;   //meter
float d1 = 0;   //meter

// DEFINE VARIABLE FOR TIME.........
unsigned long time_1;
unsigned long time_2;
unsigned long time_3;
unsigned long time_4;
unsigned long time_5;

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

void setup() {
  SPI.begin();
  RC522.PCD_Init();

  Serial.begin(115200);
  pinMode(IR_1, INPUT_PULLUP);
  pinMode(IR_2, INPUT_PULLUP);
  pinMode(IR_3, INPUT_PULLUP);
  pinMode(IR_4, INPUT_PULLUP);
  pinMode(IR_5, INPUT_PULLUP);
  pinMode(buzzerButton, INPUT_PULLUP);

  pinMode(LED_RFID, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
}

void loop() {
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
    //digitalWrite(LED_RFID, HIGH);
    state_RFID = 1;
    digitalWrite(LED_RFID, HIGH);
  }

  if (state_RFID == 1 && state_buzzer == 0) {
    state_buzzer = 1;
    digitalWrite(buzzerPin, HIGH);
    delay(400);
    digitalWrite(buzzerPin, LOW);
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
  }

  if (state_1 == 1 && state_2 == 1 && state_3 == 1 && state_4 == 1 && state_5 == 1) {

    // Measure time taken to reach each poles and convert from ms to s
    travel_12 = (time_2 - time_1) / 1000.0;
    travel_23 = (time_3 - time_2) / 1000.0;
    travel_34 = (time_4 - time_3) / 1000.0;
    travel_45 = (time_5 - time_4) / 1000.0;

    travel_13 = (time_3 - time_1) / 1000.0;
    travel_24 = (time_4 - time_2) / 1000.0;
    travel_35 = (time_5 - time_3) / 1000.0;

    //Calculate Time taken from START to FINISH Line....
    finalTime = (time_5 - time_1) / 1000.0;

    // Calculate Velocity
    vel_1 = (d2 - d1) / (travel_12) * (3600.0 / 1000.0);  // km/h
    vel_2 = (d3 - d2) / (travel_23) * (3600.0 / 1000.0);  // km/h
    vel_3 = (d4 - d3) / (travel_34) * (3600.0 / 1000.0);  // km/h
    vel_4 = (d5 - d4) / (travel_45) * (3600.0 / 1000.0);  // km/h

    // Find the maximum velocity among vel_1A, vel_2A, vel_3A, and vel_4A
    maxVel = max(vel_1, max(vel_2, max(vel_3, vel_4)));
    avgVel = (vel_1 + vel_2 + vel_3 + vel_4) / 4.0;

    //Calculate Acceleration
    acc_13 = abs(vel_2 - vel_1) * (1000.0 / 3600.0) / (travel_13);  // m/s^2
    acc_24 = abs(vel_3 - vel_2) * (1000.0 / 3600.0) / (travel_24);  // m/s^2
    acc_35 = abs(vel_4 - vel_3) * (1000.0 / 3600.0) / (travel_35);  // m/s^2

    // Find the maximum acceleration among acc_12A, acc_23A, acc_34A
    maxAcc = max(acc_13, max(acc_24, acc_35));

    // Serial.print(finalTime, 3);
    // Serial.print(" sec");
    // Serial.print("\t");
    // Serial.print(maxVel);
    // Serial.print(" km/h");
    // Serial.print("\t");
    // Serial.print(maxAcc);
    // Serial.print(" m/s^2");
    // Serial.print("\t");

    // Serial.print(travel_12, 3);
    // Serial.print("\t");
    // Serial.print(travel_23, 3);
    // Serial.print("\t");
    // Serial.print(travel_34, 3);
    // Serial.print("\t");
    // Serial.print(travel_45, 3);
    // Serial.print("\t");

    String finalData = String(uid) + "," + String(finalTime) + "," + String(avgVel) + "," + String(maxVel) + "," + String(maxAcc);
    Serial.println(finalData);

    // Serial.print(finalTime, 3);
    // Serial.print(", ");
    // Serial.print(maxVel, 3);
    // Serial.print(", ");
    // Serial.println(maxAcc, 3);
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