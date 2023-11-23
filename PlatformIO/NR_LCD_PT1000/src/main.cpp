#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

#include "namino_rosso.h"
#include <LiquidCrystal.h>

// Namino Board
#define CS_MICRO      10
#define LED_BUILTIN             1       // Out Channel for Built-in Led
namino_rosso nr = namino_rosso();
bool            configANIN = true;
bool            naminoReady = false;
float           tempT1 = 0.0;
float           tempT2 = 0.0;
float           tempT3 = 0.0;
float           tempT4 = 0.0;

// Define LCD Control Pins: LiquidCrystal uses pins D4-D9 to control Keypad board
const int RS = 40;                  // D8 Pin on Shield PB0 on namino
const int Enable = 39;              // D9 Pin on Shield PB1 on namino
const int D4 = 16;                  // D4 Pin on Shield PD4 on namino 
const int D5 = 21;                  // D5 Pin on Shield PD5 on namino
const int D6 = 42;                  // D5 Pin on Shield PD6 on namino
const int D7 = 41;                  // D5 Pin on Shield PD7 on namino
const int BL = 38;                  // D10 Pin on Shield PB2 on namino

//Define LCD Size
const int lcdRows = 2;              //  2 Rows
const int lcdCols = 16;             // 16 Columns

//Define Keyboard Const
const int keyPin = 3;               // Analog Input A0 on Shield PC0 on namino
const int btnRIGHT = 0;
const int btnUP = 1;
const int btnDOWN = 2;
const int btnLEFT = 3;
const int btnSELECT = 4;
const int btnNONE = 5;
//Allocate the LCD variable, with defined Pins
LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);

// Functions Protos
int  readKeyboard();
void setBackLight(bool mode);


// Timers
#define LOOP_PERIOD            500                  // 500ms loop period
#define FIELD_READ_PERIOD     4000                  // 4s interval of field read
#define BLINK_ON    10000                           // seconds of cursor blink

uint32_t      lastLoop = 0;
uint32_t      lastKeyTouch = 0;                     // Last Key Touch in seconds from Boot
uint32_t      secsFromBoot = 0;                     // Seconds from Namino Rosso 
uint32_t      lastFieldRead = 0;                    // Last field Read in millis()
uint32_t      loopCounter = 0;

// put function declarations here:
int myFunction(int, int);

