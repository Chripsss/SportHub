#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <MFRC522.h>

#define SDA_DIO 49
#define RESET_DIO 48
MFRC522 RC522(SDA_DIO, RESET_DIO);

#define led1 2
#define led1Num 24
#define led2 3
#define led2Num 24
#define led3 4
#define led3Num 24
#define led4 5
#define led4Num 24
#define LED_RFID A15

#define sensor1 54
#define sensor2 55
#define sensor3 56
#define sensor4 57
#define startButton A14
#define buzzerPin 12

int buttonPressed;
int right;
int wrong;
bool state;

float realTime;
float fastest;
float slowest;
float dummyFast = 10;
float dummySlow = 0;
float fastValue;
float slowValue;
float avg;
float sum = 0;
float accuracy;

float data[200] = { 0 };  //an array capable of holding 200 entries numbered 0 to 199
float dataRight[200] = { 0 };
int dataIndex = 0;

unsigned long startGame;

int randomLed;
int oldRandomLed1;
int oldRandomLed2;

bool ledState1 = 0;
bool ledState2 = 0;
bool ledState3 = 0;
bool ledState4 = 0;

bool ttonState1;
bool buttonState2;
bool buttonState3;
bool buttonState4;

bool pressed = 0;
bool buttonState = 0;
bool state_buzzer = 0;
int rightNum = 0;
int duration = 30;

const int blinkingTime = 1000;
const int changeDelay = 100;

String uid = "";
bool state_RFID = 0;

