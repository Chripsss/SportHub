#include <SPI.h>
#include <MFRC522.h>

#define SDA_DIO 53
#define RESET_DIO 5

MFRC522 RC522(SDA_DIO, RESET_DIO);

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  RC522.PCD_Init();
}

void loop()
{
  if (RC522.PICC_IsNewCardPresent() && RC522.PICC_ReadCardSerial())
  {
    String uid = "";

    for (byte i = 0; i < RC522.uid.size; i++)
    {
      uid += String(RC522.uid.uidByte[i] < 0x10 ? "0" : "");
      uid += String(RC522.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();   // Convert to uppercase

    Serial.println(uid);

    // Send UID to ESP8266
    Serial1.println(uid);

    RC522.PICC_HaltA(); // Halt PICC operation
    RC522.PCD_StopCrypto1(); // Stop encryption on PCD

    delay(2000); // Wait for 2 seconds before detecting the next card
  }
}