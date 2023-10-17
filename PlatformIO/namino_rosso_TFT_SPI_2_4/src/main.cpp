/*
 * info display: http://www.lcdwiki.com/2.4inch_SPI_Module_ILI9341_SKU:MSP2402
*/
#include <Arduino.h>
#include <SPI.h>
#include <Preferences.h>
#include <SD.h>

#include "namino_rosso.h"

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

#include <TFT_eSPI.h>      // Hardware-specific library

namino_rosso nr = namino_rosso();

Preferences appPreferences;

#define LOOP_PERIOD   500 // 500ms loop period
#define CS_MICRO      10
#define CS_SD_CARD    14
#define CALIBRATION_DATA    "pointercal"
#define CALIBRATION_POINTS  5

#define TFT_SCREEN_SAVER_SECONDS 600         // Screen Saver ON in Seconds


TFT_eSPI myGLCD = TFT_eSPI(); // Invoke custom library

uint32_t      lastLoop = 0;
uint32_t      lastTouch = 0;                       // Last Screen Touch in seconds from Boot
uint32_t      secsFromBoot = 0;                    // Seconds from Namino Rosso 
uint32_t      loopCounter = 0;
uint32_t      myColor = TFT_BLACK;
bool          sdCardPresent = false;
bool          configANIN = true;
bool          naminoReady = false;
// Color Loop Flags
uint8_t       myRed = 0;
uint8_t       myGreen = 0;
uint8_t       myBlue = 0;
bool          redDone = false;
bool          greenDone = false;
bool          blueDone = false;

bool          screenON = false;


void setScreenBackLight(bool setON);

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
  myGLCD.setTextColor(TFT_WHITE, TFT_BLACK);

  myGLCD.println("Touch corners as indicated");

  myGLCD.setTextFont(1);
  myGLCD.println();

  myGLCD.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  myGLCD.fillScreen(TFT_BLACK);
  
  myGLCD.setTextColor(TFT_GREEN, TFT_BLACK);
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

void setup() {

  uint16_t calibrationData[CALIBRATION_POINTS] = {0,0,0,0,0};   // Screen Calibration Data

  // Serial Setup
  Serial.begin(115200);
  delay(3000);
  Serial.println("");
  Serial.println("=====================================");
  Serial.println("##       ESP32 SPI TFT EXAMPLE     ##");
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
  Serial.print("TFT_BACKLIGHT PIN: ");
  Serial.println(TFT_BL);
  Serial.print("CONFIG_ENABLE_BL: ");
  Serial.println(CONFIG_ENABLE_BL);
  Serial.print("TFT_SCREEN SAVER SEC: ");
  Serial.println(TFT_SCREEN_SAVER_SECONDS);
  Serial.println("-------------------");
  Serial.flush();
  delay(2000);

  // Deselect Micro Chip Select
  pinMode(CS_MICRO, OUTPUT);
  digitalWrite(CS_MICRO, HIGH);    

  // TFT Init
  Serial.println("Starting TFT Display");
  myGLCD.init();
  myGLCD.setRotation(1);
  if (CONFIG_ENABLE_BL && TFT_BL >= 0)  {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  }
  myGLCD.fillScreen(TFT_BLACK);       //  fill the screen with black color
  myGLCD.setTextColor(TFT_GREEN);     //  set the text color
  
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

}

