#ifndef MAIN_H
#define MAIN_H

// RC-522 SPI PINS
#define CS_MICRO    10          // Chip Select for LPC on Shared SPI Bus
#define CS_PIN       2          // Chip Select for user device on Shared SPI Bus (RFID Reader)
#define RST_PIN     18          // Reset pin for RFID Reader

// Arduino Pins for LED traffic light module
#define GREEN_PIN    5
#define YELLOW_PIN   4
#define RED_PIN      3

// Namino Out Channel for Light Indicators on Magic Box
#define GREEN_LIGHT  1
#define YELLOW_LIGHT 2
#define RED_LIGHT    3
#define WHITE_LIGHT  4

// Namino Keylock Input on Magic Box
#define KEYLOCK_IN   1

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
#define WINNERS_TAG         10          // Number of Winners Tags

// Function Protos
void selectWinners();
bool searchCodeInWinners(uint64_t);
void tagOK();
void tagFailed();
void setKeyLock(bool);
bool setBlink(bool);

#endif // MAIN_H