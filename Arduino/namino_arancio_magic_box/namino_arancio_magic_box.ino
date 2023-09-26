#include <namino_rosso.h>
#include <SPI.h>

#include <MFRC522.h>

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

#define LOOP_PERIOD   500           // 500ms loop period
// #define SCREEN_ON    8000           //  s of screen on time
#define LINE_LEN    20              // Line Buffer Size

// Namino Rosso interface instance
// namino_rosso nr = namino_rosso();

// SPI Interface
#define VSPI FSPI
SPIClass spi;


// RC522 Reader
#define MFRC522_SPICLOCK (800000U)	// MFRC522 accept upto 10MHz, set to 800KHz.
#define RST_PIN       (18)
#define MOSI_PIN      MOSI        //11
#define MISO_PIN      MISO        // 13
#define CLK_PIN       SCK         // 12
#define CS_PIN        CS_SDCARD   //2
#define NM_SS         (10)



MFRC522 mfrc522(CS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

// Loop Params
unsigned long lastTagdRead = 0;
unsigned long lastLoop = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.println("");
  Serial.println("=====================================");
  Serial.println("##      RFID MAGIC BOX EXAMPLE     ##");
  Serial.println("=====================================");
  Serial.println();
  Serial.print("MOSI: ");
  Serial.println(MOSI);
  Serial.print("MISO: ");
  Serial.println(MISO);
  Serial.print("SCK: ");
  Serial.println(SCK);
  Serial.print("CS: ");
  Serial.println(CS_SDCARD);  
  delay(2000);
  // TODO: Start Mamino Industrial Interface
  // Namino Industrial Interface start
  // Serial.println("Namino Rosso Interface starting");
  // reset namino microcontroller. Industrial side board reset.
  // nr.resetSignalMicroprocessor();
  // Openin communication of the industrial side interface
  // nr.begin(800000U, MISO, MOSI, SCK, SS);
  // delay(2000);
  // nr.readAllRegister();

  // while (not nr.isReady())  {
  //    delay(200);
  //    Serial.print(".");
  // }
  // Serial.println("\nNamino Rosso Interface started!");
  // MFRC522 Setup
  // Wire.begin();
  // disable on SPI bus
  // pinMode(NM_SS, OUTPUT);
  // digitalWrite(NM_SS, HIGH);

  // CS Reader Pin
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  spi = SPIClass(VSPI);
  spi.begin(CLK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);        // Init SPI bus
  Serial.println("RFID RC522 Test");
  digitalWrite(CS_PIN, LOW);
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
  // nr.readAllRegister();
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
