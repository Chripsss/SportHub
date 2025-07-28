#define led1 A5
#define led2 A4
#define led3 A3
#define led4 A2
#define led5 A1
#define led6 A0

#define sensor1 2
#define sensor2 3
#define sensor3 4
#define sensor4 5
#define sensor5 6
#define sensor6 7
#define startButton 13

int buttonPressed;
int right;
int wrong;
bool state;

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
int delayHelp = 100;

void setup() {

  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);

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

  while (keyPress() == 0) {
  }
  ledOffAll();
  while (digitalRead(startButton) == 0 || digitalRead(sensor1) == 0 || digitalRead(sensor2) == 0 || digitalRead(sensor3) == 0 || digitalRead(sensor4) == 0 || digitalRead(sensor5) == 0 || digitalRead(sensor6) == 0) {
  }

  // digitalWrite(buzzerPin, HIGH);
  // delay(1000);
  // digitalWrite(buzzerPin, LOW);
  // delay(500);

  // delay(10);  // 3
  // tone(buzzerPin, 2500, 100);
  // delay(1000);  // 2
  // tone(buzzerPin, 2500, 100);
  // delay(1000);  // 1
  // tone(buzzerPin, 2500, 500);
  // delay(1000);  // 0
  // noTone(buzzerPin);

  startGame = millis();
  while (((millis() - startGame) / 1000) < 20) {
    playGame();
  }

  for (int count = 0; count < dataIndex; count++) {
    // Serial.println(data[count], 3);
    fastValue = min(data[count], dummyFast);
    dummyFast = fastValue;
    slowValue = max(data[count], dummySlow);
    dummySlow = slowValue;
    sum = sum + data[count];
  }
  avg = sum / right;
  fastest = fastValue;
  slowest = slowValue;
  accuracy = ((right * 1.0) / (right * 1.0 + wrong * 1.0)) * 100.00;
  // Serial.print("Fastest: ");
  // Serial.println(fastest, 3);
  // Serial.print("Slowest: ");
  // Serial.println(slowest, 3);
  // Serial.print("Average: ");
  // Serial.println(avg, 3);
  // Serial.print("Data: ");
  // Serial.println(dataIndex);
  // Serial.print("Right: ");
  // Serial.println(right);
  // Serial.print("Wrong: ");
  // Serial.println(wrong);
  // Serial.print("Accuracy: ");
  // Serial.println(accuracy, 3);

  Serial.println("True  |   Fastest Press    |     Slowest Press    |     Average Time     |    Accuracy");
  Serial.println(" " + String(right) + "   |       " + String(fastest, 3) + "        |          " + String(slowest, 3) + "       |           " + String(avg, 3) + "      |      " + String(accuracy, 2));


  for (int count = 0; count < dataIndex; count++) {
    data[count] = 0;
  }
  dataIndex = 0;
  right = 0;
  wrong = 0;
  dummyFast = 10.00;
  dummySlow = 0.00;
  avg = 0;
  sum = 0;
  accuracy = 0;
  digitalWrite(A6, LOW);
  //data[200] = { 0 };
  ledOnAll();
  delay(2000);

  //ledRun(100);
}

void playGame() {
  if (state == 0) {
    state = 1;
    randomLed = random(1, 7);
    while (randomLed == oldRandomLed1) {
      randomLed = random(1, 7);
      while (randomLed == oldRandomLed2) {
        randomLed = random(1, 7);
      }
    }
  }

  oldRandomLed2 = oldRandomLed1;
  oldRandomLed1 = randomLed;
  realTime = millis();
  ledOn(randomLed);

  int buttonPressed = isButtonPressed(randomLed);
  while (digitalRead(sensor1) == 1 && digitalRead(sensor2) == 1 && digitalRead(sensor3) == 1 && digitalRead(sensor4) == 1 && digitalRead(sensor5) == 1 && digitalRead(sensor6) == 1) {
    //waiting to pressed
  }

  delay(delayHelp);

  if (digitalRead(buttonPressed) == 0 && state == 1) {
    right++;
    ledOff(randomLed);
    state = 0;
  } else if (digitalRead(buttonPressed) == 1 && state == 1) {
    wrong++;
  }
  realTime = (millis() - realTime) / 1000.00;
  data[dataIndex] = realTime;

  while (digitalRead(sensor1) == 0 || digitalRead(sensor2) == 0 || digitalRead(sensor3) == 0 || digitalRead(sensor4) == 0 || digitalRead(sensor5) == 0 || digitalRead(sensor6) == 0) {
    //while hold the button
  }


  delay(delayHelp);
  dataIndex++;
}

boolean keyPress() {

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

void spin(int speed) {
  blink(1, speed);
  blink(2, speed);
  blink(3, speed);
  blink(4, speed);
  blink(5, speed);
  blink(6, speed);
}

void boomerang(int speed) {
  blink(1, speed);
  blink(2, speed);
  blink(3, speed);
  blink(4, speed);
  blink(5, speed);
  blink(6, speed);
  blink(5, speed);
  blink(4, speed);
  blink(3, speed);
  blink(2, speed);
  blink(1, speed);
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