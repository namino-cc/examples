#ifndef MAIN_H
#define MAIN_H

// RC-522 SPI PINS
#define CS_PIN      48          // Chip Select  on Shared SPI Bus (RFID Reader)
#define RST_PIN     18          // Reset pin for RFID Reader

// Arduino 3.3V Pins for LED Traffic light module
#define GREEN_PIN    40
#define YELLOW_PIN   39
#define RED_PIN      38

// Box Lights (4M Relay is ACTIVE LOW!!)
#define GREEN_LIGHT  42
#define YELLOW_LIGHT 21
#define RED_LIGHT    16
#define WHITE_LIGHT  41

// Keylock Output on Magic Box (via Active HIGH relay)
#define KEYLOCK_OUT  14

// Keylock Input on Magic Box (12V Input on PC Input block)
#define KEYLOCK_IN   06

// Loop interval & Timing constants
#define LOOP_PERIOD        250          // General loop period
#define KEYLOCK_ON_TIME    200          // Lock opening impulse duration
#define BLINK_ON_TIME     8000          // Blink On Time  (Waiting time for electric lock cooling)
#define RESULT_TIME       4000          // Time to show Failure or Success
#define TAG_READ_DELAY     100          // Delay after Tag Read (Waiting time for closing the Tag Reader interface & return to Namino on shared SPI)

// Buffers
#define LINE_LEN           256          // Line Buffer Size
#define TAG_LEN             16          // Maximum Tag Len
#define ITEM_LEN            32          // Item Display string len
#define TAG_ITEMS           20          // Number of Tags handled
#define WINNERS_TAG          6          // Number of Winners Tags

// Function Protos
void selectWinners();
bool searchCodeInWinners(uint64_t);
void tagOK();
void tagFailed();
void setKeyLock(bool);
bool setBlink(bool);

#endif // MAIN_H