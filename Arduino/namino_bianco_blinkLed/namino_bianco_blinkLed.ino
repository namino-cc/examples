/* namino bianco blinkLed example

  Turn ON and OFF repeatedly on board TEST LED, connected to pin GPIO47


Copyright (c) 2023 Namino Team, version: 1.0.19 @ 2023-11-07

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


#ifdef NAMINO_BIANCO_BOARD
#undef NAMINO_BIANCO_BOARD
#endif

#ifdef Pins_Arduino_h
#undef Pins_Arduino_h
#endif

#include "namino_bianco_pins.h"
#include <Arduino.h>

// Time counters (in millis)
#define LOOP_PERIOD           1000            // 1000ms loop period
uint32_t      lastLoop = 0;
uint32_t      secsFromBoot = 0;               // Seconds from Namino Rosso configuration


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("=============================================");
  Serial.println("## NAMINO BIANCO Blink Onboard Led example ##");
  Serial.println("=============================================");
  Serial.println();
  Serial.print("SPI MOSI: ");
  Serial.println(MOSI);
  Serial.print("SPI MISO: ");
  Serial.println(MISO);
  Serial.print("SPI SCK: ");
  Serial.println(SCK);
  Serial.print("SPI CS: ");
  Serial.println(SS);  
  Serial.println();
  delay(2000);

}

void loop() {
  uint32_t  theTime = millis();
  bool      ledIsOn;
  char      buf[80];

  // limit loop period
  if ((theTime - lastLoop) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;
  // Seconds from boot
  secsFromBoot = lastLoop / 1000;
  // Read Led Status
  ledIsOn = digitalRead(LED_BUILTIN);
  // Reverse Led Status
  digitalWrite(LED_BUILTIN, not ledIsOn);
  Serial.printf("NB Lifetime: [%d] Led Status:[%s]\n", secsFromBoot, ledIsOn ? "ON" : "OFF");
}
