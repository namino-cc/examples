#ifndef MAIN_H
#define MAIN_H

#include <Preferences.h>        // Preference Settings Library
#include <SD.h>                 // SD       Library
#include <RTClib.h>             // RTC      Library
#include <TFT_eSPI.h>           // TFT      Library
#include <Adafruit_SHT4x.h>     // SHT4x    Library
#include <ModbusRTU.h>


#define NAMINO_I2C_SCL         0 
#define NAMINO_I2C_SDA         1

#define LOOP_PERIOD          200            // 200ms loop period
#define TIME_PERIOD         1000            // Read period of RTC
#define FIELD_READ_PERIOD   4000            // 4s interval of field read

#define TFT_SCREEN_SAVER_SECONDS 60         // Screen Saver ON in Seconds

#define CS_MICRO      10
#define CS_SD_CARD    14

#define CALIBRATION_DATA    "pointercal"
#define CALIBRATION_POINTS  5

// Modbus RTU parameters
#define NAMINO_MODBUS_RX        (44)        /* */
#define NAMINO_MODBUS_TX        (43)
#define NAMINO_MODBUS_RTS       (15)
#define NAMINO_MODBUS_BAUD      (9600)
#define NAMINO_MODBUS_NODE_ID   (11)


// Function Protos
void setScreenBackLight(bool);
void initMB();
#endif // MAIN_H
