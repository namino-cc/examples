/*
  namino_libra, 0-100kg digital scale

  display: 
  https://www.maffucci.it/2021/02/08/arduino-utilizzo-delllcd1602-keypad-shield-della-keyestudio/
  https://wiki.keyestudio.com/Ks0256_keyestudio_LCD1602_Expansion_Shield
  https://forum.arduino.cc/t/lcd-keypad-shield-for-arduino-lcd-1602-wiring-by-botton-side/177192
*/

// include the library code

#include <LiquidCrystal.h>

// allocate LCD 20 x 2 Control Pins + modified keyboard R2 VCC = 3V3, see attached docs

const int RS =      40;  
const int EN =      39;  
const int D4 =      16;  
const int D5 =      21;  
const int D6 =      42;  
const int D7 =      41;  
// const int Backlight = 9;

// set LCD Size

const int lcdRows = 2;                  //  2 Rows
const int lcdCols = 16;                 // 16 Columns

// declare the LCD variable, with defined Pins

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// load cell values
float weightLeft = 0;
float weightLeftBias = 0;
float weightRight = 0;
float weightRightBias = 0;

#define LCD_HOME(r)                 lcd.setCursor(0,r);
#define ABS(x)                      ((x)>0?(x):-(x))
#define FULL_SCALE                  (5)      
#define WEIGHT_NET_L                ABS(weightLeft - weightLeftBias)
#define WEIGHT_NET_R                ABS(weightRight - weightRightBias)    

unsigned long lastTiming = millis();

void setup() {

 // set Pins as Output

  pinMode(RS, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  // pinMode(Backlight, OUTPUT);
  
  // turn on the Backlight
  // digitalWrite(Backlight, HIGH);
  
  lcd.begin(lcdRows, lcdCols);     //Initialise the LCD to the correct size
  
  displayBanner();
  autozero();
  lcd.clear();

  // random weights
  weightLeft = readWeightL();
  weightRight = readWeightR();
}

void displayBanner() {
  LCD_HOME(0);
  lcd.print("  namino libra  ");
  LCD_HOME(1);
  lcd.print("  double scale  ");
  delay(2000);

  LCD_HOME(0);
  lcd.print("  NAMINO  TEAM ");
  delay(2000);
  lcd.clear();
}

float readWeightL() {
  float n = random();
  float d = random();
  float w;

  if (d == 0) { d = 1; }

  w = ABS(n / d);
  if (w > FULL_SCALE) { w = FULL_SCALE; }
  return w; 
}

float readWeightR() {
  float n = random();
  float d = random();
  float w;

  if (d == 0) { d = 1; }

  w = ABS(n / d);
  if (w > FULL_SCALE) { w = FULL_SCALE; }
  return w; 
}

void autozero() {
  LCD_HOME(1);
  lcd.print("   auto  zero   ");

  for (uint8_t i = 0; i < 20; i++) {
    // TODO
  }

  weightLeftBias = readWeightL();
  weightRightBias = readWeightR();
  lcd.clear();
}

void displayMeasure() {
  char buf[32];

  LCD_HOME(0);
  sprintf(buf, "<L%5.2f %5.2f R>", WEIGHT_NET_L, WEIGHT_NET_R);
  lcd.print(buf);

  LCD_HOME(1);
  if (WEIGHT_NET_L < WEIGHT_NET_R) {
    sprintf(buf, "%+5.2f kg ------>", ABS(WEIGHT_NET_R - WEIGHT_NET_L));
  } else if (WEIGHT_NET_L > WEIGHT_NET_R) {
    sprintf(buf, "<------ %+5.2f kg  ", ABS(WEIGHT_NET_R - WEIGHT_NET_L));
  } else {
    sprintf(buf, "================");
  }
  lcd.print(buf);
}

char decodeButton(int button_v) {
  if (button_v >= 4000) {
    return ' ';
  } else if (button_v >= 2700 && button_v <= 2850) {
    return 'S';
  } else if (button_v >= 1800 && button_v <= 1900) {
    return 'L';
  } else if (button_v >= 400 && button_v <= 500) {
    return 'U';
  } else if (button_v >= 1100 && button_v <= 1200) {
    return 'D';
  } else if (button_v <= 100) {
    return 'R';
  }
  return ' ';
}

void loop() {
  char buf[20+1];

  int button_v = analogRead(GPIO_NUM_3);
  // debug analog keyboard
  // sprintf(buf, "V: %d %c   ", button_v, decodeButton(button_v));
  // LCD_HOME(0);
  // lcd.print(buf);

  if (decodeButton(button_v) == 'S') {
    weightLeft = readWeightL();
    weightRight = readWeightR();
    lastTiming = millis();
  }

  if (millis() - lastTiming > 1000 * 30) {
    displayBanner();
    lastTiming = millis();
  } else {
    displayMeasure();
  }
}
