
#ifdef NAMINO_BIANCO_BOARD
#undef NAMINO_BIANCO_BOARD
#endif
#include "namino_bianco_pins.h"			// Namino Bianco Pins definitions
#include "main.h"

#include <MFRC522.h>

// RC522 Reader Instance
MFRC522 mfrc522(CS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
int           tagReaderErrors = 0;

// Time counters (in millis)
uint32_t      lastTagdRead = 0;
uint32_t      lastLoop = 0;
uint32_t      loopCounter = 0;
uint32_t      lastDoorOpened = 0;
uint32_t      startBlinkTime = 0;
uint32_t      startResultTime = 0;

// Status Flags
bool          readerOk = false;
bool          blinkOn = false;
int           totalAttempts = 0;
int           successAttempts = 0;
char          printLine[LINE_LEN + 1];

// Tag Items
uint64_t      tagCodes[TAG_ITEMS] =  {
  2596934586, 
  2597103498, 
  2605553114, 
  2607512746, 
  2607748650,
  2607771722, 
  2608145130, 
  2608145994, 
  2612808346, 
  2613737642, 
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
  Serial.println("##  BIANCO RFID MAGIC BOX EXAMPLE  ##");
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

  //---------------------------
  // I/O Pins
  //---------------------------
  // Traffic light
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  // Box Lights (4M Relay is ACTIVE LOW!!)
  pinMode(GREEN_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT, OUTPUT);
  pinMode(RED_LIGHT, OUTPUT);
  pinMode(WHITE_LIGHT, OUTPUT);
  // Keylock input
  pinMode(KEYLOCK_IN, INPUT);
  // Keylock Relay
  pinMode(KEYLOCK_OUT, OUTPUT);
  // Switch on Traffic Lights
  digitalWrite(GREEN_PIN, HIGH);    
  digitalWrite(RED_PIN, HIGH);    
  digitalWrite(YELLOW_PIN, HIGH);    
  // Switch off Magic Box Lights (Active LOW Relays)
  digitalWrite(GREEN_LIGHT, HIGH);    
  digitalWrite(YELLOW_LIGHT, HIGH);    
  digitalWrite(RED_LIGHT, HIGH);    
  digitalWrite(WHITE_LIGHT, HIGH);
  // Close Keylock
  digitalWrite(KEYLOCK_OUT, LOW);



  // Starting SPI
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);    
  // SPI Setup (Exposed SPI Pin on Mikroe Bus)
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

  randomSeed(millis());
  // Feedbak to User
  if (readerOk)  {
    digitalWrite(GREEN_PIN, LOW);    
    digitalWrite(RED_PIN, LOW);    
    digitalWrite(YELLOW_PIN, LOW);    
  }
}

void loop() {
  uint32_t    theTime = millis();
  uint64_t    tagIDLow = 0;
  uint64_t    tagIDHigh = 0;
  byte        tagID[TAG_LEN];
  bool        keyLocked = true;


  // limit loop period
  if ((theTime - lastLoop) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;

  // Check RFID Reader   
  if (not readerOk)  {
    tagReaderErrors++;
    Serial.printf("Tag Reader is OFFLINE. Errors:[%d]\n", tagReaderErrors);    
    return;
  }

  // Reading Keylock Status and setting Keylock Light
  keyLocked = digitalRead(KEYLOCK_IN);
  if (keyLocked)  {
    // Keylock Input is ON if keylock is closed, so White Light must be OFF 
    digitalWrite(WHITE_LIGHT, HIGH);
  }
  else  {
    // White Light must be ON if Keylock is OFF
    digitalWrite(WHITE_LIGHT, LOW);
  }

  // Switch Off Keylock after opening
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
        if ((loopCounter % 2) == 0)  {
          blinkOn = setBlink(blinkOn);
        }
      }
  }

  // Result Display
  if (startResultTime > 0)  {
    if (RESULT_TIME < (theTime - startResultTime))  {
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(GREEN_LIGHT, HIGH);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(RED_LIGHT, HIGH);
      startResultTime = 0;
    }
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
  // Loop End, show Keylock status
  Serial.printf("[%s]", keyLocked ? "L" : "U");
  loopCounter++;
}

void tagOK()
{
  setKeyLock(true);
  // Update Door Opening Time Stamp
  lastDoorOpened = millis();
  startBlinkTime = lastDoorOpened;
  startResultTime = lastDoorOpened;
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(GREEN_LIGHT, LOW);
  setBlink(true);
}

void tagFailed()
{
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(RED_LIGHT, LOW);
  startResultTime = millis();
}

void setKeyLock(bool keyON)
{
  if (keyON)  {
    digitalWrite(KEYLOCK_OUT, HIGH);
  }
  else  {
    digitalWrite(KEYLOCK_OUT, LOW);
  }
  Serial.printf("Set KeyLock: [%s]\n", keyON ? "ON" : "OFF");
}

bool setBlink(bool blinkON)
{
  if (blinkON)  {
	  digitalWrite(YELLOW_PIN, HIGH);    
	  digitalWrite(YELLOW_LIGHT, LOW);    
  }
  else  {
	  digitalWrite(YELLOW_PIN, LOW);    
	  digitalWrite(YELLOW_LIGHT, HIGH);    
  }
  Serial.printf("Set Led Blink: [%d]\n", blinkON);
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
