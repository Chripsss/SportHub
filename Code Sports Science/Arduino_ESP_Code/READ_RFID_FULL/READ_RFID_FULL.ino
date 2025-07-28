#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN 5                  // Configurable, see typical pin layout above
#define SDA_PIN 53                  // Configurable, see typical pin layout above
MFRC522 mfrc522(SDA_PIN, RST_PIN);  // Create MFRC522 instance
void setup() {
  Serial.begin(115200);               // Initialize serial communications with the PC
  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
}
void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  // Dump debug info about the card; PICC_HaltA() is automatically called
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}