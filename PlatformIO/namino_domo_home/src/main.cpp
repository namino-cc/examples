/*
 * info display: http://www.lcdwiki.com/2.4inch_SPI_Module_ILI9341_SKU:MSP2402
*/
#include <Arduino.h>
#include <SPI.h>
#include <Preferences.h>
#include <SD.h>
#include <HCSR04.h>

#include "namino_rosso.h"

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

#include <TFT_eSPI.h>      // Hardware-specific library

namino_rosso nr = namino_rosso();

Preferences appPreferences;

#define LOOP_PERIOD   3000 // loop period
#define CS_MICRO      10
#define CS_SD_CARD    14
#define TFT_BACKLIGHT 16
#define CALIBRATION_DATA    "pointercal"
#define CALIBRATION_POINTS  5
// namino rosso industrial pins
#define LED_BAR_1     1
#define LED_BAR_2     2
// distance sense
double *distances;

TFT_eSPI myGLCD = TFT_eSPI(); // Invoke custom library

unsigned long lastLoop = 0;
unsigned long loopCounter = 0;
uint32_t      myColor = TFT_BLACK;
bool          sdCardPresent = false;

// Color Loop Flags
uint8_t       myRed = 0;
uint8_t       myGreen = 0;
uint8_t       myBlue = 0;
bool          redDone = false;
bool          greenDone = false;
bool          blueDone = false;

typedef enum {
  TEMP    = 1001,
  HUMI    = 1002,
  CO2     = 1003,
  VOC     = 1005,
  PM1_0   = 1006,
  PM2_5   = 1007,
  PM4_0   = 1008,
  PM10    = 1009,
} SENSORS_MODBUS_REGISTERS;

typedef enum {
  TEMP_GET    = 1010,
  TEMP_SET    = 1011,
} THERMOREGULATION_MODBUS_REGISTERS;

#define   NAMINO_MODBUS_RX        (44)
#define   NAMINO_MODBUS_TX        (43)
#define   NAMINO_MODBUS_RTS       (15)
#define   NAMINO_MODBUS_BAUD      (38400)
#define   NAMINO_MODBUS_NODE_ID   (2)

#include <ModbusRTU.h>

ModbusRTU mb;

bool readTouchCalibration(uint16_t *calData)
{
  char      ptKey[5];
  bool      nonZero = false;

  appPreferences.begin(CALIBRATION_DATA, true);
  for (uint8_t nPoint = 0; nPoint < CALIBRATION_POINTS; nPoint++)  {
    sprintf(ptKey, "pt%d", nPoint + 1);
    calData[nPoint] = appPreferences.getUShort(ptKey, 0);
    // At least one value must be non-zero
    if (not nonZero && calData[nPoint] > 0)  {
      nonZero = true;
    }
    Serial.printf("Read Calibration Point:%d Key:%s Value:%d Non Zero:[%d]", nPoint + 1, ptKey, calData[nPoint], nonZero);
    Serial.println();
  }
  appPreferences.end();
  return nonZero;
}

bool writeTouchCalibration(uint16_t *calData)
{
  char      ptKey[5];
  int       nBytes = 0;
  bool      nonZero = true;

  appPreferences.begin(CALIBRATION_DATA, false);
  for (uint8_t nPoint = 0; nPoint < CALIBRATION_POINTS && nonZero; nPoint++)  {
    sprintf(ptKey, "pt%d", nPoint + 1);
    nBytes = appPreferences.putUShort(ptKey, calData[nPoint]);
    nonZero = nBytes == sizeof(uint16_t);
    Serial.printf("Updated Calibration Point:%d Key:%s Value:%d Success:[%d]", nPoint + 1, ptKey, calData[nPoint], nonZero);
    Serial.println();
  }
  appPreferences.end();
  return nonZero;
}

