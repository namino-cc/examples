#include <namino_rosso.h>
#include <MFRC522.h>

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

#define LOOP_PERIOD   500           // 500ms loop period
// #define SCREEN_ON    8000           //  s of screen on time
#define LINE_LEN    20              // Line Buffer Size

// Namino Rosso interface instance
namino_rosso nr = namino_rosso();

// RC522 Reader
#define RST_PIN 18
MFRC522 mfrc522(SS, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

// Loop Params
unsigned long lastTagdRead = 0;
unsigned long lastLoop = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.println("");
  // Namino Industrial Interface start
  Serial.println("Namino Rosso Interface starting");
  // reset namino microcontroller. Industrial side board reset.
  nr.resetSignalMicroprocessor();
  // Openin communication of the industrial side interface
  nr.begin(800000U, MISO, MOSI, SCK, SS);
  delay(2000);
  nr.readAllRegister();

  // while (not nr.isReady())  {
  //    delay(200);
  //    Serial.print(".");
  // }
  Serial.println("\nNamino Rosso Interface started!");
  // MFRC522 Setup
  Serial.println("RFID RC522 Test");
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Tap an RFID/NFC tag");
  Serial.println("NO TAG");
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long theTime = millis();
  char line[LINE_LEN + 1];

  // limit loop period
  if (abs( (long long) (theTime - lastLoop)) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;
  nr.readAllRegister();
  // Check RFID Presence
  if (mfrc522.PICC_IsNewCardPresent()) { // new tag is available
    lastTagdRead = theTime;
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
        char byteStr[5];
        sprintf(byteStr, " %02X", (int) (mfrc522.uid.uidByte[i]) ) ;
        Serial.print(byteStr);
        strcat(line, byteStr);
      }
      Serial.println();
      mfrc522.PICC_HaltA(); // halt PICC
      mfrc522.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}
