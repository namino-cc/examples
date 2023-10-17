#include "main.h"

uint32_t      lastTagdRead = 0;
uint32_t      lastLoop = 0;
bool          readerOk = false;

// LCD 20x4
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x3F for a 20 chars and 4 line display

// RC522 Reader
MFRC522 mfrc522(CS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

// put function declarations here:

void setup() {

  // Serial Setup
  Serial.begin(115200);
  while (!Serial);
  Serial.println("");
  Serial.println("=====================================");
  Serial.println("##        RFID   READ  EXAMPLE     ##");
  Serial.println("=====================================");
  Serial.println();
  Serial.print("RFID SPI MOSI: ");
  Serial.println(MOSI_PIN);
  Serial.print("RFID SPI MISO: ");
  Serial.println(MISO_PIN);
  Serial.print("RFID SPI SCK: ");
  Serial.println(SCK_PIN);
  Serial.print("RFID SPI CS: ");
  Serial.println(CS_PIN);  
  Serial.print("RFID SPI RST: ");
  Serial.println(RST_PIN);  
  Serial.println("-------------------");
  Serial.flush();

  // LCD on i2C Setup
  Wire.begin(SDA, SCL);
  lcd.begin(20, 4);
  lcd.init();
  lcd.noBacklight();
  lcd.setCursor(0,0);
  Serial.println("Display Init Done");

  // SPI Setup (Custom Pins other than J3 on Namino boards)
  Serial.println("Starting SPI Interface");
  // MFRC522 Setup
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  mfrc522.PCD_Init(); // Init MFRC522 card
  delay(2000);

  // Testing MFRC522 Reader
  if (mfrc522.PCD_PerformSelfTest())  {
    Serial.printf("Antenna Gain: %02X\n", mfrc522.PCD_GetAntennaGain());
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    mfrc522.PCD_AntennaOn();
    Serial.printf("Antenna Gain set to Max: %02X\n", mfrc522.PCD_GetAntennaGain());
    Serial.println("Tap an RFID/NFC tag");
    Serial.println("NO TAG");
    lcd.print("Tap an RFID/NFC tag");
    readerOk = true;
  }
  else  {
    Serial.println("Error in Reader Self Test");
    lcd.print("No Reader Found!");
  }
}

void loop() {
  uint32_t    theTime = millis();
  char        line[LINE_LEN + 1];

  // limit loop period
  if ((theTime - lastLoop) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;
  // Check RFID Reader
  if (readerOk)  {
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
    else  {
      Serial.print(".");
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
  else  {
    Serial.println("No Reader Found!");
  }
}