void setup() {
  // Serial Setup
  Serial.begin(115200);
  delay(3000);
  Serial.println("");
  Serial.println("=====================================");
  Serial.println("## NAMINO ROSSO LCD PT1000 EXAMPLE ##");
  Serial.println("=====================================");
  Serial.print("SPI MOSI: ");
  Serial.println(MOSI);
  Serial.print("SPI MISO: ");
  Serial.println(MISO);
  Serial.print("LPC CLK: ");
  Serial.println(SCK);
  Serial.print("LPC SPI CS: ");
  Serial.println(CS_MICRO);  
  Serial.print("LPC RST: ");
  Serial.println(RESET_ADD_ON);  
  Serial.print("DISPLAY RST: ");
  Serial.println(RS);  
  Serial.print("DISPLAY Enable: ");
  Serial.println(Enable);  
  Serial.print("DISPLAY D4: ");
  Serial.println(D4);  
  Serial.print("DISPLAY D5: ");
  Serial.println(D5);  
  Serial.print("DISPLAY D6: ");
  Serial.println(D6);  
  Serial.print("DISPLAY D7: ");
  Serial.println(D7);  
  Serial.print("DISPLAY BACKLIGHT: ");
  Serial.println(BL);  
  Serial.print("Internal LED Pin: ");
  Serial.println(LED_BUILTIN);
  Serial.println("-------------------");
  Serial.flush();
  delay(2000);
  // Set lcd Pins as Output
  pinMode(RS, OUTPUT);
  pinMode(Enable, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(BL, OUTPUT);
  // Set Keys pin as input
  // pinMode(keyPin, INPUT);
  pinMode(keyPin, ANALOG);
  // LCD Setup
  lcd.begin(lcdCols, lcdRows);
  lcd.noCursor();
  lcd.setCursor(0,0);
  lcd.leftToRight();
  lcd.print("Keypad demo");
  lcd.noBlink();
  setBackLight(true);
  lastKeyTouch = millis();
  Serial.println("Display Init Done");

  // reset namino microcontroller. Industrial side board reset.
  nr.resetSignalMicroprocessor();

  // Opening/closing of communication / initialization of the industrial side interface
  nr.begin(800000U, MISO, MOSI, SCK, SS);
  delay(2000);
  Serial.println("Namino Rosso Init Done");

}

void loop() {
  char buf1[80];
  char buf2[80];
  uint32_t    theTime = millis();
  bool        naminoReady = false;
  bool        ledIsOn;

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
    nr.writeRegister(WR_ANALOG_IN_CH05_CONF, ANALOG_IN_CH05_CONF_VALUES::CH05_PT1000 );
    nr.writeRegister(WR_ANALOG_IN_CH07_CONF, ANALOG_IN_CH07_CONF_VALUES::CH07_PT1000 );
    nr.writeRegister(WR_ANALOG_OUT_CH01_CONF, ANALOG_OUT_CH01_CONF_VALUES::OUT_CH01_VOLTAGE);
    nr.writeAnalogOut(0.0); // output voltage
    nr.writeDigOut(LED_BUILTIN, true);
    nr.writeAllRegister();
    delay(200);
    secsFromBoot = 1;
    Serial.println("NR Analog config completed");
    Serial.printf("fwVersion: [0x%04x] boardType: [0x%04x] LifeTime: [%d]\n", nr.fwVersion(), nr.boardType(), secsFromBoot);
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

  // Read Led Status
  ledIsOn = nr.readDigOut(LED_BUILTIN);

  // Read Keyboard
  int buttons = readKeyboard();

  // Read Fields Values (PT1000 Temperature Probes)
  if (naminoReady && (theTime - lastFieldRead) >= FIELD_READ_PERIOD)   {
    lastFieldRead = theTime;
    tempT1 = nr.readPt1000(1);
    tempT2 = nr.readPt1000(2);
    tempT3 = nr.readPt1000(3);
    tempT4 = nr.readPt1000(4);
  }
  sprintf(buf1, "T1:%.1f T2:%.1f", tempT1, tempT2);
  sprintf(buf2, "T3:%.1f T4:%.1f", tempT3, tempT4);
  Serial.printf("%s %s\n", buf1, buf2);
  lcd.setCursor(0,0);
  lcd.write(buf1);
  lcd.setCursor(0,1);
  lcd.write(buf2);

  // printText(0,230, buf);

  // Display data
  // Namino Status and lifetime
  sprintf(buf1, "NR:%d - T:%d NR LT:%d Last T.:%d Elaps.:%d Led:[%s] Key:[%d]", naminoReady, (theTime / 1000), secsFromBoot, lastKeyTouch, 
                (secsFromBoot - lastKeyTouch), ledIsOn ? "ON" : "OFF", buttons);
  Serial.println(buf1);

  // Reverse Led Status
  nr.writeDigOut(LED_BUILTIN, not ledIsOn);

  // Update Registers
  if (naminoReady)  {
    nr.writeAllRegister();
  }
}

int readKeyboard()
{
  int adc_key_in = analogRead(keyPin);
  char buff[16];
  int retValue = 0;

  sprintf(buff, "%08d", adc_key_in);
  Serial.printf("Analog Read Value: [%s]\n", buff);
  
  if (adc_key_in <= 100)            // On my test board mapped as  0
    retValue = btnRIGHT;
  else if (adc_key_in <  800)       // On my test board mapped about as  490
    retValue = btnUP;  
  else if (adc_key_in < 1500)       // On my test board mapped about as  1230
    retValue = btnDOWN; 
  else if (adc_key_in < 2200)       // On my test board mapped about as  1830
    retValue = btnLEFT; 
  else if (adc_key_in < 3500)       // On my test board mapped about as  2780
    retValue = btnSELECT; 
  else                              // On my test board mapped as  4095
    retValue = btnNONE; 
  // almost one key is pressed 
  if (retValue != btnNONE)  {
    lastKeyTouch = millis();
  }
  // Check keyboard delay
  if (millis() - lastKeyTouch > BLINK_ON)  {
    lcd.noBlink();
    setBackLight(false);
  }
  else  {
    lcd.blink();
    setBackLight(true);
  }
  // Retur Keycode
  return retValue;
}

void setBackLight(bool mode)
{
  digitalWrite(BL, mode ? HIGH : LOW);
}