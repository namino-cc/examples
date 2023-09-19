//---------------------------------------
// Project **namino_RTC_LCD_Keypad_Shield**
//---------------------------------------
// Purpose of this Project: Show how to use **Robot D1 LCD Keypad Shield** with *Namino Boards* and internal DS1307 RTC via I2C.  
// See ReadMe.md for details
//---------------------------------------

#include "main.h"

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

// Key-Loop Counters
unsigned long lastKeyTouch = 0;
unsigned long lastFieldRead = 0;
unsigned long lastLoop = 0;
unsigned long lastTime = 0;


//Allocate the LCD variable, with defined Pins
LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);

// RTC Object
RTC_DS1307 rtc;
bool  rtcFound = false;


void setup() {
  // put your setup code here, to run once:
  // Serial Setup
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Serial Port Started");
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
  lcd.noBlink();
  setBackLight(true);
  lastKeyTouch = millis();
  Serial.println("Display Init Done");
  // RTC Setup
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
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long theTime = millis();

  // limit loop period
  if (abs( (long long) (theTime - lastLoop)) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;

  // Read Keyboard
  int buttons = readKeyboard();
  lcd.setCursor(9, 1);  
  if(buttons == btnRIGHT)       lcd.print("RIGHT ");
  else if(buttons == btnLEFT)   lcd.print("LEFT  ");
  else if(buttons == btnUP)     lcd.print("UP    ");
  else if(buttons == btnDOWN)   lcd.print("DOWN  ");
  else if(buttons == btnSELECT) lcd.print("SELECT");
  else if(buttons == btnNONE)   lcd.print("NONE  ");
  // Read RTC every second
  if (rtcFound && (abs( (long long) (theTime - lastTime)) > TIME_PERIOD))  {
    lastTime = theTime;
    DateTime now = rtc.now();
    char line[2 * lcdCols];
    sprintf(line, "%01d:%01d:%02d %01d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year() - 2000);
    lcd.setCursor(0,0);
    Serial.printf("Current Date and Time: [%s]\n", line);
    lcd.println(line);
  }
}

int readKeyboard()
{
  int adc_key_in = analogRead(keyPin);
  char buff[16];
  int retValue = 0;

  sprintf(buff, "%06d", adc_key_in);
  Serial.printf("Analog Read Value: [%s]\n", buff);
  lcd.setCursor(0,1);
  lcd.write(buff);
  
  if (adc_key_in <= 500)            // On my test board mapped as  0
    retValue = btnRIGHT;
  else if (adc_key_in < 1000)       // On my test board mapped about as  776
    retValue = btnUP;  
  else if (adc_key_in < 2000)       // On my test board mapped about as  1860
    retValue = btnDOWN; 
  else if (adc_key_in < 4000)       // On my test board mapped about as  2930
    retValue = btnLEFT; 
  else if (adc_key_in >= 4000)      // On my test board mapped as  4095
    retValue = btnNONE; 
  else 
    // At the moment the button Select seems not working....
    retValue = btnSELECT;  
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