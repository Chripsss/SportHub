#define buzzerPin 4
#define button 12

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  //Serial.println(digitalRead(button));

  // digitalWrite(buzzerPin, HIGH);
  // delay(300);
  // digitalWrite(buzzerPin, LOW);
  // delay(900);

  // digitalWrite(buzzerPin, HIGH);
  // delay(300);
  // digitalWrite(buzzerPin, LOW);
  // delay(900);


  // digitalWrite(buzzerPin, HIGH);
  // delay(300);
  // digitalWrite(buzzerPin, LOW);
  // delay(900);


  // digitalWrite(buzzerPin, HIGH);
  // delay(1000);
  // digitalWrite(buzzerPin, LOW);
  //delay(500);
}


void loop() {
  Serial.println(digitalRead(button));
  if (digitalRead(button) == 0) {
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(800);

    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(800);

    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(800);

    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }
}