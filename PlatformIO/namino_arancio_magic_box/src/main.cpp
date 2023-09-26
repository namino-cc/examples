#include "namino_arancio.h"
#include <Wire.h>
#include <MFRC522.h>



#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

#define CS_PIN   16
#define RST_PIN  18
#define SCK_PIN  14
#define MISO_PIN 17
#define MOSI_PIN  9

MFRC522 mfrc522(CS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

// Loop interval
#define LOOP_PERIOD   500           // 500ms loop period
#define LINE_LEN    20              // Line Buffer Size

unsigned long lastTagdRead = 0;
unsigned long lastLoop = 0;
bool          readerOk = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("");
  Serial.println("=====================================");
  Serial.println("##      RFID MAGIC BOX EXAMPLE     ##");
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
  delay(2000);


  // Starting custom SPI
  Serial.println("Starting SPI Interface");
  // SPI Setup (Custom Pins other than J3 on Namino boards)
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  // MFRC522 Setup
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
    readerOk = true;
  }
  else  {
    Serial.println("Error in Reader Self Test");
  }
}

void loop() {
  unsigned long theTime = millis();
  char line[LINE_LEN + 1];

  // limit loop period
  if (abs( (long long) (theTime - lastLoop)) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;

  // Check RFID Reader
  if (readerOk)  {
    // Check RFID Presence
    if(mfrc522.PICC_IsNewCardPresent()) { 
      // new tag is available
      lastTagdRead = theTime;
      Serial.println();
      if (mfrc522.PICC_ReadCardSerial()) { // NUID has been readed
        memset(line, 0, LINE_LEN);
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        strcat(line, "RFID/NFC Tag Type:");
        Serial.print(line);

        strncpy(line, (char *) mfrc522.PICC_GetTypeName(piccType), LINE_LEN);
        Serial.println(line);

        // Get Tag ID
        strcpy(line, "UID:");
        Serial.print(line);
        for (int i = 0; i < mfrc522.uid.size; i++) {
          // Build Tag ID String
          char byteStr[5];
          sprintf(byteStr, " %02X", (int) (mfrc522.uid.uidByte[i]) ) ;
          strcat(line, byteStr);
        }
        Serial.println(line);
        mfrc522.PICC_HaltA(); // halt PICC
        mfrc522.PCD_StopCrypto1(); // stop encryption on PCD
      }
    }
    else  {
      Serial.print(".");
    }
  }
  else  {
    Serial.println("No Reader Found!");
  }
}

