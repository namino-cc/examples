#include <LiquidCrystal.h>

#define SCREEN_ON   10000           //  s of screen on time
#define LOOP_PERIOD   200           // 200ms loop period
#define FIELD_READ_PERIOD 4000      // 4s interval of field read

//Define LCD Control Pins
const int RS = 40;  
const int Enable = 39;  
const int D4 = 16;  
const int D5 = 21;  
const int D6 = 42;  
const int D7 = 41;  

//Define LCD Size
const int lcdRows = 2;                  //2 Rows
const int lcdCols = 16;                 //16 Columns

//Define Keyboard Const
const int keyPin = 3; 
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


//Allocate the LCD variable, with defined Pins
LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);

// Functions Protos
int readKeyboard();

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
  // Set Keys pin as input
  pinMode(keyPin, INPUT);
  // LCD Setup
  lcd.begin(lcdCols, lcdRows);
  lcd.cursor();
  lcd.setCursor(0,0);
  lcd.leftToRight();
  lcd.print("Keypad demo");
  lcd.noBlink();
  Serial.println("Display Init Done");

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
}

int readKeyboard()
{
  int adc_key_in = analogRead(keyPin);
  char buff[16];
  int retValue = 0;

  sprintf(buff, "%08d", adc_key_in);
  Serial.printf("Analog Read Value: [%s]\n", buff);
  lcd.setCursor(0,1);
  lcd.write(buff);
  
  if (adc_key_in <= 500) 
    retValue = btnRIGHT;
  else if (adc_key_in > 500 && adc_key_in < 1000)   
    retValue = btnUP;  
  else if (adc_key_in >= 1000 && adc_key_in < 2000)  
    retValue = btnDOWN; 
  else if (adc_key_in >= 2000 && adc_key_in < 4000)  
    retValue = btnLEFT; 
  else if (adc_key_in >= 4000)  
    retValue = btnNONE; 
  else 
    retValue = btnSELECT;  
  // almost one key is pressed 
  if (retValue > 0 && retValue != btnNONE)  {
    lastKeyTouch = millis();
  }
  // Check keyboard delay
  if (millis() - lastKeyTouch > SCREEN_ON)  {
    lcd.noBlink();
  }
  else  {
    lcd.blink();
  }
  // Retur Keycode
  return retValue;
}
