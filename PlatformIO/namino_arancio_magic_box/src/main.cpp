#include "namino_arancio.h"
#include <Wire.h>
#include <MFRC522.h>

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

#include "main.h"

// Namino Arancio Instance
namino_arancio  na = namino_arancio();
bool            configANIN = true;
bool            naminoReady = false;

// RC522 Reader Instance
MFRC522 mfrc522(CS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

// Time counters (in millis)
unsigned long lastTagdRead = 0;
unsigned long lastLoop = 0;
unsigned long lastDoorOpened = 0;
unsigned long startBlinkTime = 0;
unsigned long startResultTime = 0;

// Status Flags
bool          readerOk = false;
bool          blinkOn = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);

  // Init Namino Arancio Board
  // reset namino microcontroller
  na.resetSignalMicroprocessor();
  na.begin(800000U, MISO, MOSI, SCK, SS);
  delay(2000);

  Serial.println("");
  Serial.println("=====================================");
  Serial.println("##      RFID MAGIC BOX EXAMPLE     ##");
  Serial.println("=====================================");
  Serial.println();
  Serial.print("SPI MOSI: ");
  Serial.println(MOSI);
  Serial.print("SPI MISO: ");
  Serial.println(MISO);
  Serial.print("SPI SCK: ");
  Serial.println(SCK);
  Serial.print("RFID SPI CS: ");
  Serial.println(CS_PIN);  
  Serial.print("RFID SPI RST: ");
  Serial.println(RST_PIN);  
  Serial.println("-------------------");
  Serial.flush();
  delay(2000);

  // I/O Pins
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(KEYLOCK_IN, INPUT);
  digitalWrite(GREEN_PIN, HIGH);    
  digitalWrite(RED_PIN, HIGH);    
  digitalWrite(YELLOW_PIN, HIGH);    

  // Starting custom SPI
  // SPI Setup (Custom Pins other than J3 on Namino boards)
  // SPI.begin(SCK, MISO_PIN, MOSI, CS_PIN);

  // MFRC522 Setup
  Serial.println("Starting RFID Reader");
  mfrc522.PCD_Init(); // Init MFRC522 card
  delay(4000);



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
    readerOk = false;
    Serial.println("Error in Reader Self Test");
  }
  // Force Reader Ok also if test failed
  readerOk = true;
  // Starting
  delay(2000);
  digitalWrite(GREEN_PIN, LOW);    
  digitalWrite(RED_PIN, LOW);    
  digitalWrite(YELLOW_PIN, LOW);    
}