Adafruit_NeoPixel ledRGB1(led1Num, led1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledRGB2(led2Num, led2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledRGB3(led3Num, led3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledRGB4(led4Num, led4, NEO_GRB + NEO_KHZ800);

float reactionTime;

int param1 = 0;
int param2 = 0;
int param3 = 0;
int param4 = 0;
long value1 = 0, value2 = 0, value3 = 0, value4 = 0;
int param;

int wallHit;

void ledLaunch() {
  ledRGB1.begin();
  ledRGB1.show();
  ledRGB2.begin();
  ledRGB2.show();
  ledRGB3.begin();
  ledRGB3.show();
  ledRGB4.begin();
  ledRGB4.show();
}

void ledShowAll() {
  ledRGB1.show();
  ledRGB2.show();
  ledRGB3.show();
  ledRGB4.show();
}

void ledBrightness() {
  int brignessLevel = 100;
  ledRGB1.setBrightness(brignessLevel);
  ledRGB2.setBrightness(brignessLevel);
  ledRGB3.setBrightness(brignessLevel);
  ledRGB4.setBrightness(brignessLevel);
  ledShowAll();
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  RC522.PCD_Init();

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(LED_RFID, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(startButton, INPUT_PULLUP);

  ledLaunch();
  ledBrightness();

  randomSeed(analogRead(44));
}

void loop() {

  while (keyPress() == 0) {
  }
  ledOffAll();

  while (digitalRead(startButton) == 1) {
    blinkBlue(200);
  }

  while (digitalRead(startButton) == 0) {
  }

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

  ledOffAll();

  startGame = millis();
  while (((millis() - startGame) / 1000) < 30) {
    playGame();
  }

  for (int count = 0; count < right; count++) {

    fastValue = min(dataRight[count], dummyFast);
    dummyFast = fastValue;
    slowValue = max(dataRight[count], dummySlow);
    dummySlow = slowValue;
    sum = sum + dataRight[count];
  }

  avg = sum / right;
  fastest = fastValue;
  slowest = slowValue;
  accuracy = ((right * 1.0) / (right * 1.0 + wrong * 1.0)) * 100.00;

  Serial.println(String(uid) + "," + String(right) + "," + String(fastest, 3) + "," + String(slowest, 3) + "," + String(avg, 3) + "," + String(accuracy, 2));

  for (int count = 0; count < right; count++) {
    dataRight[count] = 0;
  }
  dataIndex = 0;
  right = 0;
  wrong = 0;
  dummyFast = 10.00;
  dummySlow = 0.00;
  avg = 0;
  sum = 0;
  accuracy = 0;

  uid = "";
  state_RFID = 0;

  ledOffAll();
  spinGreenOn(10);
  spinBlueR(10);
  delay(2000);
}

void setParam() {
  int samples = 4;
  for (int i = 0; i < samples; i++) {
    value1 += analogRead(sensor1);
    value2 += analogRead(sensor2);
    value3 += analogRead(sensor3);
    value4 += analogRead(sensor4);
  }

  value1 /= samples;
  value2 /= samples;
  value3 /= samples;
  value4 /= samples;
  int const add = -50;
  param1 = value1 + add;
  param2 = value2 + add;
  param3 = value3 + add;
  param4 = value4 + add;
}

void sensorCheck() {
  wallHit = 0;
  setParam();
  while (wallHit == 0) {
    if (analogRead(sensor1) > param1) {
      wallHit = sensor1;
    }
    if (analogRead(sensor2) > param2) {
      wallHit = sensor2;
    }
    if (analogRead(sensor3) > param3) {
      wallHit = sensor3;
    }
    if (analogRead(sensor4) > param4) {
      wallHit = sensor4;
    }
  }
  Serial.println(wallHit);
}

void playGame() {
  if (state == 0) {
    state = 1;
    randomLed = random(1, 5);
    while (randomLed == oldRandomLed1) {
      randomLed = random(1, 5);
    }
  }
  oldRandomLed1 = randomLed;
  realTime = millis();
  ledOn(randomLed);
  isButtonPressed(randomLed);
  setParam();

  while (state == 1) {
    sensorCheck();
    
    if (wallHit == buttonPressed) {
      digitalWrite(buzzerPin, HIGH);
      reset();
      ledRedHelp(1, 0);
      ledRedHelp(2, 0);
      ledRedHelp(3, 0);
      ledRedHelp(4, 0);
      state = 0;
      reactionTime = (millis() - realTime) / 1000.00;
      dataRight[right] = reactionTime;
      right++;
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
    }

    else {
      wrong++;
    }
    delay(100);
    dataIndex++;
  }
}

int isButtonPressed(int randomLed) {
  if (randomLed == 1) {
    buttonPressed = sensor1;
    param = param1;
  }
  if (randomLed == 2) {
    buttonPressed = sensor2;
    param = param2;
  }
  if (randomLed == 3) {
    buttonPressed = sensor3;
    param = param3;
  }
  if (randomLed == 4) {
    buttonPressed = sensor4;
    param = param4;
  }

  return buttonPressed;
  return param;
}

void ledOn(int randomLed) {
  if (randomLed == 1) {
    ledGreen(1, 0);
    ledRedHelp(2, 0);
    ledRedHelp(3, 0);
    ledRedHelp(4, 0);
  } else if (randomLed == 2) {
    ledGreen(2, 0);
    ledRedHelp(1, 0);
    ledRedHelp(3, 0);
    ledRedHelp(4, 0);
  } else if (randomLed == 3) {
    ledGreen(3, 0);
    ledRedHelp(1, 0);
    ledRedHelp(2, 0);
    ledRedHelp(4, 0);
  } else if (randomLed == 4) {
    ledGreen(4, 0);
    ledRedHelp(1, 0);
    ledRedHelp(2, 0);
    ledRedHelp(3, 0);
  }
}

void ledOffAll() {
  ledRGB1.clear();
  ledRGB1.show();
  ledRGB2.clear();
  ledRGB2.show();
  ledRGB3.clear();
  ledRGB3.show();
  ledRGB4.clear();
  ledRGB4.show();
  digitalWrite(LED_RFID, LOW);
}

void ledOff(int randomLed) {
  if (randomLed == 1) {
    ledRGB1.clear();
    ledRGB1.show();
  } else if (randomLed == 2) {
    ledRGB2.clear();
    ledRGB2.show();
  } else if (randomLed == 3) {
    ledRGB3.clear();
    ledRGB3.show();
  } else if (randomLed == 4) {
    ledRGB4.clear();
    ledRGB4.show();
  }
}

void ledRed(int randomLed, int pixelDelay) {
  if (randomLed == 1) {
    ledState1 = 1;
    for (int pixel1 = 0; pixel1 < led1Num; pixel1++) {
      ledRGB1.setPixelColor(pixel1, ledRGB1.Color(255, 0, 0));
      ledRGB1.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 2) {
    ledState2 = 1;
    for (int pixel2 = 0; pixel2 < led2Num; pixel2++) {
      ledRGB2.setPixelColor(pixel2, ledRGB2.Color(255, 0, 0));
      ledRGB2.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 3) {
    ledState3 = 1;
    for (int pixel3 = 0; pixel3 < led3Num; pixel3++) {
      ledRGB3.setPixelColor(pixel3, ledRGB3.Color(255, 0, 0));
      ledRGB3.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 4) {
    ledState4 = 1;
    for (int pixel4 = 0; pixel4 < led4Num; pixel4++) {
      ledRGB4.setPixelColor(pixel4, ledRGB4.Color(255, 0, 0));
      ledRGB4.show();
      delay(pixelDelay);
    }
  }
}

void ledRedHelp(int randomLed, int pixelDelay) {
  if (randomLed == 1) {
    for (int pixel1 = 0; pixel1 < led1Num; pixel1++) {
      ledRGB1.setPixelColor(pixel1, ledRGB1.Color(255, 0, 0));
      ledRGB1.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 2) {
    for (int pixel2 = 0; pixel2 < led2Num; pixel2++) {
      ledRGB2.setPixelColor(pixel2, ledRGB2.Color(255, 0, 0));
      ledRGB2.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 3) {
    for (int pixel3 = 0; pixel3 < led3Num; pixel3++) {
      ledRGB3.setPixelColor(pixel3, ledRGB3.Color(255, 0, 0));
      ledRGB3.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 4) {
    for (int pixel4 = 0; pixel4 < led4Num; pixel4++) {
      ledRGB4.setPixelColor(pixel4, ledRGB4.Color(255, 0, 0));
      ledRGB4.show();
      delay(pixelDelay);
    }
  }
}

void ledGreen(int randomLed, int pixelDelay) {
  if (randomLed == 1) {
    ledState1 = 1;
    for (int pixel1 = 0; pixel1 < led1Num; pixel1++) {
      ledRGB1.setPixelColor(pixel1, ledRGB1.Color(0, 255, 0));
      ledRGB1.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 2) {
    ledState2 = 1;
    for (int pixel2 = 0; pixel2 < led2Num; pixel2++) {
      ledRGB2.setPixelColor(pixel2, ledRGB2.Color(0, 255, 0));
      ledRGB2.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 3) {
    ledState3 = 1;
    for (int pixel3 = 0; pixel3 < led3Num; pixel3++) {
      ledRGB3.setPixelColor(pixel3, ledRGB3.Color(0, 255, 0));
      ledRGB3.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 4) {
    ledState4 = 1;
    for (int pixel4 = 0; pixel4 < led4Num; pixel4++) {
      ledRGB4.setPixelColor(pixel4, ledRGB4.Color(0, 255, 0));
      ledRGB4.show();
      delay(pixelDelay);
    }
  }
}

void ledGreenHelp(int randomLed, int pixelDelay) {
  if (randomLed == 1) {
    for (int pixel1 = 0; pixel1 < led1Num; pixel1++) {
      ledRGB1.setPixelColor(pixel1, ledRGB1.Color(0, 255, 0));
      ledRGB1.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 2) {
    for (int pixel2 = 0; pixel2 < led2Num; pixel2++) {
      ledRGB2.setPixelColor(pixel2, ledRGB2.Color(0, 255, 0));
      ledRGB2.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 3) {
    for (int pixel3 = 0; pixel3 < led3Num; pixel3++) {
      ledRGB3.setPixelColor(pixel3, ledRGB3.Color(0, 255, 0));
      ledRGB3.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 4) {
    for (int pixel4 = 0; pixel4 < led4Num; pixel4++) {
      ledRGB4.setPixelColor(pixel4, ledRGB4.Color(0, 255, 0));
      ledRGB4.show();
      delay(pixelDelay);
    }
  }
}

void ledBlue(int randomLed, int pixelDelay) {
  if (randomLed == 1) {
    ledState1 = 1;
    for (int pixel1 = 0; pixel1 < led1Num; pixel1++) {
      ledRGB1.setPixelColor(pixel1, ledRGB1.Color(0, 0, 255));
      ledRGB1.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 2) {
    ledState2 = 1;
    for (int pixel2 = 0; pixel2 < led2Num; pixel2++) {
      ledRGB2.setPixelColor(pixel2, ledRGB2.Color(0, 0, 255));
      ledRGB2.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 3) {
    ledState3 = 1;
    for (int pixel3 = 0; pixel3 < led3Num; pixel3++) {
      ledRGB3.setPixelColor(pixel3, ledRGB3.Color(0, 0, 255));
      ledRGB3.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 4) {
    ledState4 = 1;
    for (int pixel4 = 0; pixel4 < led4Num; pixel4++) {
      ledRGB4.setPixelColor(pixel4, ledRGB4.Color(0, 0, 255));
      ledRGB4.show();
      delay(pixelDelay);
    }
  }
}

void ledBlueHelp(int randomLed, int pixelDelay) {
  if (randomLed == 1) {
    for (int pixel1 = 0; pixel1 < led1Num; pixel1++) {
      ledRGB1.setPixelColor(pixel1, ledRGB1.Color(0, 0, 255));
      ledRGB1.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 2) {
    for (int pixel2 = 0; pixel2 < led2Num; pixel2++) {
      ledRGB2.setPixelColor(pixel2, ledRGB2.Color(0, 0, 255));
      ledRGB2.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 3) {
    for (int pixel3 = 0; pixel3 < led3Num; pixel3++) {
      ledRGB3.setPixelColor(pixel3, ledRGB3.Color(0, 0, 255));
      ledRGB3.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 4) {
    for (int pixel4 = 0; pixel4 < led4Num; pixel4++) {
      ledRGB4.setPixelColor(pixel4, ledRGB4.Color(0, 0, 255));
      ledRGB4.show();
      delay(pixelDelay);
    }
  }
}

void ledRedAll() {
  ledRed(1, 0);
  ledRed(2, 0);
  ledRed(3, 0);
  ledRed(4, 0);
}

void ledGreenAll() {
  ledGreen(1, 0);
  ledGreen(2, 0);
  ledGreen(3, 0);
  ledGreen(4, 0);
}

void ledBlueAll() {
  ledBlue(1, 0);
  ledBlue(2, 0);
  ledBlue(3, 0);
  ledBlue(4, 0);
}

void blinkRed(int blinkTime) {
  ledRedAll();
  delay(blinkTime);
  ledOffAll();
  delay(blinkTime);
}

void blinkGreen(int blinkTime) {
  ledGreenAll();
  delay(blinkTime);
  ledOffAll();
  delay(blinkTime);
}

void blinkBlue(int blinkTime) {
  ledBlueAll();
  delay(blinkTime);
  ledOffAll();
  delay(blinkTime);
}

void spinRed(int pixelDelay) {
  ledRed(1, pixelDelay);
  ledOff(1);
  ledRed(2, pixelDelay);
  ledOff(2);
  ledRed(3, pixelDelay);
  ledOff(3);
  ledRed(4, pixelDelay);
  ledOff(4);
}

void spinRedOn(int pixelDelay) {
  ledRed(1, pixelDelay);
  ledRed(2, pixelDelay);
  ledRed(3, pixelDelay);
  ledRed(4, pixelDelay);
}

void spinGreen(int pixelDelay) {
  ledGreen(1, pixelDelay);
  ledOff(1);
  ledGreen(2, pixelDelay);
  ledOff(2);
  ledGreen(3, pixelDelay);
  ledOff(3);
  ledGreen(4, pixelDelay);
  ledOff(4);
}

void spinGreenOn(int pixelDelay) {
  ledGreen(1, pixelDelay);
  ledGreen(2, pixelDelay);
  ledGreen(3, pixelDelay);
  ledGreen(4, pixelDelay);
}

void spinBlue(int pixelDelay) {
  ledBlue(1, pixelDelay);
  ledOff(1);
  ledBlue(2, pixelDelay);
  ledOff(2);
  ledBlue(3, pixelDelay);
  ledOff(3);
  ledBlue(4, pixelDelay);
  ledOff(4);
}

void spinBlueOn(int pixelDelay) {
  ledBlue(1, pixelDelay);
  ledBlue(2, pixelDelay);
  ledBlue(3, pixelDelay);
  ledBlue(4, pixelDelay);
}

void spinBlueR(int pixelDelay) {
  ledBlueR(4, pixelDelay);
  ledBlueR(3, pixelDelay);
  ledBlueR(2, pixelDelay);
  ledBlueR(1, pixelDelay);
}

void ledBlueR(int randomLed, int pixelDelay) {
  if (randomLed == 1) {
    ledState1 = 1;
    for (int pixel1 = led1Num; pixel1 >= 0; pixel1--) {
      ledRGB1.setPixelColor(pixel1, ledRGB1.Color(0, 0, 255));
      ledRGB1.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 2) {
    ledState2 = 1;
    for (int pixel2 = led2Num; pixel2 >= 0; pixel2--) {
      ledRGB2.setPixelColor(pixel2, ledRGB2.Color(0, 0, 255));
      ledRGB2.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 3) {
    ledState3 = 1;
    for (int pixel3 = led3Num; pixel3 >= 0; pixel3--) {
      ledRGB3.setPixelColor(pixel3, ledRGB3.Color(0, 0, 255));
      ledRGB3.show();
      delay(pixelDelay);
    }
  } else if (randomLed == 4) {
    ledState4 = 1;
    for (int pixel4 = led4Num; pixel4 >= 0; pixel4--) {
      ledRGB4.setPixelColor(pixel4, ledRGB4.Color(0, 0, 255));
      ledRGB4.show();
      delay(pixelDelay);
    }
  }
}

void reset() {
  ledState1 = 0;
  ledState2 = 0;
  ledState3 = 0;
  ledState4 = 0;
}

boolean keyPress() {
  while (state_RFID == 0) {
    blinkBlue(200);
    if (state_RFID == 0 && RC522.PICC_IsNewCardPresent() && RC522.PICC_ReadCardSerial()) {
      for (byte i = 0; i < RC522.uid.size; i++) {
        uid += String(RC522.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(RC522.uid.uidByte[i], HEX);
      }
      uid.toUpperCase();
      RC522.PICC_HaltA();
    }
    if (uid.length() > 0) {
      state_RFID = 1;
      digitalWrite(LED_RFID, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(300);
      digitalWrite(buzzerPin, LOW);
    }
  }
  blinkBlue(250);
  boolean pressed = 0;

  if (digitalRead(startButton) == 0) {
    pressed = 1;
  }

  return pressed;
}