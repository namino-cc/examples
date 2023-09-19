#include "main.h"

unsigned long lastTagdRead = 0;
unsigned long lastLoop = 0;

// LCD 20x4
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x3F for a 20 chars and 4 line display

// RC522 Reader
MFRC522 mfrc522(SDA_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

// put function declarations here:

void setup() {

  // Serial Setup
  Serial.begin(115200);
  while (!Serial);

  // LCD Setup
  lcd.begin(20, 4);
  lcd.init();
  lcd.noBacklight();
  lcd.setCursor(0,0);
  Serial.println("Display Init Done");


  // SPI Setup
  Serial.println("RFID RC522 Test");
  Wire.begin();
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SDA_PIN);        // Init SPI bus
  // MFRC522 Setup
  mfrc522.PCD_Init(); // Init MFRC522 card
  lcd.print("Tap an RFID/NFC tag");
  Serial.println("NO TAG");
}

void loop() {
  unsigned long theTime = millis();
  char line[LINE_LEN + 1];

  // limit loop period
  if (abs( (long long) (theTime - lastLoop)) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;
  // Check RFID Presence
  if (mfrc522.PICC_IsNewCardPresent()) { // new tag is available
    lastTagdRead = theTime;
    if (mfrc522.PICC_ReadCardSerial()) { // NUID has been readed
      memset(line, 0, LINE_LEN);
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      strcat(line, "RFID/NFC Tag Type:");
      Serial.print(line);

      // Switch On LCD
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(0,0);
      lcd.print(line);
      lcd.setCursor(0,1);
      strncpy(line, (char *) mfrc522.PICC_GetTypeName(piccType), LINE_LEN);
      Serial.println(line);
      lcd.print(line);

      // Get Tag ID
      strcpy(line, "UID:");
      Serial.print(line);
      for (int i = 0; i < mfrc522.uid.size; i++) {
        char byteStr[5];
        sprintf(byteStr, " %02X", (int) (mfrc522.uid.uidByte[i]) ) ;
        Serial.print(byteStr);
        strcat(line, byteStr);
      }
      Serial.println();
      lcd.setCursor(0,2);
      lcd.print(line);
      mfrc522.PICC_HaltA(); // halt PICC
      mfrc522.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
  // Switch Off LCD
  if (lastTagdRead && theTime - lastTagdRead > SCREEN_ON)  {
    lcd.noBacklight();
    lastTagdRead = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tap an RFID/NFC tag");
    lcd.setCursor(0,1);
    lcd.print("NO TAG");
  }
}


