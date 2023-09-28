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

/* ESP32-S3 datasheet https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf */

#include <Arduino.h>
#include <SPI.h>
#include <Mapf.h>

#include "namino_rosso.h"

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

namino_rosso nr = namino_rosso();
uint16_t loopNr = 1;
float_t setPoint = 30;

#include "Arduino.h"

// LED 8 digit 74HC595
#define DIO_PIN            9    /* data I/O */
#define SCK_PIN           18    /* clock */
#define RCK_PIN           17    /* latch */
#define LED_DIGITS         8

// https://forum.arduino.cc/index.php?topic=518797.0
// ST_CP = SCK
// SH_CP = RCK
// SDI   = DIO
// Common anode
#define DS 		DIO_PIN
#define STCP 	SCK_PIN
#define SHCP 	RCK_PIN
#define SPEED 	500

bool numbersDef[10][8] = 
{
  {1,1,1,1,1,1,0}, //zero
  {0,1,1,0,0,0,0}, //one
  {1,1,0,1,1,0,1}, //two
  {1,1,1,1,0,0,1}, //three
  {0,1,1,0,0,1,1}, //four
  {1,0,1,1,0,1,1}, //five
  {1,0,1,1,1,1,1}, //six
  {1,1,1,0,0,0,0}, //seven
  {1,1,1,1,1,1,1}, //eight
  {1,1,1,1,0,1,1}  //nine
};

bool digitsTable[8][8] =
{
  {0,0,0,0,1,0,0,0}, // first digit
  {0,0,0,0,0,1,0,0}, // second
  {0,0,0,0,0,0,1,0}, // third
  {0,0,0,0,0,0,0,1},  // 8th 
  {1,0,0,0,0,0,0,0}, // forth
  {0,1,0,0,0,0,0,0}, // fifth
  {0,0,1,0,0,0,0,0},  // sixth  
  {0,0,0,1,0,0,0,0} // 7th

   
};

bool display_buffer[32];
void prepareDisplayBuffer(int number, int digit_order, boolean showDot)
{
  for(int index=7; index>=0; index--)
  {
    display_buffer[index] = digitsTable[digit_order-1][index];
  }
  for(int index=14; index>=8; index--)
  {
    display_buffer[index] = !numbersDef[number-1][index]; //because logic is sanity, right?
  }
  if(showDot == true)
    display_buffer[15] = 0;
  else
    display_buffer[15] = 1;
}

void writeDigit(int number, int order, bool showDot = false)
{
  prepareDisplayBuffer(number, order, showDot);
  digitalWrite(SHCP, LOW);
  for(int i=15; i>=0; i--)
  {
      digitalWrite(STCP, LOW);
      digitalWrite(DS, display_buffer[i]); //output LOW - enable segments, HIGH - disable segments
      digitalWrite(STCP, HIGH);
   }
  digitalWrite(SHCP, HIGH);
}

void setup() {
  Serial.begin(115200);
  delay(500);  // mandatory delay

  // pinMode(DS,         OUTPUT);
  // pinMode(STCP,       OUTPUT);
  // pinMode(SHCP,       OUTPUT);
  // digitalWrite(DS,    LOW);
  // digitalWrite(STCP,  LOW);
  // digitalWrite(SHCP,  LOW);

  // reset namino microcontroller. Industrial side board reset.
  nr.resetSignalMicroprocessor();

  // Opening/closing of communication / initialization of the industrial side interface
  nr.begin(800000U, MISO, MOSI, SCK, SS);

  Serial.println();
  Serial.println("=========================================");
  Serial.println("## NAMINO ROSSO LAMP THERMO REGULATION ##");
  Serial.println("=========================================");
  Serial.println();
  delay(2000);

  // // for (int i = 0; i <= 1000; i++) {
  // //   display.set("0000", ALIGN_LEFT);
  // //   display.show(2000);
  // //   display.update();
  // //   Serial.println(i);
  // // }

  // for (int i = 1000; i > 0; i--) {
	// 	// store number and show it for 400ms
	// 	display.set("AAAAAAAA", ALIGN_LEFT);
  //   //display.changeDot(0);
	// 	display.show(400);
	// 	// add dot to stored number and show it for 400ms
	// 	//display.changeDot(0);
	// 	//display.show(400);
	// }

  // display.set("01234567");
}

uint8_t rele = 0;
bool configANOUT = true;
unsigned long msANOUT = millis();
#define AN_OUT_INIT_DELAY_MS      (5 * 1000)

float round2(float value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

void loop() {
  // writeDigit(0, 1,true); // write 0 on first digit
  // writeDigit(1, 2,false); // write 1 on second digit
  // writeDigit(2, 3,false); // write 7 on third digit
  // writeDigit(3, 4, false);
  // writeDigit(4, 5,true);
  // writeDigit(5, 6, false);
  // writeDigit(6, 7,false);
  // writeDigit(7, 8,false);

  // potentiometer useful conversion for temperature setpoint
  uint16_t cpot = analogRead(ADC1_CH3);         // PIN 4 on board mark
  float vpot = cpot * (3.3 / 4096.0);
  float tpot = mapf(cpot, 0, 4096, 20, 60);
  setPoint = tpot;

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
    float_t tc = nr.readPt1000(1);
    // Serial.printf("readPt1000(1) %f RO_ANALOG_IN_CH01 %d setPoint %f\n", tc, nr.loadRegister(RO_ANALOG_IN_CH01), setPoint);
    Serial.printf("vpot: %f V | T set: %f C | T get: %f C\n", vpot, setPoint, tc);
    if (tc < setPoint) {
      nr.writeRele(true);  // Relay status setting.
    } else {
      nr.writeRele(false); // Relay status setting.
    }
  }

  // nr.showRegister();    // debug function
  // // In the loop() there must be the following function, which allow the exchange of values with the industrial side board
  nr.writeAllRegister();  

  // Serial.printf("RO_NAMINO_ID   %d\n", nr.loadRegister(RO_NAMINO_ID));
  // Serial.printf("RO_LIFE_TIME_L %d\n", nr.loadRegister(RO_LIFE_TIME_L));
  // Serial.printf("RO_LIFE_TIME_H %d\n", nr.loadRegister(RO_LIFE_TIME_H));
  // Serial.printf("RO_ANALOG_IN_CH01 %d\n", nr.loadRegister(RO_ANALOG_IN_CH01));

  delay(500);
}
