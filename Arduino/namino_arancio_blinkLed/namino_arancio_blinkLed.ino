/* namino arancio blinkLed example

  Turn ON and OFF repeatedly on board TEST LED, connected in parallel to DIG OUT1 (digital output 1) - Side B – J15


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


#ifdef NAMINO_ARANCIO_BOARD
#undef NAMINO_ARANCIO_BOARD
#endif

#include <namino_arancio.h>
#include <RTClib.h>             // RTC      Library


// Namino Arancio Instance
#define LED_BUILTIN             1       // Out Channel for Built-in Led
#define NAMINO_I2C_SCL          0       // Namino Arancio I2C SCL Pin
#define NAMINO_I2C_SDA          1       // Namino Arancio I2C SDA Pin

namino_arancio na = namino_arancio();
bool            configANIN = true;
bool            naminoReady = false;

// RTC Object
RTC_DS1307      rtc;
bool            rtcFound = false;

// Time counters (in millis)
#define LOOP_PERIOD           1000            // 1000ms loop period
#define TIME_PERIOD           1000            // Read period of RTC

uint32_t      lastLoop = 0;
uint32_t      loopCounter = 0;
uint32_t      secsFromBoot = 0;               // Seconds from Namino Arancio configuration
uint32_t      lastRTCReadTime = 0;            // Last RTC Read in millis()

void setup() {
  Serial.begin(115200);
  delay(500);  // mandatory delay

  Serial.println();
  Serial.println("==============================================");
  Serial.println("## NAMINO ARANCIO Blink Onboard Led example ##");
  Serial.println("==============================================");
  Serial.println();
  Serial.print("SPI MOSI: ");
  Serial.println(MOSI);
  Serial.print("SPI MISO: ");
  Serial.println(MISO);
  Serial.print("SPI SCK: ");
  Serial.println(SCK);
  Serial.print("SPI CS: ");
  Serial.println(SS);  
  Serial.print("Internal LED Dig.OUT: ");
  Serial.println(LED_BUILTIN);
  Serial.println("-------------------");
  Serial.println();
  delay(2000);
  // RTC Setup
  Serial.println("Starting RTC");
  Wire.begin(NAMINO_I2C_SDA, NAMINO_I2C_SCL);
  if (rtc.begin())  {
    // Adjust RTC Date and time to Compile time if needed
    if (rtc.now().year() < 2023)  {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    Serial.println("RTC Init Done");
    rtcFound = true;
  }
  else  {
    Serial.println("No RTC found!!");
  }

  // Init Namino Arancio Board
  Serial.println("Starting Namino industrial Board");
  delay(2000);
  // reset namino microcontroller
  na.resetSignalMicroprocessor();
  na.begin(800000U, MISO, MOSI, SCK, SS);
  Serial.println("Waiting Namino industrial Board Analog Configuration");
}

void loop() {
  uint32_t  theTime = millis();
  bool      ledIsOn;
char        buf[80];

  // limit loop period
  if ((theTime - lastLoop) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;
  loopCounter++;
  // Read Industrial Registers
  na.readAllRegister();
  naminoReady = na.isReady();

  // Analog Config (only once at boot when Namino is Ready)
  if (configANIN && naminoReady) {
    // Configure analog Input (2 x PT1000 on channel 1 and 3, to be read as Probe)  
    na.writeRegister(WR_ANALOG_IN_CH01_CONF, ANALOG_IN_CH01_CONF_VALUES::CH01_DISABLED);
    na.writeRegister(WR_ANALOG_IN_CH02_CONF, ANALOG_IN_CH02_CONF_VALUES::CH02_DISABLED);
    na.writeRegister(WR_ANALOG_IN_CH03_CONF, ANALOG_IN_CH03_CONF_VALUES::CH03_DISABLED);
    na.writeRegister(WR_ANALOG_IN_CH04_CONF, ANALOG_IN_CH04_CONF_VALUES::CH04_DISABLED);
    // Configure Analog Output in voltage [V]
    na.writeRegister(WR_ANALOG_OUT_CH01_CONF, ANALOG_OUT_CH01_CONF_VALUES::OUT_CH01_VOLTAGE);
    na.writeAnalogOut(0.0); // output voltage
    na.writeDigOut(LED_BUILTIN, true);
    na.writeAllRegister();
    delay(200);
    secsFromBoot = 1;
    Serial.println("Namino Arancio Analog config completed");
    Serial.printf("fwVersion: [0x%04x] boardType: [0x%04x] LifeTime: [%d]\n", na.fwVersion(), na.boardType(), secsFromBoot);
    configANIN = false;
    return;
  }

  // Namino Ready ?
  if (not naminoReady)  {
    Serial.println("NR NOT Ready");
    return;
  }
  // Seconds from boot
  secsFromBoot = na.readLifeTime();

  // Read RTC (every second)
  if (rtcFound && ( (theTime - lastRTCReadTime) >= TIME_PERIOD))  {
    // Reading RTC
    lastRTCReadTime = theTime;
    DateTime now = rtc.now();
    sprintf(buf, "%01d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year() - 2000);
    Serial.printf("Current Date and Time: [%s]\n", buf);
  }

  // Read Led Status
  ledIsOn = na.readDigOut(LED_BUILTIN);
  Serial.printf("Loop: [%d] NR Lifetime: [%d] Led Status:[%s]\n", loopCounter, secsFromBoot, ledIsOn ? "ON" : "OFF");

  // Reverse Led Status
  na.writeDigOut(LED_BUILTIN, not ledIsOn);

  // Debug infos:
  // na.showRegister();    

  // Update the industrial registers, mandatory to make the writing of the new registers effective
  if (naminoReady && secsFromBoot)  {
    na.writeAllRegister();
  }
  delay(50);
}
