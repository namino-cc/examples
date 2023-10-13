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
bool            naminoErr = false;
int             naminoErrors = 0;

// RC522 Reader Instance
MFRC522 mfrc522(CS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
int             tagReaderErrors = 0;

// Time counters (in millis)
unsigned long lastTagdRead = 0;
unsigned long lastLoop = 0;
unsigned long lastDoorOpened = 0;
unsigned long startBlinkTime = 0;
unsigned long startResultTime = 0;

// Status Flags
bool          readerOk = false;
bool          blinkOn = false;
int           totalAttempts = 0;
int           successAttempts = 0;
char          printLine[LINE_LEN + 1];


// Tag Items
uint64_t      tagCodes[TAG_ITEMS] =  {
  2737845411,
  3711343923,
  3711716611,
  3712263715,
  3712378723,
  3712785523,
  3712971107,
  3712972387,
  3713360963,
  3714530083
};
// Winner codes (different for each attempt)
uint64_t      winnerCodes[WINNERS_TAG];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
  pinMode(CS_MICRO, OUTPUT);
  digitalWrite(CS_MICRO, HIGH);    

  // Starting SPI
  // SPI Setup (Exposed SPI Pin of Namino boards)
  SPI.begin(SCK, MISO, MOSI, CS_PIN);

  // MFRC522 Setup
  Serial.println("Starting RFID Reader");
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
    readerOk = false;
    Serial.println("Error in Reader Self Test");
  }

  // Init Namino Arancio Board
  Serial.println("Starting Namino industrial Board");
  delay(2000);
  // reset namino microcontroller
  na.resetSignalMicroprocessor();
  na.begin(800000U, MISO, MOSI, SCK, SS);
  Serial.println("Waiting Namino industrial Board Analog Configuration");
  randomSeed(millis());
}

void loop() {
  unsigned long theTime = millis();
  uint64_t    tagIDLow = 0;
  uint64_t    tagIDHigh = 0;
  byte        tagID[TAG_LEN];
  bool        keyLocked = true;


  // limit loop period
  if (abs( (long long) (theTime - lastLoop)) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;

  // Read Industrial Registers
  na.readAllRegister();
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
    Serial.printf("Namino Arancio config completed - fwVersion: [0x%04x] boardType: [0x%04x] LifeTime: [%d]\n", na.fwVersion(), na.boardType(), na.readLifeTime());
    configANIN = false;
    // Feedbak to User
    digitalWrite(GREEN_PIN, LOW);    
    digitalWrite(RED_PIN, LOW);    
    digitalWrite(YELLOW_PIN, LOW);    
  }

  if (naminoReady)  {
    // Back from Error
    if (naminoErr)  {
      digitalWrite(RED_PIN, LOW);    
      naminoErr = false;
      Serial.println("Namino Back ONLINE");    
    }
    // Reading Keylock Status and setting Keylock Light
    keyLocked = na.readDigIn(KEYLOCK_IN);
    if (keyLocked)  {
      // Keylock Input is ON if keylock is closed, so White Light must be OFF 
      na.writeDigOut(WHITE_LIGHT, false);
    }
    else  {
      // White Light must be ON if Keylock is OFF
      na.writeDigOut(WHITE_LIGHT, true);
    }
  }
  else  {
    // Namino not ready after IO config
    if (not configANIN)  {
      naminoErr = true;
      naminoErrors++;
      Serial.println("Namino is OFFLINE");    
      if (digitalRead(RED_PIN))    {
        digitalWrite(RED_PIN, LOW);
      }
      else  {
        digitalWrite(RED_PIN, HIGH);
      }
    }
    return;
  }

  // Switch Off Keylock
  if (lastDoorOpened > 0 && KEYLOCK_ON_TIME < (theTime - lastDoorOpened) )  {
    setKeyLock(false);
    lastDoorOpened = 0;
  }

  // Led Blinking (Waiting time for electric lock cooling)
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
      na.writeDigOut(GREEN_LIGHT, false);
      digitalWrite(RED_PIN, LOW);
      na.writeDigOut(RED_LIGHT, false);
      startResultTime = 0;
    }
  }

  // Check RFID Reader
  if (not readerOk)  {
    tagReaderErrors++;
    Serial.printf("Tag Reader is OFFLINE. Errors:[%d]\n", tagReaderErrors);    
    return;
  }

  // Check RFID Presence only if ended result time
  // Uncomment Keylocked = true when 12V is not connected
  // keyLocked = true;
  if(keyLocked && startBlinkTime == 0 && mfrc522.PICC_IsNewCardPresent()) { 
    // new tag is available
    lastTagdRead = theTime;
    Serial.println();
    Serial.println("=====================================");
    if (mfrc522.PICC_ReadCardSerial()) { // NUID has been readed
      memset(printLine, 0, LINE_LEN);
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      strcat(printLine, "RFID/NFC Tag Type: ");
      Serial.print(printLine);

      strncpy(printLine, (char *) mfrc522.PICC_GetTypeName(piccType), LINE_LEN);
      Serial.println(printLine);

      // Get Tag ID
      strcpy(printLine, "UID:");
      Serial.print(printLine);
      // UUID may be 4, 7 or 10 Bytes
      memset(&(tagID[0]), 0, TAG_LEN);
      memset(printLine, 0, LINE_LEN);
      tagIDLow = 0;
      tagIDHigh = 0;
      for (int i = 0; i < mfrc522.uid.size; i++) {
        // Build Tag ID String
        char byteStr[ITEM_LEN];
        sprintf(byteStr, " %02X", (int) (mfrc522.uid.uidByte[i]) ) ;
        tagID[i] = mfrc522.uid.uidByte[i];
        strcat(printLine, byteStr);
      }
      // Can copy read value byte per byte as the platform is "Little Endian"
      memcpy(&tagIDLow, &(tagID[0]), sizeof(tagIDLow));
      if (mfrc522.uid.size > 8)  {
        memcpy(&tagIDHigh, &(tagID[sizeof(tagIDLow)]), sizeof(tagIDHigh));
      }
      else  {
        tagIDHigh = 0;
      }
      Serial.printf("[%s] Len:[%d] Low:[%u] - High:[%u] \n", printLine, mfrc522.uid.size, tagIDLow, tagIDHigh);
      mfrc522.PICC_HaltA();       // halt PICC
      mfrc522.PCD_StopCrypto1();  // stop encryption on PCD
      delay(TAG_READ_DELAY);
      // Generate a new Winners list
      totalAttempts++;
      selectWinners();
      // Search Tag in Winner List
      bool tagFound = searchCodeInWinners(tagIDLow);
      // Actuate attempt
      if (tagFound)  {
        successAttempts++;
        tagOK();
      }
      else  {
        tagFailed();      
      }
      Serial.flush();
      // Debug Tag List printout
      Serial.printf("T.Attempt:[%d] T.Success:[%d] Found:[%s] Winners List:", totalAttempts, successAttempts, (tagFound ? "Y" : "N"));
      memset(printLine, 0, LINE_LEN);       
      for (int item = 0; item < WINNERS_TAG; item++)  {
        char tagString[ITEM_LEN];
        sprintf(tagString, "[%u] ", winnerCodes[item]);
        strcat(printLine, tagString);
      }      
      Serial.println(printLine);      
      Serial.println("=====================================");
    }
  }
  // Update Industrial Registers
  na.writeAllRegister();
  // Loop End, show Namino Data
  Serial.printf("[L:%s R:%d E:%d L:%u]-", keyLocked ? "L" : "U", naminoReady, naminoErrors, na.readLifeTime());
}