void loop() {
  unsigned long theTime = millis();
  char line[LINE_LEN + 1];
  uint64_t    tagIDLow = 0;
  uint64_t    tagIDHigh = 0;
  byte        tagID[TAG_LEN];

  // Read Industrial Registers
  na.readAllRegister();
  naminoReady = false;
  naminoReady = na.isReady();

  // Check Analog In Configuration
  if (configANIN && naminoReady) {
    // Configure analog Input (not used at the moment)  
    na.writeRegister(WR_ANALOG_IN_CH01_CONF, ANALOG_IN_CH01_CONF_VALUES::CH01_VOLTAGE);
    na.writeRegister(WR_ANALOG_IN_CH02_CONF, ANALOG_IN_CH02_CONF_VALUES::CH02_VOLTAGE);  
    na.writeRegister(WR_ANALOG_IN_CH03_CONF, ANALOG_IN_CH03_CONF_VALUES::CH03_VOLTAGE);
    na.writeRegister(WR_ANALOG_IN_CH04_CONF, ANALOG_IN_CH04_CONF_VALUES::CH04_VOLTAGE);
    na.writeRegister(WR_ANALOG_OUT_CH01_CONF, ANALOG_OUT_CH01_CONF_VALUES::OUT_CH01_VOLTAGE);
    na.writeAnalogOut(0.0); // output voltage
    Serial.println("NR config completed");
    Serial.printf("fwVersion: 0x%04x boardType: 0x%04x\n", na.fwVersion(), na.boardType());
    configANIN = false;
  }

  // limit loop period
  if (abs( (long long) (theTime - lastLoop)) < LOOP_PERIOD)  {
    goto endLoop;
  }
  lastLoop = theTime;

  if (naminoReady)  {
    // Reading Keylock Status and setting Keylock Light
    if (na.readDigIn(KEYLOCK_IN))  {
      na.writeDigOut(WHITE_LIGHT, true);
    }
    else  {
      na.writeDigOut(WHITE_LIGHT, false);
    }
  }

  // Check RFID Reader
  if (not readerOk)  {
    // Serial.println("No Reader Found!");
    goto endLoop;
  }

  // Switch Off Keylock
  if (lastDoorOpened > 0 && KEYLOCK_ON_TIME < (theTime - lastDoorOpened) )  {
    setKeyLock(false);
    lastDoorOpened = 0;
  }

  // Led Blinking
  if (startBlinkTime > 0)  {
      if (BLINK_ON_TIME < (theTime - startBlinkTime))  {
        // Definitely stops the blinking
        setBlink(false);
        blinkOn = false;
        startBlinkTime = 0;
      }
      else  {
        blinkOn = setBlink(blinkOn);
      }
  }

  // Result Display
  if (startResultTime > 0)  {
    if (RESULT_TIME < (theTime - startResultTime))  {
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(RED_PIN, LOW);
      startResultTime = 0;
    }
  }
  // Check RFID Presence only if ended result time
  if(startBlinkTime == 0 && mfrc522.PICC_IsNewCardPresent()) { 
    // new tag is available
    lastTagdRead = theTime;
    Serial.println();
    if (mfrc522.PICC_ReadCardSerial()) { // NUID has been readed
      memset(line, 0, LINE_LEN);
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      strcat(line, "RFID/NFC Tag Type: ");
      Serial.print(line);

      strncpy(line, (char *) mfrc522.PICC_GetTypeName(piccType), LINE_LEN);
      Serial.println(line);

      // Get Tag ID
      strcpy(line, "UID:");
      Serial.print(line);
      // UUID may be 4, 7 or 10 Bytes
      memset(&(tagID[0]), 0, TAG_LEN);
      tagIDLow = 0;
      tagIDHigh = 0;
      for (int i = 0; i < mfrc522.uid.size; i++) {
        // Build Tag ID String
        char byteStr[5];
        sprintf(byteStr, " %02X", (int) (mfrc522.uid.uidByte[i]) ) ;
        tagID[i] = mfrc522.uid.uidByte[i];
        strcat(line, byteStr);
      }
      // Can copy read value byte per byte as the platform is "Little Endian"
      memcpy(&tagIDLow, &(tagID[0]), sizeof(tagIDLow));
      if (mfrc522.uid.size > 8)  {
        memcpy(&tagIDHigh, &(tagID[sizeof(tagIDLow)]), sizeof(tagIDHigh));
      }
      else  {
        tagIDHigh = 0;
      }
      Serial.printf("[%s] Len:[%d] Low:[%u] - High:[%u]\n", line, mfrc522.uid.size, tagIDLow, tagIDHigh);
      mfrc522.PICC_HaltA();     // halt PICC
      mfrc522.PCD_StopCrypto1(); // stop encryption on PCD
      // Success Reading Tag
      tagOK();
    }
  }

endLoop:
  // Write Industrial Registers
  // na.showRegister();
  delay(50);
  na.writeAllRegister();
}

void tagOK()
{
  setKeyLock(true);
  // Update Door Opening Time Stamp
  lastDoorOpened = millis();
  startBlinkTime = lastDoorOpened;
  startResultTime = lastDoorOpened;
  digitalWrite(GREEN_PIN, HIGH);
}

void tagFailed()
{
  digitalWrite(RED_PIN, HIGH);
  startResultTime = millis();
}

void setKeyLock(bool keyON)
{
  if (keyON)  {
	  digitalWrite(RED_PIN, HIGH);
    if (naminoReady)  {
      na.writeRele(true);
    }
  }
  else  {
	  digitalWrite(RED_PIN, LOW);    
    if (naminoReady)  {
      na.writeRele(false);
    }
  }
  Serial.printf("Set KeyLock: [%d] Namino Ready:[%d]\n", keyON, naminoReady);
}

bool setBlink(bool blinkON)
{
  if (blinkON)  {
	  digitalWrite(YELLOW_PIN, HIGH);    
  }
  else  {
	  digitalWrite(YELLOW_PIN, LOW);    
  }
  // Serial.printf("Set Led Blink: [%d]\n", blinkON);
  return not blinkON;
}