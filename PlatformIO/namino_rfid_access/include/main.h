#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>


#define RST_PIN         17          // Configurable, see typical pin layout above

#define SDA_PIN          5          // Configurable, see typical pin layout above
#define SCK_PIN         18          // Configurable, see typical pin layout above
#define MISO_PIN        19          // Configurable, see typical pin layout above
#define MOSI_PIN        23          // Configurable, see typical pin layout above

#define LOOP_PERIOD   500           // 500ms loop period
#define SCREEN_ON    8000           //  s of screen on time
#define LINE_LEN    20              // Line Buffer Size
// #define FIELD_READ_PERIOD 2000      // 2s interval of field read

#endif // MAIN_H