void tagOK()
{
  setKeyLock(true);
  // Update Door Opening Time Stamp
  lastDoorOpened = millis();
  startBlinkTime = lastDoorOpened;
  startResultTime = lastDoorOpened;
  digitalWrite(GREEN_PIN, HIGH);
  na.writeDigOut(GREEN_LIGHT, true);
}

void tagFailed()
{
  digitalWrite(RED_PIN, HIGH);
  na.writeDigOut(RED_LIGHT, true);
  startResultTime = millis();
}

void setKeyLock(bool keyON)
{
  if (keyON)  {
	  // digitalWrite(RED_PIN, HIGH);
    if (naminoReady)  {
      na.writeRele(true);
    }
  }
  else  {
	  // digitalWrite(RED_PIN, LOW);    
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
    na.writeDigOut(YELLOW_LIGHT, true);
  }
  else  {
	  digitalWrite(YELLOW_PIN, LOW);    
    na.writeDigOut(YELLOW_LIGHT, false);
  }
  // Serial.printf("Set Led Blink: [%d]\n", blinkON);
  return not blinkON;
}

void selectWinners()
{
  int item = 0; 
  int attempts = 0;
  int tagIndexes[WINNERS_TAG];
  
  // Clean Winners List
  for (item = 0; item < WINNERS_TAG; item++)  {
    winnerCodes[item] = 0;   
  }
  // Fill Winners List
  item = 0;
  while (item < WINNERS_TAG) {
    // Find a code Index
    int randNum = (int) random(TAG_ITEMS);
    // Get Tag Code from Tag List
    uint64_t newCode = tagCodes[randNum];
    // Check if code is already present in Winner List
    if (not searchCodeInWinners(newCode))  {
      // Add Code to Winner List
      winnerCodes[item] = newCode;
      tagIndexes[item] = randNum;
      item++;
    }
    attempts++;
  }
  // debug printout
  Serial.printf("Winner List Ins.Attps:[%d] W.Ind.List:", attempts);
  memset(printLine, 0, LINE_LEN);       
  for (item = 0; item < WINNERS_TAG; item++)  {
    char tagString[ITEM_LEN];
    sprintf(tagString, "%d ", tagIndexes[item]);
    strcat(printLine, tagString);
  }
  Serial.println(printLine);
}

bool  searchCodeInWinners(uint64_t tagCode)
{
  int   item = 0; 
  bool  found = false;

  for (item = 0; item < WINNERS_TAG; item++)  {
    if (tagCode > 0 && tagCode == winnerCodes[item])  {
      found = true;
      break;
    }
    else if(winnerCodes[item] == 0)  {
      break;
    }
  }
  return found;
}