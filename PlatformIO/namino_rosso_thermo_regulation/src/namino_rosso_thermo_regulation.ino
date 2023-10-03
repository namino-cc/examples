/* namino rosso lamp thermo regulator

Copyright (c) 2023 Namino Team, version: 1.0.0 @ 2023-09-26

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* NB, use profile: adafruit_feather_esp32s3_reversetft */

/* datasheets:
   https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf 
   https://www.analog.com/media/en/technical-documentation/data-sheets/max7219-max7221.pdf
   infos:
   http://www.whatimade.today/programming-an-8-digit-7-segment-display-the-easy-way-using-a-max7219/
*/

#include <Arduino.h>
#include <SPI.h>
#include <Mapf.h>

#include "./namino_rosso.h"

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

namino_rosso nr = namino_rosso();
uint16_t      loopNr = 1;
float_t       setPoint = 30;
char          buf[64] = { 0 };
uint8_t       rele = 0;
bool          configANOUT = true;
unsigned long msANOUT = millis();
float_t       tc = 0;
#define AN_OUT_INIT_DELAY_MS  (5 * 1000)


#define DISPLAY_DIGITS  8
// define pins attached to MAX7219 (and also see notes above)
#define MAX7219_DIN           38
#define MAX7219_CS            39
#define MAX7219_CLK           40

// enumerate the MAX7219 registers
// See MAX7219 Datasheet, Table 2, page 7
enum {  MAX7219_REG_DECODE    = 0x09,  
        MAX7219_REG_INTENSITY = 0x0A,
        MAX7219_REG_SCANLIMIT = 0x0B,
        MAX7219_REG_SHUTDOWN  = 0x0C,
        MAX7219_REG_DISPTEST  = 0x0F };

// enumerate the SHUTDOWN modes
// See MAX7219 Datasheet, Table 3, page 7
enum  { OFF = 0,  
        ON  = 1 };

const byte DP     = 0b10000000;  
const byte C      = 0b01001110;  
const byte F      = 0b01000111;
const byte COMMA  = 0b11111111;

// write a value into a max7219 register 
// See MAX7219 Datasheet, Table 1, page 6
void set_register(byte reg, byte value) {
    digitalWrite(MAX7219_CS, LOW);
    shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, reg);
    shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, value);
    digitalWrite(MAX7219_CS, HIGH);
}

// reset the max7219 chip
void resetDisplay() {
    set_register(MAX7219_REG_SHUTDOWN, OFF);   // turn off display
    set_register(MAX7219_REG_DISPTEST, OFF);   // turn off test mode
    set_register(MAX7219_REG_INTENSITY, 0x0D); // display intensity
}

// display the GET TEMP / SET TEMP on the 7-segment display
void display(String s) {
    set_register(MAX7219_REG_SHUTDOWN, OFF);      // turn off display
    set_register(MAX7219_REG_SCANLIMIT, 7);       // scan limit 8 digits
    set_register(MAX7219_REG_DECODE, 0b11111111); // decode only 1 digits

    set_register(1, s.charAt(7));
    set_register(2, s.charAt(6));
    set_register(3, s.charAt(5));
    set_register(4, s.charAt(4));
    set_register(5, s.charAt(3) | DP);
    set_register(6, s.charAt(2));
    set_register(7, s.charAt(1));
    set_register(8, s.charAt(0));

    set_register(MAX7219_REG_SHUTDOWN, ON);   // Turn On display
}

void setup() {
  Serial.begin(115200);
  delay(5000);  // mandatory delay

  Serial.println();
  Serial.println("=========================================");
  Serial.println("## NAMINO ROSSO LAMP THERMO REGULATION ##");
  Serial.println("=========================================");
  Serial.println();
  Serial.flush();

  pinMode(MAX7219_DIN, OUTPUT);   // serial data-in
  pinMode(MAX7219_CS, OUTPUT);    // chip-select, active low    
  pinMode(MAX7219_CLK, OUTPUT);   // serial clock
  digitalWrite(MAX7219_CS, HIGH);

  resetDisplay();                 // reset the MAX2719 display

  // reset namino microcontroller. Industrial side board reset.
  nr.resetSignalMicroprocessor();

  // Opening/closing of communication / initialization of the industrial side interface
  nr.begin(800000U, MISO, MOSI, SCK, SS);

  delay(2000);
}

float round2(float value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

void loop() {
  // potentiometer useful conversion for temperature setpoint
  uint16_t cpot = analogRead(ADC1_CH3);         // PIN 4 on board mark
  float vpot = cpot * (3.3 / 4096.0);
  float tpot = mapf(cpot, 0, 4096, 20, 60);
  setPoint = tpot;
  float_t tc = 0;

  // In the loop() there must be the following functions, which allow the exchange of values with the industrial side board
  nr.readAllRegister();

  // To set the configuration, in the loop inside an if there must be all the initial analog configurations, after having waited for the industrial side to start,
  if (configANOUT && nr.isReady()) {
    configANOUT = false;
    nr.writeRegister(WR_ANALOG_IN_CH01_CONF, ANALOG_IN_CH01_CONF_VALUES::CH01_PT1000);        // temp pt1000 feedback
    nr.writeRegister(WR_ANALOG_IN_CH02_CONF, ANALOG_IN_CH02_CONF_VALUES::CH02_VOLTAGE);       // pot for temp.setpoint (alternate input, unused)

    Serial.printf("fwVersion: 0x%04x boardType: 0x%04x\n", nr.fwVersion(), nr.boardType());
  }
  
  // The industrial side NAMINO board performs some initializations, if the board has finished the initialization and is ready this function returns true
  if (nr.isReady()) {
    // Reading converted to reference units by the channel with a connected thermocouple and/or thermometric probe.
    tc = nr.readPt1000(1);
    // Serial.printf("readPt1000(1) %f RO_ANALOG_IN_CH01 %d setPoint %f\n", tc, nr.loadRegister(RO_ANALOG_IN_CH01), setPoint);
    Serial.printf("vpot: %3.6f V | T set: %3.6f C | T get: %3.6f C\n", vpot, setPoint, tc);
    if (tc < setPoint) {
      nr.writeRele(true);  // Relay status setting.
    } else {
      nr.writeRele(false); // Relay status setting.
    }
  }

  // debug function
  // nr.showRegister();

  // In the loop() there must be the following function, which allow the exchange of values with the industrial side board
  nr.writeAllRegister();  

  // debug functions
  // Serial.printf("RO_NAMINO_ID   %d\n", nr.loadRegister(RO_NAMINO_ID));
  // Serial.printf("RO_LIFE_TIME_L %d\n", nr.loadRegister(RO_LIFE_TIME_L));
  // Serial.printf("RO_LIFE_TIME_H %d\n", nr.loadRegister(RO_LIFE_TIME_H));
  // Serial.printf("RO_ANALOG_IN_CH01 %d\n", nr.loadRegister(RO_ANALOG_IN_CH01));

  // display formatted temperatures on MAX7219
  sprintf(buf, "%4.1f%4.1f", tc, setPoint);
  display(String(buf));

  delay(500);
}
