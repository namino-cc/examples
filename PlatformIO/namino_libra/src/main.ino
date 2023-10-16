/*
  namino_libra, 0-5 kg digital double scale + win indicator

  display: 
  https://www.maffucci.it/2021/02/08/arduino-utilizzo-delllcd1602-keypad-shield-della-keyestudio/
  https://wiki.keyestudio.com/Ks0256_keyestudio_LCD1602_Expansion_Shield
  https://forum.arduino.cc/t/lcd-keypad-shield-for-arduino-lcd-1602-wiring-by-botton-side/177192

  calibration:
  1. autozero 1 minute, unload any weights
  2. set 312 grams on left, press LEFT
  3. set 312 grams on right, press RIGHT
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
#define WEIGHT_NET_L                ((weightLeftM - weightLeftBias) * (312.0 / (weightLeftRamp - weightLeftBias)))
#define WEIGHT_NET_R                ((weightRightM - weightRightBias) * (312.0 / (weightRightRamp - weightRightBias)))
#define AUTOZERO_COUNT_MAX          (100)
#define COUNT_1                     (25)
#define COUNT_2                     (10)

float  weightLeftM = 0;
float  weightLeftBias = 0;
double weightLeftRamp = 0;
float  weightRightM = 0;
float  weightRightBias = 0;
double weightRightRamp = 0;
float  weightTol = 20.0;
int    autozeroCount = AUTOZERO_COUNT_MAX;

unsigned long lastTiming        = millis();
unsigned long lastDisplayTiming = millis();
bool          indicatorGreen    = false;
bool          indicatorRed      = false;
bool          configAN          = true;
unsigned long msAN              = millis();
char          buf[64];
// lamp dual color blink
typedef enum {
  DISABLE   = 0,
  RED       = 1,
  GREEN     = 2,
} LAMP_DUAL_BLINK;
unsigned long lastBlinkTiming   = millis();
unsigned long lastBlinkStart    = millis();
LAMP_DUAL_BLINK ldt             = LAMP_DUAL_BLINK::DISABLE;

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
  ldt = LAMP_DUAL_BLINK::DISABLE;
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
      //weightLeftM = (nr.readLoadCell(1) + weightLeftM * (AUTOZERO_COUNT_MAX - 1)) / AUTOZERO_COUNT_MAX;
      //weightRightM = (nr.readLoadCell(2) + weightRightM * (AUTOZERO_COUNT_MAX - 1)) / AUTOZERO_COUNT_MAX;
      weightMeasure();
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
  if (nr.isReady()) {
    if (autozeroCount > 0) {
      if (AUTOZERO_COUNT_MAX - autozeroCount < 10) {
        weightLeftM = nr.readLoadCell(1);
      } else {
        //Serial.println("weightMeasure autozeroCount L");
        weightLeftM = (nr.readLoadCell(1) + weightLeftM * (AUTOZERO_COUNT_MAX - 1)) / AUTOZERO_COUNT_MAX;
      }
    } else if (ABS(weightLeftM - nr.readLoadCell(1)) < 100) {
      weightLeftM = (nr.readLoadCell(1) + weightLeftM * (COUNT_1 - 1)) / COUNT_1;
    } else if (ABS(weightLeftM - nr.readLoadCell(1)) < 200) {
      weightLeftM = (nr.readLoadCell(1) + weightLeftM * (COUNT_2 - 1)) / COUNT_2;
    } else if (ABS(weightLeftM - nr.readLoadCell(1)) >= 200) {
      weightLeftM = nr.readLoadCell(1);
    }

    if (autozeroCount > 0) {
      if (AUTOZERO_COUNT_MAX - autozeroCount < 10) {
        weightRightM = nr.readLoadCell(2);
      } else {
        //Serial.println("weightMeasure autozeroCount R");
        weightRightM = (nr.readLoadCell(2) + weightRightM * (AUTOZERO_COUNT_MAX - 1)) / AUTOZERO_COUNT_MAX;
      }
    } else if (ABS(weightRightM - nr.readLoadCell(2)) < 100) {
      weightRightM = (nr.readLoadCell(2) + weightRightM * (COUNT_1 - 1)) / COUNT_1;
    } else if (ABS(weightRightM - nr.readLoadCell(2)) < 200) {
      weightRightM = (nr.readLoadCell(2) + weightRightM * (COUNT_2 - 1)) / COUNT_2;
    } else if (ABS(weightRightM - nr.readLoadCell(2)) >= 200) {
      weightRightM = nr.readLoadCell(2);
    } 
  }
}

void calibrateL() {
  if (nr.isReady() && autozeroCount == 0) {
      weightLeftRamp = weightLeftM;
      Serial.print(" weightLeftBias: ");
      Serial.print(weightLeftBias);
      Serial.print(" weightLeftRamp: ");
      Serial.println(weightLeftRamp);
  }
}

void calibrateR() {
  if (nr.isReady() && autozeroCount == 0) {
      weightRightRamp = weightRightM;
      Serial.print(" weightRightBias: ");
      Serial.print(weightRightBias);
      Serial.print(" weightRightRamp: ");
      Serial.println(weightRightRamp);
  }
}

void displayMeasure() {
  if (nr.isReady() && autozeroCount == 0) {
    LCD_HOME(0);
    sprintf(buf, "<L%5.0f %5.0f R>", WEIGHT_NET_L, WEIGHT_NET_R);
    lcd.print(buf);

    sprintf(buf, "MLT %f MRT %f", WEIGHT_NET_L, WEIGHT_NET_R);
    Serial.println(buf); // debug
    sprintf(buf, "weightLeftM %f weightRightM %f", weightLeftM, weightRightM);
    Serial.println(buf); // debug
    sprintf(buf, "weightLeftBias %f weightRightBias %f", weightLeftBias, weightRightBias);
    Serial.println(buf); // debug
    sprintf(buf, "weightLeftRamp %f weightRightRamp %f", weightLeftRamp, weightRightRamp);
    Serial.println(buf); // debug

    LCD_HOME(1);
    float delta = ABS(WEIGHT_NET_R - WEIGHT_NET_L);
    if (delta < weightTol && (WEIGHT_NET_L > 20 && WEIGHT_NET_R > 20)) {
      sprintf(buf, " HAI VINTO VINTO! ");
      indicatorGreen = true;
      indicatorRed = false;
      ldt = LAMP_DUAL_BLINK::DISABLE;
    } else if (WEIGHT_NET_L < WEIGHT_NET_R) {
      sprintf(buf, "%+5.0f g ------->", delta);
      indicatorGreen = false;
      indicatorRed = true;
    } else if (WEIGHT_NET_L > WEIGHT_NET_R) {
      sprintf(buf, "<------- %+5.0f g  ", delta);
      indicatorGreen = false;
      indicatorRed = true;
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

void dualBlink() {
  if (ldt == LAMP_DUAL_BLINK::DISABLE) return;

  if (millis() - lastBlinkTiming > 250) {
    lastBlinkTiming = millis();

    if (ldt == LAMP_DUAL_BLINK::RED) {
      ldt = LAMP_DUAL_BLINK::GREEN;
      indicatorGreen = true;
      indicatorRed = false;
    } else if (ldt == LAMP_DUAL_BLINK::GREEN) {
      ldt = LAMP_DUAL_BLINK::RED;
      indicatorGreen = false;
      indicatorRed = true;
    }
  }
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
    Serial.println("lamp test toggle");
    indicatorGreen = !indicatorGreen;
    indicatorRed = false;
  } else if (decodeButton(button_v) == 'D') {
    //
  } else if (decodeButton(button_v) == 'L') {
    Serial.println("calibrate 312 g on left");
    calibrateL();
  } else if (decodeButton(button_v) == 'R') {
    Serial.println("calibrate 312 g on right");
    calibrateR();
  }
  dualBlink();
  nr.writeDigOut(1, indicatorGreen);
  nr.writeDigOut(2, indicatorRed);

  if ((autozeroCount == 0) && (millis() - lastTiming > 1000 * 30)) {
    displayBanner();
    lastTiming = millis();
  } else {
    weightMeasure();
    if (millis() - lastDisplayTiming > 1000 * 2) {
      displayMeasure();
      lastDisplayTiming = millis();
    }
  }

  // nr.showRegister();    // debug function
  // In the loop() there must be the following function, which allow the exchange of values with the industrial side board
  nr.writeAllRegister();

  // reset game
  if (WEIGHT_NET_L < 25 && WEIGHT_NET_R < 25) {
    indicatorGreen = false;
    indicatorRed = true;
    ldt = LAMP_DUAL_BLINK::DISABLE;
  }
  if ((autozeroCount == 0) &&
      (WEIGHT_NET_L < 25 && WEIGHT_NET_R >= 25) ||
      (WEIGHT_NET_L >= 25 && WEIGHT_NET_R < 25)) { 
    Serial.println("BLINK");
  }
  // lamp dual color blink
  // // lamp dual color blink start
  // if ((autozeroCount == 0) && 
  //     (ldt == LAMP_DUAL_BLINK::DISABLE) && 
  //     (WEIGHT_NET_L >= 50 || WEIGHT_NET_R >= 50)) {
  //   lastBlinkStart = millis();
  //   ldt = LAMP_DUAL_BLINK::RED;
  //   Serial.println("2 ---");
  // }
  // // lamp dual color blink reset
  // if ((autozeroCount == 0) && 
  //     (ldt != LAMP_DUAL_BLINK::DISABLE) && 
  //     (WEIGHT_NET_L < 25 && WEIGHT_NET_R < 25)) {
  //   ldt = LAMP_DUAL_BLINK::DISABLE;
  //   Serial.println("3 ---");
  // }
  // // lamp dual color blink timeout 
  // if ((autozeroCount == 0) && 
  //     (millis() - lastBlinkStart > (1000 * 5))) {
  //   ldt = LAMP_DUAL_BLINK::DISABLE;
  //   Serial.println("4 ---");
  // }

  delay(250);  
}
