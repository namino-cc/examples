/*
  namino_libra, 0-5 kg digital double scale + win indicator

  display: 
  https://www.maffucci.it/2021/02/08/arduino-utilizzo-delllcd1602-keypad-shield-della-keyestudio/
  https://wiki.keyestudio.com/Ks0256_keyestudio_LCD1602_Expansion_Shield
  https://forum.arduino.cc/t/lcd-keypad-shield-for-arduino-lcd-1602-wiring-by-botton-side/177192
*/

// include the library code
#include "namino_rosso.h"

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

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
#define LCD_HOME(r)                 lcd.setCursor(0,r);
#define ABS(x)                      ((x)>0.0?(x):-(x))
#define FULL_SCALE                  (5)      
#define WEIGHT_NET_L                (ABS(weightLeftM - weightLeftBias) * weightLeftRamp)
#define WEIGHT_NET_R                (ABS(weightRightM - weightRightBias) * weightRightRamp)
#define AUTOZERO_COUNT_MAX          (200)

float weightLeftM = 0;
float weightLeftBias = 0;
float weightLeftRamp = 372.0 / 2.85;
float weightRightM = 0;
float weightRightBias = 0;
float weightRightRamp = 372.0 / 2.85; // float weightRightRamp = 372.0 / 1.85; mect cal weight
float weightTol = 150.0;
int   autozeroCount = AUTOZERO_COUNT_MAX;
bool  winLock = false;

unsigned long lastTiming = millis();
unsigned long lastDisplayTiming = millis();
bool          indicator = false;
bool          configAN = true;
unsigned long msAN = millis();
char          buf[64];

namino_rosso nr = namino_rosso();

