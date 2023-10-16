/*
 * info display: http://www.lcdwiki.com/2.4inch_SPI_Module_ILI9341_SKU:MSP2402
*/
#include "main.h"
#include "namino_rosso.h"

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif


// Namino Rosso Industrial Interface
namino_rosso nr = namino_rosso();
bool        configANIN = true;
bool        naminoReady = false;
bool        screenON = false;

// Application Preferences
Preferences appPreferences;

// RTC Object
RTC_DS1307 rtc;
bool          rtcFound = false;

// Sensirion SHT4 module
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
bool          sht41Present = false;

// TFT Object
TFT_eSPI myGLCD = TFT_eSPI(); // Invoke custom library

// Modbus RTU
ModbusRTU mb;
int mbNodeID = NAMINO_MODBUS_NODE_ID;

// Loop time Counters
uint32_t lastLoop = 0;                        // Last ended loop in millis()
uint32_t lastTouch = 0;                       // Last Screen Touch in seconds from Boot
uint32_t lastRTCReadTime = 0;                 // Last RTC Read in millis()
uint32_t lastFieldRead = 0;                   // Last field Read in millis()
uint32_t secsFromBoot = 0;                    // Seconds from Namino Rosso Boot                        

uint32_t      myColor = TFT_BLACK;
bool          sdCardPresent = false;

// Field Values
float         roomTemperature = 0.0;
float         roomHumidity = 0.0;
float         externalTemperature = 0.0;
float         heaterTemperature = 0.0;

