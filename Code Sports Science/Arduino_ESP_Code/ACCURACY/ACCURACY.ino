//ADD LIBRARY
#include <Adafruit_NeoPixel.h>  // Add library for led strip


//LIBRARY RFID........
#include <SPI.h>
#include <MFRC522.h>

//DEFINE PIN RFID.....
#define SDA_DIO 49
#define RESET_DIO 48
MFRC522 RC522(SDA_DIO, RESET_DIO);

// DEFINE LED PIN
#define LED_PIN1 A0
#define LED_PIN2 A1
#define LED_PIN3 A2
#define LED_PIN4 A3


//DEFINE INDIKATOR (BUZZER & LED LAMP)
#define LED_RFID A4
#define buzzerPin A5

//DEFINE REMOTE PIN
#define buttonA A10
#define buttonB A8
#define buttonC A11
#define buttonD A9

//DEFINE START BUTTON
#define startButton 53

// DEFINE STRIP NUMBER
#define STRIP_NUMBER 55

//SETUP LED
Adafruit_NeoPixel LED1(STRIP_NUMBER, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LED2(STRIP_NUMBER, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LED3(STRIP_NUMBER, LED_PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LED4(STRIP_NUMBER, LED_PIN4, NEO_GRB + NEO_KHZ800);


//DEFINE SENSOR PIN
#define SENSOR_PIN1A 2
#define SENSOR_PIN1B 3
#define SENSOR_PIN1C 4

#define SENSOR_PIN2A 5
#define SENSOR_PIN2B 6
#define SENSOR_PIN2C 7

#define SENSOR_PIN3A 8
#define SENSOR_PIN3B 9
#define SENSOR_PIN3C 10

#define SENSOR_PIN4A 11
#define SENSOR_PIN4B 12
#define SENSOR_PIN4C 13

//STATE RFID
String uid = "";
bool state_RFID = 0;

// Declare an array to hold three sensor values A, B, and C
int sensorValues[3];

//DEFINE STATE TO GENERATE RANDOM NUMBER
bool state = 0;

//TRACK GENERATED RANDOM LED
int randomLed;
int oldRandomLed;
int delayHelp = 10;

//TRACK SCORE
int right;
int wrong;
int shoot;
float accuracy;

// TRACK DURATION
unsigned long startGame;

//DEFINE GAME DURATION
int gameDuration = 30;

//DEFINE REMOTE BUTTON STATE
int stateButtonA = 0;
int stateButtonB = 0;
int stateButtonC = 0;
int stateButtonD = 0;


void setup() {

  //SET SERIAL BEGIN
  Serial.begin(115200);

  //SET SENSOR PIN
  pinMode(SENSOR_PIN1A, INPUT_PULLUP);
  pinMode(SENSOR_PIN1B, INPUT_PULLUP);
  pinMode(SENSOR_PIN1C, INPUT_PULLUP);

  pinMode(SENSOR_PIN2A, INPUT_PULLUP);
  pinMode(SENSOR_PIN2B, INPUT_PULLUP);
  pinMode(SENSOR_PIN2C, INPUT_PULLUP);

  pinMode(SENSOR_PIN3A, INPUT_PULLUP);
  pinMode(SENSOR_PIN3B, INPUT_PULLUP);
  pinMode(SENSOR_PIN3C, INPUT_PULLUP);

  pinMode(SENSOR_PIN4A, INPUT_PULLUP);
  pinMode(SENSOR_PIN4B, INPUT_PULLUP);
  pinMode(SENSOR_PIN4C, INPUT_PULLUP);

  //DEFINE INDIKATOR
  pinMode(startButton, INPUT_PULLUP);
  pinMode(LED_RFID, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  //SET REMOTE PIN
  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonC, INPUT_PULLUP);
  pinMode(buttonD, INPUT_PULLUP);

  //SETUP LED
  LED1.begin();
  LED2.begin();
  LED3.begin();
  LED4.begin();
  LED1.show();
  LED2.show();
  LED3.show();
  LED4.show();
  LED1.setBrightness(10);
  LED2.setBrightness(10);
  LED3.setBrightness(10);
  LED4.setBrightness(10);

  //SETUP RFID
  SPI.begin();
  RC522.PCD_Init();
}


void loop() {
  while (keyPress() == 0) {
    //wait until rfid being tap
  }
  ledOffAll();
  while (digitalRead(startButton) == 0) {
    //wait until startbutton being pressed
  }
  redAll();
  buzzerCountDown();
  startGame = millis();

  while ((millis() - startGame) / 1000 < gameDuration) {
    playGame();
  }
  shoot = right + wrong;
  accuracy = ((right * 1.0) / (right * 1.0 + wrong * 1.0)) * 100.00;
  Serial.println(String(uid)+ "," + String(shoot) + "," + String(right) + "," + String(wrong) + "," + String(accuracy));
  blueAll();
  delay(5000);
  right = 0;
  wrong = 0;
  state_RFID = 0;
  accuracy = 0;
  uid = "";
  digitalWrite(LED_RFID, LOW);
}

boolean keyPress() {
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
      digitalWrite(LED_RFID, HIGH);
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
  if (state == 0) {
    state = 1;
    randomLed = random(1, 5);
    while (randomLed == oldRandomLed) {
      randomLed = random(1, 5);
    }
    ledOnBasedOnGenerateRandom(randomLed);
  }

  oldRandomLed = randomLed;

  int *sensorRead = isSensorDetected(randomLed);

  while (digitalRead(SENSOR_PIN1A) == 1 && digitalRead(SENSOR_PIN1B) == 1 && digitalRead(SENSOR_PIN1C) == 1 && digitalRead(SENSOR_PIN2A) == 1 && digitalRead(SENSOR_PIN2B) == 1 && digitalRead(SENSOR_PIN2C) == 1 && digitalRead(SENSOR_PIN3A) == 1 && digitalRead(SENSOR_PIN3B) == 1 && digitalRead(SENSOR_PIN3C) == 1 && digitalRead(SENSOR_PIN4A) == 1 && digitalRead(SENSOR_PIN4B) == 1 && digitalRead(SENSOR_PIN4C) == 1 && digitalRead(buttonB) == 0) {
  }

  delay(delayHelp);

  if ((digitalRead(sensorRead[0]) == 0 || digitalRead(sensorRead[1]) == 0 || digitalRead(sensorRead[2]) == 0) && state == 1) {
    right++;
    ledOffBasedOnGenerateRandom(randomLed);
  } else if ((digitalRead(sensorRead[0]) == 1 || digitalRead(sensorRead[1]) == 1 || digitalRead(sensorRead[2]) == 1) && state == 1) {
    wrong++;
    ledOffBasedOnGenerateRandom(randomLed);
  } else if (digitalRead(buttonB) == 1) {
    wrong++;
    ledOffBasedOnGenerateRandom(randomLed);
  }

  while (digitalRead(buttonA) == 0) {
    //while holding the button
  }
  state = 0;
}



int isSensorDetected(int randomLed) {
  if (randomLed == 1) {
    sensorValues[0] = 2;
    sensorValues[1] = 3;
    sensorValues[2] = 4;
  } else if (randomLed == 2) {
    sensorValues[0] = 5;
    sensorValues[1] = 6;
    sensorValues[2] = 7;
  } else if (randomLed == 3) {
    sensorValues[0] = 8;
    sensorValues[1] = 9;
    sensorValues[2] = 10;
  } else if (randomLed == 4) {
    sensorValues[0] = 11;
    sensorValues[1] = 12;
    sensorValues[2] = 13;
  }
  return sensorValues;
}

void ledOnBasedOnGenerateRandom(int randomLed) {
  if (randomLed == 1) {
    for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
      LED1.setPixelColor(stripLed, 0, 255, 0);
      LED2.setPixelColor(stripLed, 225, 0, 0);
      LED3.setPixelColor(stripLed, 225, 0, 0);
      LED4.setPixelColor(stripLed, 225, 0, 0);
    }
    ledOn();
  } else if (randomLed == 2) {
    for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
      LED1.setPixelColor(stripLed, 255, 0, 0);
      LED2.setPixelColor(stripLed, 0, 255, 0);
      LED3.setPixelColor(stripLed, 225, 0, 0);
      LED4.setPixelColor(stripLed, 225, 0, 0);
    }
    ledOn();
  } else if (randomLed == 3) {
    for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
      LED1.setPixelColor(stripLed, 255, 0, 0);
      LED2.setPixelColor(stripLed, 225, 0, 0);
      LED3.setPixelColor(stripLed, 0, 255, 0);
      LED4.setPixelColor(stripLed, 225, 0, 0);
    }
    ledOn();
  } else if (randomLed == 4) {
    for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
      LED1.setPixelColor(stripLed, 255, 0, 0);
      LED2.setPixelColor(stripLed, 225, 0, 0);
      LED3.setPixelColor(stripLed, 225, 0, 0);
      LED4.setPixelColor(stripLed, 0, 255, 0);
    }
    ledOn();
  }
}