void setup() {
  Serial.begin(115200);
  delay(500);  // mandatory delay

  // reset namino microcontroller. Industrial side board reset.
  nr.resetSignalMicroprocessor();

  // Opening/closing of communication / initialization of the industrial side interface
  nr.begin(800000U, MISO, MOSI, SCK, SS);

  Serial.println();
  Serial.println("=========================================");
  Serial.println("##    NAMINO ROSSO LIBRA SAMPLE CODE   ##");
  Serial.println("=========================================");
  Serial.println();
  delay(2000);

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
  lcd.clear();

  autozeroCount = AUTOZERO_COUNT_MAX;
  weightLeftM = weightRightM = 0;
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

void autozero() {
  if (nr.isReady()) {
    if (autozeroCount >= 2) {
      LCD_HOME(1);
      sprintf(buf, "  auto zero %d  ", autozeroCount);
      lcd.print(buf);
      weightLeftM = (nr.readLoadCell(1) + weightLeftM) / AUTOZERO_COUNT_MAX;
      weightRightM = (nr.readLoadCell(2) + weightRightM) / AUTOZERO_COUNT_MAX;
      sprintf(buf, "(1) #%d LM %5.2f RM %5.2f", autozeroCount, weightLeftM, weightRightM);
      Serial.println(buf);
    } else if (autozeroCount == 1) {
      weightLeftBias = weightLeftM;
      weightRightBias = weightRightM;
      weightLeftM = weightRightM = 0;
      sprintf(buf, "(2) #%d BL %5.2f BR %5.2f", autozeroCount, weightLeftBias, weightRightBias);
      Serial.println(buf);
      lcd.clear();
      autozeroCount = 0;
    }
    if (autozeroCount >= 2 && weightLeftM > 0 && weightRightM > 0) {
    // the first, 3, readings are at zero
      if (--autozeroCount < 0 ) { autozeroCount = 0; }
  }
  }
}

void weightMeasure() {
  if (nr.isReady() && autozeroCount == 0) {
      weightLeftM = (nr.readLoadCell(1) + weightLeftM) / AUTOZERO_COUNT_MAX;
      weightRightM = (nr.readLoadCell(2) + weightRightM) / AUTOZERO_COUNT_MAX;
  }
}

// void weightMeasure() {
//   if (nr.isReady() && autozeroCount == 0) {
//       rawValueL = nr.loadRegister(RO_ANALOG_IN_CH01);
//       rawValueR = nr.loadRegister(RO_ANALOG_IN_CH05);
//       // weightLeft = nr.readLoadCell(1);
//       // weightRight = nr.readLoadCell(2);

//       if (rawValueL >= 32768) {
//         Serial.println("L 1");
//         weightLeft = rawValueL - 65536;
//       } else {
//         Serial.println("L 2");
//         weightLeft = rawValueL;
//       }
//       if (rawValueR >= 32768) {
//         Serial.println("R 1");
//         weightRight = rawValueR - 65536;
//       } else {
//         Serial.println("R 2");
//         weightRight = rawValueR;
//       }
//   }
// }

void displayMeasure() {
  if (nr.isReady() && autozeroCount == 0 && !winLock) {
    LCD_HOME(0);
    sprintf(buf, "<L%5.0f %5.0f R>", WEIGHT_NET_L, WEIGHT_NET_R);
    lcd.print(buf);

    sprintf(buf, "MLT %5.2f MRT %5.2f", WEIGHT_NET_L, WEIGHT_NET_R);
    Serial.println(buf); // debug

    LCD_HOME(1);
    float delta = ABS(WEIGHT_NET_R - WEIGHT_NET_L);
    if (delta < weightTol && (WEIGHT_NET_L > 50 && WEIGHT_NET_R > 50)) {
      sprintf(buf, " HAI VINTO VINTO! ");
      indicator = true;
      winLock = true;
    } else if (WEIGHT_NET_L < WEIGHT_NET_R) {
      sprintf(buf, "%+5.0f g ------->", delta);
      indicator = false;
    } else if (WEIGHT_NET_L > WEIGHT_NET_R) {
      sprintf(buf, "<------- %+5.0f g  ", delta);
      indicator = false;
    } else {
      // nothing
    }
    lcd.print(buf);
  } else {
    // not ready, no zero calculated
  }
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

  // In the loop() there must be the following functions, which allow the exchange of values with the industrial side board
  nr.readAllRegister();

  // To set the configuration, in the loop inside an if there must be all the initial analog configurations, after having waited for the industrial side to start,
  if (configAN && nr.isReady()) {
    configAN = false;
    nr.writeRegister(WR_ANALOG_IN_CH01_CONF, ANALOG_IN_CH01_CONF_VALUES::CH01_LOAD_CELL);
    nr.writeRegister(WR_ANALOG_IN_CH05_CONF, ANALOG_IN_CH05_CONF_VALUES::CH05_LOAD_CELL);

    Serial.println("AN: config completed");
    Serial.printf("fwVersion: 0x%04x boardType: 0x%04x\n\n", nr.fwVersion(), nr.boardType());
  }

  autozero();

  int button_v = analogRead(GPIO_NUM_3);
  // debug analog keyboard
  // sprintf(buf, "V: %d %c   ", button_v, decodeButton(button_v));
  // LCD_HOME(0);
  // lcd.print(buf);
  // Serial.println(buf);

  if (decodeButton(button_v) == 'S') {
    weightMeasure();
    lastTiming = millis();
  } else if (decodeButton(button_v) == 'U') {
    // lamp test on
    Serial.println("lamp test on");
    indicator = true;
  } else if (decodeButton(button_v) == 'D') {
    // lamp test off
    Serial.println("lamp test off");
    indicator = false;
    winLock = false;
  } else if (decodeButton(button_v) == 'L') {
    // win lock off
    Serial.println("win lock off");
    indicator = false;
    winLock = false;
  }
  nr.writeDigOut(1, indicator);

  if (!winLock) {
    if (millis() - lastTiming > 1000 * 30) {
      displayBanner();
      lastTiming = millis();
    } else {
      weightMeasure();
      if (millis() - lastDisplayTiming > 1000 * 2) {
        displayMeasure();
        lastDisplayTiming = millis();
      }
    }
  }

  // nr.showRegister();    // debug function
  // In the loop() there must be the following function, which allow the exchange of values with the industrial side board
  nr.writeAllRegister();

  delay(250);  
}
