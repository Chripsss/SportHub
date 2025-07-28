/* Device name: Reaction Training
 Code Author: Wahyu Budiarta & Tiara Kusuma Dewi
 Comment Author: Aldrifa Farizki 
 Debugging code Author: Amartya Natayu*/


//LIBRARY RFID........
#include <SPI.h>
#include <MFRC522.h>

/*=======================
=======Definitions=======
=======================*/

//RFID pins
#define SDA_DIO 49
#define RESET_DIO 48
MFRC522 RC522(SDA_DIO, RESET_DIO);

//LED pins
#define led1 A0
#define led2 A1
#define led3 A2
#define led4 A3
#define led5 A4
#define led6 A8
#define LED_RFID A6
#define buzzerPin 12

//Button pins
#define sensor1 2
#define sensor2 3
#define sensor3 4
#define sensor4 5
#define sensor5 6
#define sensor6 7
#define startButton 8

//variable definitions
int buttonPressed;
int right;
int wrong;
bool state;
int timeout;
bool debugMode = 0;
bool i;

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

int randomLed;
int oldRandomLed1;
int oldRandomLed2;
int delayHelp = 10;

String uid = "";
bool state_RFID = 0;

void setup() {
  SPI.begin();
  RC522.PCD_Init();
  
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(LED_RFID, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(sensor1, INPUT_PULLUP);
  pinMode(sensor2, INPUT_PULLUP);
  pinMode(sensor3, INPUT_PULLUP);
  pinMode(sensor4, INPUT_PULLUP);
  pinMode(sensor5, INPUT_PULLUP);
  pinMode(sensor6, INPUT_PULLUP);
  pinMode(startButton, INPUT_PULLUP);

  randomSeed(analogRead(44));
}

void loop() {

  while (keyPress() == 0) {//loop that waits for the start button to be pressed.
  //will not execute this loop until the RFID has been scanned
  }
  ledOffAll();
  while (digitalRead(startButton) == 0 || digitalRead(sensor1) == 0 || digitalRead(sensor2) == 0 || digitalRead(sensor3) == 0 || digitalRead(sensor4) == 0 || digitalRead(sensor5) == 0 || digitalRead(sensor6) == 0) {
  //loops nothing waiting for the button ot be released
  }

//buzzer countdown
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
  while (((millis() - startGame) / 1000) < 20) {//gameplay. lasts for 20 seconds
    playGame();
  }

  for (int count = 0; count < dataIndex; count++) {
    //reaction time data sorting
    fastValue = min(data[count], dummyFast); // compares the current data in the array with the comparison value of dummyFast (set at 10)
    dummyFast = fastValue; // sets the new faster value as the comparison value
    slowValue = max(data[count], dummySlow); //same concept as the two lines above but compares the slow value (set at 0.001)
    dummySlow = slowValue;
    sum = sum + data[count];
  }
  avg = sum / right;
  fastest = fastValue;
  slowest = slowValue;
  accuracy = ((right * 1.0) / (right * 1.0 + wrong * 1.0)) * 100.00;

  Serial.println(String(uid) + "," + String(right) + "," + String(fastest, 3) + "," + String(slowest, 3) + "," + String(avg, 3) + "," + String(accuracy, 2));
  //this serial print is for the ESP32. sent serially

//Beyond this point in the main loop is data reset. 
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
  digitalWrite(A6, LOW);
  uid = "";

  ledOnAll();
  delay(2000);

}

/*=======================
======Game Functions=====
=======================*/

void playGame() {
  //this is the randomizer
  if (state == 0) {
    state = 1;
    randomLed = random(1, 7);//randomises the led between LED 1 to 6
    while (randomLed == oldRandomLed1) { //filter to make sure the led doesnt use the same number
      randomLed = random(1, 7);
      while (randomLed == oldRandomLed2) {// ditto as above
        randomLed = random(1, 7);
      }
    }
  }

  oldRandomLed2 = oldRandomLed1; //sets the previous value to be the second filter
  oldRandomLed1 = randomLed; //sets the current value as the first filter
  realTime = millis(); //takes the time before button is pressed
  ledOn(randomLed); // turns on the random LED

  int buttonPressed = isButtonPressed(randomLed);
  while (digitalRead(sensor1) == 1 && digitalRead(sensor2) == 1 && digitalRead(sensor3) == 1 && digitalRead(sensor4) == 1 && digitalRead(sensor5) == 1 && digitalRead(sensor6) == 1) {
    //button waiting to pressed
  }

  delay(delayHelp);

  if (digitalRead(buttonPressed) == 0 && state == 1) {
    right++;
    ledOff(randomLed);
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    state = 0;
  } else if (digitalRead(buttonPressed) == 1 && state == 1) {
    wrong++;
  }
  realTime = (millis() - realTime) / 1000.00; //records time difference with the time before the button is pressed 
  data[dataIndex] = realTime;

  while (digitalRead(sensor1) == 0 || digitalRead(sensor2) == 0 || digitalRead(sensor3) == 0 || digitalRead(sensor4) == 0 || digitalRead(sensor5) == 0 || digitalRead(sensor6) == 0) {
  //a loop of nothing waiting for the button to be released
  }


  delay(delayHelp);
  dataIndex++;
}

boolean keyPress() { //will not start if ESP is connected but wifi is off
  while (state_RFID == 0) {
    blinkAll(200); //will not allow keyPress() at line 103 to loop until RFID is scanned
    // if you want to make idle animations you can do so here
    // the animation will be gone after RFID is scanned
    if (digitalRead(startButton)==0) { //to enter debug mode, hold startButton for 3 seconds
      timeout = millis();
      while (digitalRead(startButton)==0 && debugMode==0) {
        if ((millis() - timeout) >= 2500) {
          debugMode = 1;
          i = 0;
          digitalWrite(buzzerPin, HIGH);
          delay(500);
          digitalWrite(buzzerPin, LOW);
        }
      }
      while (digitalRead(startButton)==0){
      }
      while (debugMode==1) {
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
      //digitalWrite(LED_RFID, HIGH);
      state_RFID = 1;
      digitalWrite(LED_RFID, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(400);
      digitalWrite(buzzerPin, LOW);
    }
  }
  ledOnAll();
  boolean pressed = 0;


  if (digitalRead(startButton) == 0 || digitalRead(sensor1) == 0 || digitalRead(sensor2) == 0 || digitalRead(sensor3) == 0 || digitalRead(sensor4) == 0 || digitalRead(sensor5) == 0 || digitalRead(sensor6) == 0) {
    pressed = 1;
  }

  return pressed;
}

int isButtonPressed(int randomLed) {
  if (randomLed == 1) {
    buttonPressed = 2;
  }
  if (randomLed == 2) {
    buttonPressed = 3;
  }
  if (randomLed == 3) {
    buttonPressed = 4;
  }
  if (randomLed == 4) {
    buttonPressed = 5;
  }
  if (randomLed == 5) {
    buttonPressed = 6;
  }
  if (randomLed == 6) {
    buttonPressed = 7;
  }
  return buttonPressed;
}

/*=======================
======LED Functions======
=======================*/

void ledOn(int randomLed) {
  if (randomLed == 1) {
    digitalWrite(led1, HIGH);
  } else if (randomLed == 2) {
    digitalWrite(led2, HIGH);
  } else if (randomLed == 3) {
    digitalWrite(led3, HIGH);
  } else if (randomLed == 4) {
    digitalWrite(led4, HIGH);
  } else if (randomLed == 5) {
    digitalWrite(led5, HIGH);
  } else if (randomLed == 6) {
    digitalWrite(led6, HIGH);
  }
}

void ledOff(int randomLed) {
  if (randomLed == 1) {
    digitalWrite(led1, LOW);
  } else if (randomLed == 2) {
    digitalWrite(led2, LOW);
  } else if (randomLed == 3) {
    digitalWrite(led3, LOW);
  } else if (randomLed == 4) {
    digitalWrite(led4, LOW);
  } else if (randomLed == 5) {
    digitalWrite(led5, LOW);
  } else if (randomLed == 6) {
    digitalWrite(led6, LOW);
  }
}

void ledOnAll() {
  ledOn(1);
  ledOn(2);
  ledOn(3);
  ledOn(4);
  ledOn(5);
  ledOn(6);
}

void ledOffAll() {
  ledOff(1);
  ledOff(2);
  ledOff(3);
  ledOff(4);
  ledOff(5);
  ledOff(6);
}

void blinkAll(int blinkTime) {
  ledOnAll();
  delay(blinkTime);
  ledOffAll();
  delay(blinkTime);
}

void blink(int randomLed, int blinkTime) {
  ledOn(randomLed);
  delay(blinkTime);
  ledOff(randomLed);
  delay(blinkTime);
}

void ledRun(int speed) {
  ledOn(1);
  delay(speed);
  ledOn(2);
  delay(speed);
  ledOn(3);
  delay(speed);
  ledOn(4);
  delay(speed);
  ledOn(5);
  delay(speed);
  ledOn(6);
  delay(speed);
  ledOff(6);
  delay(speed);
  ledOff(5);
  delay(speed);
  ledOff(4);
  delay(speed);
  ledOff(3);
  delay(speed);
  ledOff(2);
  delay(speed);
  ledOff(1);
  delay(speed);
}

void Debug() {
  //debug mode: when each button is pressed, the respective button's LED light up
  while (i==0) {
    ledRun(200);
    i = 1;
  }
  if (digitalRead(sensor1)==0) {
    ledOn(1);
  }
  if (digitalRead(sensor2)==0) {
    ledOn(2);
  }
  if (digitalRead(sensor3)==0) {
    ledOn(3);
  }
  if (digitalRead(sensor4)==0) {
    ledOn(4);
  }
  if (digitalRead(sensor5)==0) {
    ledOn(5);
  }
  if (digitalRead(sensor6)==0) {
    ledOn(6);
  }
  ledOffAll();

  if (digitalRead(startButton)==0) { //to quit debug mode, hold startButton for 3 seconds
    timeout = millis();
    while (digitalRead(startButton)==0 && debugMode==1) {
      if ((millis() - timeout) >= 2500) {
        debugMode = 0;
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
      }
    }
    while (digitalRead(startButton)==0) {
    }
  }
}