bool readTouchCalibration(uint16_t *calData)
{
  char        ptKey[5];
  bool        nonZero = false;

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
  char        ptKey[5];
  int         nBytes = 0;
  bool        nonZero = true;

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
  
  uint8_t     newTextSize = textSize;
  uint8_t     real_x = 0;
  uint32_t    stringLength = text.length();
  uint8_t     characters = stringLength * 5 * newTextSize + stringLength * newTextSize;

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
  uint16_t  calData[CALIBRATION_POINTS];
  uint8_t   calDataOK = 0;

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

void setup() {

  uint16_t    calibrationData[CALIBRATION_POINTS] = {0,0,0,0,0};   // Screen Calibration Data

  // Serial Setup
  Serial.begin(115200);
  delay(3000);
  Serial.println("");
  Serial.println("=====================================");
  Serial.println("## NAMINO ROSSO TFT MODBUS THERMO  ##");
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
  Serial.print("NAMINO_I2C_SCL: ");
  Serial.println(NAMINO_I2C_SCL);
  Serial.print("NAMINO_I2C_SDA: ");
  Serial.println(NAMINO_I2C_SDA);
  Serial.print("Modbus RTU Speed: ");
  Serial.println(NAMINO_MODBUS_BAUD);
  Serial.print("Modbus RTU Node ID: ");
  Serial.println(mbNodeID);

  Serial.println("-------------------");
  Serial.flush();
  delay(2000);

  // Deselect Micro Chip Select
  pinMode(CS_MICRO, OUTPUT);
  digitalWrite(CS_MICRO, HIGH);    

  Serial.println("Starting TFT Display");
  myGLCD.init();
  myGLCD.setRotation(1);
  if (CONFIG_ENABLE_BL && TFT_BL >= 0)  {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  }
  
  // Calibrate the touch screen and retrieve the scaling factors
  Serial.println("Retrieving Calibration Data");
  if (readTouchCalibration(calibrationData))  {
    myGLCD.setTouch(calibrationData);
    Serial.println("TFT Calibration Data applied");
  }
  else  {
    touch_calibrate();
  }
  myGLCD.fillScreen(TFT_BLACK);       //  fill the screen with black color
  myGLCD.setTextColor(TFT_GREEN, TFT_BLACK, true);     //  set the text color
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

  // RTC Setup
  Serial.println("Starting RTC");
  Wire.begin(NAMINO_I2C_SDA, NAMINO_I2C_SCL);
  if (rtc.begin())  {
    // Uncomment to Set Date and time to Compile time if needed
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC Init Done");
    rtcFound = true;
  }
  else  {
    Serial.println("No RTC found!!");
  }

  // SHT 41 Sensor
  Serial.println("Starting Adafruit SHT4x");
  if (! sht4.begin()) 
  {
    Serial.println("Couldn't find SHT4x");
  }
  else  {
    Serial.println("Found SHT4x sensor");
    Serial.print("Serial number 0x");
    Serial.println(sht4.readSerial(), HEX);

    // You can have 3 different precisions, higher precision takes longer
    sht4.setPrecision(SHT4X_HIGH_PRECISION);

    // You can have 6 different heater settings
    sht4.setHeater(SHT4X_NO_HEATER);
    sht41Present = true;
  }
  // reset namino microcontroller. Industrial side board reset.
  nr.resetSignalMicroprocessor();

  // Opening/closing of communication / initialization of the industrial side interface
  nr.begin(800000U, MISO, MOSI, SCK, SS);
  delay(2000);
  Serial.println("Namino Rosso Init Done");
  // Opening Modbus Interface
  initMB();
}

void loop() {
  char        buf[80];
  uint32_t    theTime = millis();
  uint16_t    t_x = 0, t_y = 0; // To store the touch coordinates
  bool        pressed = false;
  sensors_event_t humidity, temp;



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
    delay(200);
    secsFromBoot = 1;
    Serial.println("NR Analog config completed");
    Serial.printf("fwVersion: [0x%04x] boardType: [0x%04x] LifeTime: [%d]\n", nr.fwVersion(), nr.boardType(), secsFromBoot);
    configANIN = false;
    setScreenBackLight(true);
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
    printText(0,230, buf);
    Serial.println(buf);
    // Switch back on BlackLight
    setScreenBackLight(true);
  }
  else  {
    // Switch off backlight
    if (naminoReady && lastTouch > 0 && ( (secsFromBoot - lastTouch)  > TFT_SCREEN_SAVER_SECONDS) )   {
      Serial.printf("Screen Saver Interval elapsed: %d\n", TFT_SCREEN_SAVER_SECONDS);
      setScreenBackLight(false);
    }
  }

  // Read RTC every second
  if (rtcFound && ( (theTime - lastRTCReadTime) > TIME_PERIOD))  {
    // Reading RTC
    lastRTCReadTime = theTime;
    DateTime now = rtc.now();
    sprintf(buf, "%01d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year() - 2000);
    Serial.printf("Current Date and Time: [%s]\n", buf);
    printText(0, 0, buf);
    // Namino Status and lifetime
    sprintf(buf, "Namino Ready:%d - LifeTime:%d Scr On: %d LT:%d", naminoReady, secsFromBoot, screenON, lastTouch);
    printText(0,60, buf);
    Serial.println(buf);
  }

  // Fields Values
  if ((theTime - lastFieldRead) > FIELD_READ_PERIOD)   {
    if (sht41Present)  {
  // Read SHT 41
      sht4.getEvent(&humidity, &temp);      // populate temp and humidity objects with fresh data    
      lastFieldRead = theTime;
      sprintf(buf,"Temperature: %.1f Humidity: %.1f", temp.temperature, humidity.relative_humidity);
      roomTemperature = temp.temperature;
      roomHumidity = humidity.relative_humidity;
      Serial.println(buf);
      printText(0, 120, buf);
      roomTemperature = temp.temperature;
      roomHumidity = humidity.relative_humidity;
    }
    if (naminoReady)  {
      externalTemperature = nr.readPt1000(1);
      heaterTemperature = nr.readPt1000(2);
      sprintf(buf,"External Temperature: %.1f Heater Temperature: %.1f", externalTemperature, heaterTemperature);
      Serial.println(buf);
      printText(0, 135, buf);
      // debug function
      nr.showRegister();    
    }
  }

  // Update Industrial Registers
  if (naminoReady && secsFromBoot)  {
    nr.writeAllRegister();
  }

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

void initMB()
{
  Serial1.begin(NAMINO_MODBUS_BAUD, SERIAL_8N1, NAMINO_MODBUS_RX, NAMINO_MODBUS_TX);
  Serial1.setRxBufferSize(2048);
  Serial1.setTxBufferSize(2048);
  Serial1.setRxFIFOFull(1);
  mb.begin(&Serial1, NAMINO_MODBUS_RTS);
  mb.slave(mbNodeID);

  // modbus registers
  mb.addHreg(RO_NAMINO_ID);
  mb.addHreg(RO_LIFE_TIME_H);
  mb.addHreg(RO_LIFE_TIME_L);

}