void printText(int x, int y, String text, uint8_t textSize = 1, uint8_t textAllign = 1, uint8_t lineLength = 239) {
  /*  This function is used for displaying text on the screen
   *  Arguments:
   *    - X           position of the cursor
   *    - Y           position of the cursor
   *    - text        the actual text that will be displayed
   *    - textSize    text size can be one of these values 1, 2, 3, 4, 5
   *    - textAllign  text allign can be 1 - left align, 2 - center and 3 - right align
   *    - lineLenght  this should be used for line lenght of text, but does not works as shoud - TODO
   *    
   *  Returns:
   *  nothing
   */
  
  uint8_t newTextSize = textSize;
  uint8_t real_x = 0;
  uint32_t stringLength = text.length();
  uint8_t characters = stringLength * 5 * newTextSize + stringLength * newTextSize;

  while ((characters + 10) > lineLength) {
    // make text smaller if it exceeds the screen
    // all text in this app is not (and it should not be) longer than line length
    newTextSize = newTextSize - 1;
    characters = stringLength * 5 * newTextSize + stringLength * newTextSize;
  }
  myGLCD.setTextSize(newTextSize);

  if ((stringLength > 16) && (newTextSize > 2)) {
    // there is an error with text that is 17 characters long with size of 2
    // so this IF statement is explicitly for that error, to make text size smaller
    newTextSize = newTextSize - 1;
    characters = stringLength * 5 * newTextSize + stringLength * newTextSize;
  }
  myGLCD.setTextSize(newTextSize);

  if (characters + 10 < lineLength) {
    if (textAllign == 1) { // left
      myGLCD.setCursor(x, y);
      myGLCD.println(text);
    }
    else if (textAllign == 2) { // centered
      if (textSize == 1) { // letter length = 5
        real_x = x + int((lineLength - characters) / 2);
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else if (textSize == 2) { // letter length = 10
        real_x = x + int((lineLength - characters) / 2);
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else if (textSize == 3) { // letter length = 15
        real_x = x + int((lineLength - characters) / 2);
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else if (textSize == 4) { // letter length = 20
        real_x = x + int((lineLength - characters) / 2);
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else if (textSize == 5) { // letter length = 25
        real_x = x + int((lineLength - characters) / 2);
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else {
        myGLCD.setTextSize(1);
        myGLCD.setCursor(x, y);
        myGLCD.println("ERROR! Text size is from 1 to 5!");
      }
    }
    else if (textAllign == 3) { // right
      if (textSize == 1) { // letter length = 5
        real_x = x + lineLength - characters;
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else if (textSize == 2) { // letter length = 10
        real_x = x + lineLength - characters;
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else if (textSize == 3) { // letter length = 15
        real_x = x + lineLength - characters;
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else if (textSize == 4) { // letter length = 20
        real_x = x + lineLength - characters;
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else if (textSize == 5) { // letter lenght = 25
        real_x = x + lineLength - characters;
        myGLCD.setCursor(real_x, y);
        myGLCD.println(text);
      }
      else {
        myGLCD.setTextSize(1);
        myGLCD.setCursor(x, y);
        myGLCD.println("ERROR! Text size is from 1 to 5!");
      }
    }
    else {
      myGLCD.setTextSize(1);
      myGLCD.setCursor(x, y);
      myGLCD.println("ERROR! TextAlign is 0, 1 and 2!");
    }
  }
  else {
    myGLCD.setCursor(x, y);
    myGLCD.println(text);
  }
}

// Code to run a screen calibration, not needed when calibration values set in setup()
void touch_calibrate()
{
  uint16_t calData[CALIBRATION_POINTS];
  uint8_t calDataOK = 0;

  // Calibrate
  myGLCD.fillScreen(TFT_BLACK);
  myGLCD.setCursor(20, 0);
  myGLCD.setTextFont(2);
  myGLCD.setTextSize(1);
  myGLCD.setTextColor(TFT_WHITE, TFT_BLACK, true);

  myGLCD.println("Touch corners as indicated");

  myGLCD.setTextFont(1);
  myGLCD.println();

  myGLCD.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  myGLCD.fillScreen(TFT_BLACK);
  
  myGLCD.setTextColor(TFT_GREEN, TFT_BLACK, true);
  myGLCD.println("Calibration complete!");
  if (writeTouchCalibration(calData))  {
    myGLCD.println("Calibration Data saved");
    Serial.println("Calibration Data saved");
  }
  else  {
    myGLCD.println("Error Saving Calibration Data");
    Serial.println("Error Saving Calibration Data");
  }
  delay(4000);
}

void sense() {
  distances = HCSR04.measureDistanceCm();

  Serial.print(distances[0]);
  Serial.println(" cm");

  if (distances[0] < 0) {
    Serial.println("zona 0");
    nr.writeDigOut(LED_BAR_1, false);
    nr.writeDigOut(LED_BAR_2, false);
  } else if (distances[0] < 50) {
    Serial.println("zona 50  --");
    nr.writeDigOut(LED_BAR_1, true);
    nr.writeDigOut(LED_BAR_2, true);
  } else if (distances[0] < 100) {
    Serial.println("zona 100 ----------");
    nr.writeDigOut(LED_BAR_1, true);
    nr.writeDigOut(LED_BAR_2, false);
  } else if (distances[0] < 150) {
    Serial.println("zona 150 ---------------");
    nr.writeDigOut(LED_BAR_1, false);
    nr.writeDigOut(LED_BAR_2, true);
  } else if (distances[0] < 200) {
    Serial.println("zona 200 --------------------");
    nr.writeDigOut(LED_BAR_1, true);
    nr.writeDigOut(LED_BAR_2, false);
  } else {
    Serial.println("zona > 200 -------------------------");
    nr.writeDigOut(LED_BAR_1, false);
    nr.writeDigOut(LED_BAR_2, false);
  }
}

void setup() {
  uint16_t calibrationData[CALIBRATION_POINTS] = {0,0,0,0,0};   // Screen Calibration Data

  // Serial Setup
  Serial.begin(115200);
  delay(3000);
  Serial.println("");
  Serial.println("=====================================");
  Serial.println("##     NAMINO ROSSO DOMO HOME      ##");
  Serial.println("=====================================");
  Serial.println();
  Serial.print("TFT SPI MOSI: ");
  Serial.println(TFT_MOSI);
  Serial.print("TFT SPI MISO: ");
  Serial.println(TFT_MISO);
  Serial.print("TFT SPI SCK: ");
  Serial.println(TFT_SCLK);
  Serial.print("TFT SPI CS: ");
  Serial.println(TFT_CS);  
  Serial.print("TFT SPI RST: ");
  Serial.println(TFT_RST);  
  Serial.print("TFT SPI DC: ");
  Serial.println(TFT_DC);  
  Serial.print("TOUCH_CS: ");
  Serial.println(TOUCH_CS);
  Serial.print("TFT_BACKLIGHT: ");
  Serial.println(TFT_BACKLIGHT);
  Serial.print("NAMINO_MODBUS_RX: ");
  Serial.println(NAMINO_MODBUS_RX);
  Serial.print("NAMINO_MODBUS_TX: ");
  Serial.println(NAMINO_MODBUS_TX);
  Serial.print("NAMINO_MODBUS_RTS: ");
  Serial.println(NAMINO_MODBUS_RTS);
  Serial.println("-------------------");
  Serial.flush();
  delay(2000);

  Serial1.begin(NAMINO_MODBUS_BAUD, SERIAL_8N1, NAMINO_MODBUS_RX, NAMINO_MODBUS_TX);
  mb.begin(&Serial1, NAMINO_MODBUS_RTS);
  mb.slave(NAMINO_MODBUS_NODE_ID);

  // modbus registers
  mb.addHreg(SENSORS_MODBUS_REGISTERS::TEMP);
  mb.addHreg(SENSORS_MODBUS_REGISTERS::HUMI);
  mb.addHreg(SENSORS_MODBUS_REGISTERS::CO2);
  mb.addHreg(SENSORS_MODBUS_REGISTERS::VOC);
  mb.addHreg(SENSORS_MODBUS_REGISTERS::PM1_0);
  mb.addHreg(SENSORS_MODBUS_REGISTERS::PM2_5);
  mb.addHreg(SENSORS_MODBUS_REGISTERS::PM4_0);
  mb.addHreg(SENSORS_MODBUS_REGISTERS::PM10);
  mb.addHreg(THERMOREGULATION_MODBUS_REGISTERS::TEMP_GET);
  mb.addHreg(THERMOREGULATION_MODBUS_REGISTERS::TEMP_SET);

  // Deselect Micro Chip Select
  pinMode(CS_MICRO, OUTPUT);
  digitalWrite(CS_MICRO, HIGH);    

  // TFT Init
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  Serial.println("Starting TFT Display");
  myGLCD.init();
  myGLCD.setRotation(1);
  myGLCD.fillScreen(TFT_BLACK);                       //  fill the screen with black color
  myGLCD.setTextColor(TFT_WHITE, TFT_BLACK, true);    //  set the text color
  
  // Calibrate the touch screen and retrieve the scaling factors
  Serial.println("Retrieving Calibration Data");
  if (readTouchCalibration(calibrationData))  {
    myGLCD.setTouch(calibrationData);
    Serial.println("TFT Calibration Data applied");
  }
  else  {
    touch_calibrate();
  }
  Serial.println("Display Init Done");

  // SD Card
  Serial.println("Starting SD Card");
  pinMode(CS_SD_CARD, OUTPUT);
  digitalWrite(CS_SD_CARD, HIGH);
  // TODO insert common spi pins
  if(SD.begin(CS_SD_CARD))  {
    sdCardPresent = true;
    Serial.print("Card type:         ");
    switch (SD.cardType()) {
    case CARD_NONE:
      Serial.println("NONE");
      break;
    case CARD_MMC:
      Serial.println("MMC");
      break;
    case CARD_SD:
      Serial.println("SD");
      break;
    case CARD_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
    }    
  }
  else  {
    Serial.println("SD Card not found");
  }

  // reset namino microcontroller. Industrial side board reset.
  nr.resetSignalMicroprocessor();

  // Opening/closing of communication / initialization of the industrial side interface
  nr.begin(800000U, MISO, MOSI, SCK, SS);
  delay(2000);
  Serial.println("Namino Rosso Init Done");

  // distance meter
  HCSR04.begin(GPIO42, GPIO21);    // trig, echo
}

void loop() {
  char buf[80];
  unsigned long theTime = millis();
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
  bool pressed = false;
  bool naminoReady = false;
  uint32_t naLifeTime = nr.readLifeTime();

  uint16_t modbus_temp = 0;
  uint16_t modbus_humi = 0;
  uint16_t modbus_co2 = 0;
  uint16_t modbus_voc = 0;
  uint16_t modbus_pm1_0 = 0;
  uint16_t modbus_pm2_5 = 0;
  uint16_t modbus_pm4_0 = 0;
  uint16_t modbus_pm10 = 0;
  uint16_t modbus_tget = 0;
  uint16_t modbus_tset = 0;

  modbus_temp = mb.Hreg(SENSORS_MODBUS_REGISTERS::TEMP);
  modbus_humi = mb.Hreg(SENSORS_MODBUS_REGISTERS::HUMI);
  modbus_co2 = mb.Hreg(SENSORS_MODBUS_REGISTERS::CO2);
  modbus_voc = mb.Hreg(SENSORS_MODBUS_REGISTERS::VOC);
  modbus_pm1_0 = mb.Hreg(SENSORS_MODBUS_REGISTERS::PM1_0);
  modbus_pm2_5 = mb.Hreg(SENSORS_MODBUS_REGISTERS::PM2_5);
  modbus_pm4_0 = mb.Hreg(SENSORS_MODBUS_REGISTERS::PM4_0);
  modbus_pm10 = mb.Hreg(SENSORS_MODBUS_REGISTERS::PM10);
  modbus_tget = mb.Hreg(THERMOREGULATION_MODBUS_REGISTERS::TEMP_GET);
  modbus_tset = mb.Hreg(THERMOREGULATION_MODBUS_REGISTERS::TEMP_SET);

  // only debug
  Serial.printf("temp: %d | humi: %d | co2: %d | voc: %d | pm1_0: %d | pm2_5: %d | pm4_0: %d | pm10: %d | Tget %d | Tset %d\n", 
                modbus_temp, modbus_humi, modbus_co2, modbus_voc, modbus_pm1_0, modbus_pm2_5, modbus_pm4_0, modbus_pm10, modbus_tget, modbus_tset);

  mb.task();
  yield();
  delay(300);

  // limit loop period
  if (abs( (long long) (theTime - lastLoop)) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;
  // Read Industrial Registers
  nr.readAllRegister();
  naminoReady = nr.isReady();
  // Loop on Color components
  // Reset color Components
  // if (blueDone && myBlue == 255)  {
  //   myBlue = 0;
  // }
  // if (greenDone && myGreen == 255)  {
  //   myGreen = 0;
  // }
  // if (redDone && myRed == 255)  {
  //   myRed = 0;
  // }
  // Single component Loop
  // if (myBlue < 255 && not blueDone)  {
  //   ++myBlue;
  //   // Component Loop Ended
  //   if (myBlue == 255)  blueDone = true;
  // }
  // else if (myGreen < 255 && not greenDone)  {
  //   ++myGreen;
  //   // Component Loop Ended
  //   if (myGreen == 255)  greenDone = true;
  // }
  // else if (myRed < 255 && not redDone)  {
  //   ++myRed;
  //   // Component Loop Ended
  //   if (myRed == 255)  redDone = true;
  // }
  // else  if (blueDone && greenDone && redDone) {
  //   // All Components together up to 254
  //   myBlue++;
  //   myGreen++;
  //   myRed++;
  //   // Restart from single Components
  //   if (myBlue >= 255 && myGreen >=  255 && myRed >= 255)  {
  //     blueDone = false;
  //     greenDone = false;
  //     redDone = false;
  //     myBlue = 0;
  //     myGreen = 0;      
  //     myRed = 0;
  //   }
  // }
  // FillScreen: Colours are in 16bit per component format (rgb565)
  // Conversion from rgb888 to rgb565
  // uint16_t  backColor = myGLCD.color565(myRed, myGreen, myBlue);
  // myColor = myGLCD.color16to24(backColor);
  // myGLCD.fillScreen(TFT_BLACK);       // Repaint
  // Pressed will be set true is there is a valid touch on the screen
  pressed = myGLCD.getTouch(&t_x, &t_y);
  // Text Color
  // uint16_t  textColor = myGreen < 128 ? TFT_GREEN : TFT_BLUE;
  myGLCD.setTextColor(TFT_WHITE, TFT_BLACK, true);
  // Draw a white spot at the detected coordinates
  if (pressed) {
    myGLCD.fillCircle(t_x, t_y, 2, TFT_WHITE);
    // sprintf(buf, "Pressed @:X:%d - Y:%d", t_x, t_y);
    // printText(0,120, buf);
  }
  // Display data
  uint16_t ry = 0;
  sprintf(buf, "TC    %4.1f C", modbus_temp / 10.0);  // R1001 
  printText(0, ry, buf, 2, 1);
  Serial.print(buf); 

  ry += 25; 
  sprintf(buf, "HR    %4.1f %%", modbus_humi / 10.0); // R1002 
  printText(0, ry, buf, 2, 1);
  Serial.print(buf);

  ry += 25;
  sprintf(buf, "CO2  %5.0f ppm Tget %3.1f C", modbus_co2 / 1.0, modbus_tget / 10.0);  // R1003 --> to float
  printText(0, ry, buf, 2, 1);
  Serial.print(buf);

  ry += 25;
  sprintf(buf, "VOC  %5.0f     Tset %3.1f C", modbus_voc / 1.0, modbus_tset / 10.0); // R1005 --> to float
  printText(0, ry, buf, 2, 1);
  Serial.print(buf);

  ry += 25;
  sprintf(buf, "PM1.0 %4.1f ppm", modbus_pm1_0 / 10.0); // R1006
  printText(0, ry, buf, 2, 1);
  Serial.print(buf);
  
  ry += 25;
  sprintf(buf, "PM2.5 %4.1f ppm", modbus_pm2_5 / 10.0); // R1007
  printText(0, ry, buf, 2, 1);
  Serial.print(buf);
  
  ry += 25;
  sprintf(buf, "PM4.0 %4.1f ppm", modbus_pm4_0 / 10.0); // R1008
  printText(0, ry, buf, 2, 1);
  Serial.print(buf);

  ry += 25;
  sprintf(buf, "PM10  %4.1f ppm", modbus_pm10 / 10.0); // R1009
  printText(0, ry, buf, 2, 1);
  Serial.print(buf);

  if (distances != nullptr) { 
    ry += 25;
    sprintf(buf, "DIST %4.1f cm", distances[0]);
    printText(0, ry, buf, 2, 1);
    Serial.print(buf);
  }
  // Namino Status and lifetime
  // sprintf(buf, "Namino Ready:%d - LifeTime:%d", naminoReady, naLifeTime);
  // printText(0,60, buf);
  // Serial.println(buf);

  // distance meter
  if (naminoReady) {
    sense();
  }

  // only debug
  // if (naminoReady) {
  //   int i = random(100);

  //   if (i < 10) {
  //     nr.writeDigOut(LED_BAR_1, true);
  //     Serial.println("LED_BAR_1: true");
  //   } else {
  //     nr.writeDigOut(LED_BAR_1, false);
  //     Serial.println("LED_BAR_1: false");
  //   }

  //   if (i > 90) {
  //     nr.writeDigOut(LED_BAR_2, true);
  //     Serial.println("LED_BAR_2: true");
  //   } else {
  //     nr.writeDigOut(LED_BAR_2, false);
  //     Serial.println("LED_BAR_2: false");
  //   }
  // }

  nr.writeAllRegister();
}