void ledOffBasedOnGenerateRandom(int randomLed) {
  if (randomLed == 1) {
    for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
      LED1.setPixelColor(stripLed, 255, 0, 0);
    }
    LED1.show();
  } else if (randomLed == 2) {
    for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
      LED2.setPixelColor(stripLed, 255, 0, 0);
    }
    LED2.show();
  } else if (randomLed == 3) {
    for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
      LED3.setPixelColor(stripLed, 255, 0, 0);
    }
    LED3.show();
  } else if (randomLed == 4) {
    for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
      LED4.setPixelColor(stripLed, 255, 0, 0);
    }
    LED4.show();
  }
}

void ledOn() {
  LED1.show();
  LED2.show();
  LED3.show();
  LED4.show();
}

void ledOffAll() {
  LED1.clear();
  LED1.show();
  LED2.clear();
  LED2.show();
  LED3.clear();
  LED3.show();
  LED4.clear();
  LED4.show();
}


void buzzerCountDown() {
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
}

void redAll() {
  for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
    LED1.setPixelColor(stripLed, 225, 0, 0);
    LED2.setPixelColor(stripLed, 225, 0, 0);
    LED3.setPixelColor(stripLed, 225, 0, 0);
    LED4.setPixelColor(stripLed, 225, 0, 0);
  }
  LED1.show();
  LED2.show();
  LED3.show();
  LED4.show();
}

void greenAll() {
  for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
    LED1.setPixelColor(stripLed, 0, 255, 0);
    LED2.setPixelColor(stripLed, 0, 255, 0);
    LED3.setPixelColor(stripLed, 0, 255, 0);
    LED4.setPixelColor(stripLed, 0, 255, 0);
  }
  LED1.show();
  LED2.show();
  LED3.show();
  LED4.show();
}

void blueAll() {
  for (int stripLed = STRIP_NUMBER; stripLed >= 0; stripLed--) {
    LED1.setPixelColor(stripLed, 0, 0, 255);
    LED2.setPixelColor(stripLed, 0, 0, 255);
    LED3.setPixelColor(stripLed, 0, 0, 255);
    LED4.setPixelColor(stripLed, 0, 0, 255);
  }
  LED1.show();
  LED2.show();
  LED3.show();
  LED4.show();
}
