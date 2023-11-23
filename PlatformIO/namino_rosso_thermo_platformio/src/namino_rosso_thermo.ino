/* namino rosso thermo regulator sample

Copyright (c) 2023 Namino Team, version: 1.0.18 @ 2023-07-07

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

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

#include "namino_rosso.h"


namino_rosso nr = namino_rosso();
uint16_t loopNr = 1;
float_t setPoint = 60;


void setup() {
  Serial.begin(115200);
  delay(500);  // mandatory delay

  // reset namino microcontroller. Industrial side board reset.
  nr.resetSignalMicroprocessor();

  // Opening/closing of communication / initialization of the industrial side interface
  nr.begin(800000U, MISO, MOSI, SCK, SS);

  Serial.println();
  Serial.println("=========================================");
  Serial.println("## NAMINO ROSSO THERMO REG SAMPLE CODE ##");
  Serial.println("=========================================");
  Serial.println();
  delay(2000);
}

uint8_t rele = 0;
bool configANOUT = true;
unsigned long msANOUT = millis();
unsigned long loopCycleUs = 0;
unsigned long loopCycleUsMax = ULONG_MAX;
unsigned long loopCycleUsMin = 0;
#define AN_OUT_INIT_DELAY_MS      (5 * 1000)


void loop() {
  loopCycleUs = micros();
  // In the loop() there must be the following functions, which allow the exchange of values with the industrial side board
  nr.readAllRegister();

  // AN OUT delay configuration
  // To set the configuration, in the loop inside an if there must be all the initial analog configurations, after having waited for the industrial side to start,
  if (configANOUT && nr.isReady() && (millis() - msANOUT > AN_OUT_INIT_DELAY_MS)) {
    configANOUT = false;
    nr.writeRegister(WR_ANALOG_IN_CH01_CONF, ANALOG_IN_CH01_CONF_VALUES::CH01_TCK);
    nr.writeRegister(WR_ANALOG_IN_CH02_CONF, ANALOG_IN_CH02_CONF_VALUES::CH02_VOLTAGE);
    nr.writeRegister(WR_ANALOG_IN_CH03_CONF, ANALOG_IN_CH03_CONF_VALUES::CH03_VOLTAGE);
    nr.writeRegister(WR_ANALOG_IN_CH04_CONF, ANALOG_IN_CH04_CONF_VALUES::CH04_VOLTAGE);

    nr.writeRegister(WR_ANALOG_OUT_CH01_CONF, ANALOG_OUT_CH01_CONF_VALUES::OUT_CH01_VOLTAGE);
    nr.writeAnalogOut(0.0); // output voltage
    Serial.println("AN OUT: config completed");
    Serial.printf("fwVersion: 0x%04x boardType: 0x%04x\n", nr.fwVersion(), nr.boardType());
  }
  
  // The industrial side NAMINO board performs some initializations, if the board has finished the initialization and is ready this function returns true
  if (nr.isReady()) {
    // Reading converted to reference units by the channel with a connected thermocouple and/or thermometric probe.
    float_t tc = nr.readTCK(1);
    Serial.printf("readTCK(1) %f RO_ANALOG_IN_CH01 %d setPoint %f\n", tc, nr.loadRegister(RO_ANALOG_IN_CH01), setPoint);
    if (tc < setPoint) {
      nr.writeRele(true); // Relay status setting.
    } else {
      nr.writeRele(false); // Relay status setting.
    }
  }

  nr.showRegister();    // debug function
  // In the loop() there must be the following function, which allow the exchange of values with the industrial side board
  nr.writeAllRegister();  

  // Serial.printf("RO_NAMINO_ID   %d\n", nr.loadRegister(RO_NAMINO_ID));
  // Serial.printf("RO_LIFE_TIME_L %d\n", nr.loadRegister(RO_LIFE_TIME_L));
  // Serial.printf("RO_LIFE_TIME_H %d\n", nr.loadRegister(RO_LIFE_TIME_H));
  Serial.printf("RO_ANALOG_IN_CH01 %d\n", nr.loadRegister(RO_ANALOG_IN_CH01));

  delay(500);
  // loop cycle calculation
  loopCycleUs = micros() - loopCycleUs;
  if (loopCycleUs < loopCycleUsMax) {
    loopCycleUsMax = loopCycleUs;
  }
  if (loopCycleUs > loopCycleUsMin) {
    loopCycleUsMin = loopCycleUs;
  }
  Serial.printf("loop cycle timing us min %d max %d\n", loopCycleUsMax, loopCycleUsMin);
}
