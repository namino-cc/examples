#ifndef MAIN_H
#define MAIN_H

// RC-522 SPI PINS
#define CS_PIN       2
#define RST_PIN     18
#define SCK_PIN     14
#define MISO_PIN    17
#define MOSI_PIN     9

// Arduino Pins for LED traffic light module
#define GREEN_PIN    5
#define YELLOW_PIN   4
#define RED_PIN      3

// Namino Out Channel for Light Indicators
#define GREEN_LIGHT  1
#define YELLOW_LIGHT 2
#define RED_LIGHT    3
#define WHITE_LIGHT  4

// Namino Keylock Input
#define KEYLOCK_IN   1

// Loop interval
#define LOOP_PERIOD        100          // 100ms loop period
#define LINE_LEN            40          // Line Buffer Size
#define TAG_LEN             16          // Maximum Tag Len
#define KEYLOCK_ON_TIME    150          // Lock opening impulse duration
#define BLINK_ON_TIME    20000          // Blink On Time  
#define RESULT_TIME       6000          // Time to show Failure or Success

// Function Protos
void tagOK();
void tagFailed();
void setKeyLock(bool);
bool setBlink(bool);

#endif // MAIN_H