void loop() {
  char buf[80];
  uint32_t    theTime = millis();
  uint16_t    t_x = 0, t_y = 0; // To store the touch coordinates
  bool        pressed = false;
  bool        naminoReady = false;


  // limit loop period
  if ((theTime - lastLoop) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;
  // Read Industrial Registers
  nr.readAllRegister();
  naminoReady = nr.isReady();

  // Analog Config (only once at boot when Namino is Ready)
  if (configANIN && naminoReady) {
    // Configure analog Input (not used at the moment)  
    nr.writeRegister(WR_ANALOG_IN_CH01_CONF, ANALOG_IN_CH01_CONF_VALUES::CH01_PT1000);
    nr.writeRegister(WR_ANALOG_IN_CH03_CONF, ANALOG_IN_CH03_CONF_VALUES::CH03_PT1000);
    nr.writeRegister(WR_ANALOG_IN_CH05_CONF, ANALOG_IN_CH05_CONF_VALUES::CH05_DISABLED );
    nr.writeRegister(WR_ANALOG_IN_CH06_CONF, ANALOG_IN_CH06_CONF_VALUES::CH06_DISABLED );
    nr.writeRegister(WR_ANALOG_IN_CH07_CONF, ANALOG_IN_CH07_CONF_VALUES::CH07_DISABLED );
    nr.writeRegister(WR_ANALOG_IN_CH08_CONF, ANALOG_IN_CH08_CONF_VALUES::CH08_DISABLED );
    nr.writeRegister(WR_ANALOG_OUT_CH01_CONF, ANALOG_OUT_CH01_CONF_VALUES::OUT_CH01_VOLTAGE);
    nr.writeAnalogOut(0.0); // output voltage
    nr.writeAllRegister();
    delay(200);
    secsFromBoot = 1;
    Serial.println("NR Analog config completed");
    Serial.printf("fwVersion: [0x%04x] boardType: [0x%04x] LifeTime: [%d]\n", nr.fwVersion(), nr.boardType(), secsFromBoot);
    setScreenBackLight(true);
    configANIN = false;
    return;
  }
  // Namino Ready ?
  if (not naminoReady)  {
    Serial.println("NR NOT Ready");
    return;
  }
  // Seconds from boot
  secsFromBoot = nr.readLifeTime();

  // Touch Screen
  // Pressed will be set true is there is a valid touch on the screen
  pressed = myGLCD.getTouch(&t_x, &t_y);
  // Draw a white spot at the detected coordinates
  if (pressed) {
    myGLCD.fillCircle(t_x, t_y, 2, TFT_WHITE);
    sprintf(buf, "Pressed @:X:%d - Y:%d", t_x, t_y);
    printText(0,120, buf);
    Serial.println(buf);
    // Switch back on BlackLight
    setScreenBackLight(true);
    // delay(200);
  }
  else  {
    // Switch off backlight
    if (naminoReady && lastTouch > 0 && ( (secsFromBoot - lastTouch)  > TFT_SCREEN_SAVER_SECONDS) )   {
      Serial.printf("Screen Saver Interval elapsed: %d\n", TFT_SCREEN_SAVER_SECONDS);
      setScreenBackLight(false);
    }
  }

  // Loop on Color components
  // Reset color Components
  if (blueDone && myBlue == 255)  {
    myBlue = 0;
  }
  if (greenDone && myGreen == 255)  {
    myGreen = 0;
  }
  if (redDone && myRed == 255)  {
    myRed = 0;
  }
  // Single component Loop
  if (myBlue < 255 && not blueDone)  {
    ++myBlue;
    // Component Loop Ended
    if (myBlue == 255)  blueDone = true;
  }
  else if (myGreen < 255 && not greenDone)  {
    ++myGreen;
    // Component Loop Ended
    if (myGreen == 255)  greenDone = true;
  }
  else if (myRed < 255 && not redDone)  {
    ++myRed;
    // Component Loop Ended
    if (myRed == 255)  redDone = true;
  }
  else  if (blueDone && greenDone && redDone) {
    // All Components together up to 254
    myBlue++;
    myGreen++;
    myRed++;
    // Restart from single Components
    if (myBlue >= 255 && myGreen >=  255 && myRed >= 255)  {
      blueDone = false;
      greenDone = false;
      redDone = false;
      myBlue = 0;
      myGreen = 0;      
      myRed = 0;
    }
  }
  // FillScreen: Colours are in 16bit per component format (rgb565)
  // Conversion from rgb888 to rgb565
  uint16_t  backColor = myGLCD.color565(myRed, myGreen, myBlue);
  // myColor = myGLCD.color16to24(backColor);
  myGLCD.fillScreen(backColor);       // Repaint
  // Pressed will be set true is there is a valid touch on the screen
  pressed = myGLCD.getTouch(&t_x, &t_y);
  // Text Color
  uint16_t  textColor = myGreen < 128 ? TFT_GREEN : TFT_BLUE;
  myGLCD.setTextColor(textColor);
  // Draw a white spot at the detected coordinates
  if (pressed) {
    myGLCD.fillCircle(t_x, t_y, 2, TFT_WHITE);
    sprintf(buf, "Pressed @:X:%d - Y:%d", t_x, t_y);
    printText(0,120, buf);
  }
  // Display data
  sprintf(buf, "LC: %6d BG Color: %08X R:%d G:%d B:%d ", ++loopCounter, backColor, myRed, myGreen, myBlue);
  printText(0,0, buf);
  Serial.print(buf);
  // Namino Status and lifetime
  sprintf(buf, "NR:%d - LifeTime:%d Last Touch:%d Remaing:%d", naminoReady, secsFromBoot, lastTouch, TFT_SCREEN_SAVER_SECONDS - (secsFromBoot - lastTouch));
  printText(0,60, buf);
  Serial.println(buf);
}

void setScreenBackLight(bool setON)
{
  if (setON)  {
    if (CONFIG_ENABLE_BL && TFT_BL >= 0)  {
      digitalWrite(TFT_BL, HIGH);
    }
    lastTouch = secsFromBoot;
  }
  else  {
    if (CONFIG_ENABLE_BL && TFT_BL >= 0)  {
      digitalWrite(TFT_BL, LOW);
    }
    lastTouch = 0;
  }
  screenON = setON;
  delay(100);
  Serial.printf("Set Screen to: %s (BL: Enabled:%d  BL Pin:%d Last Touch Sec:%d)\n", setON ? "ON" : "OFF", CONFIG_ENABLE_BL, TFT_BL, lastTouch);
}
