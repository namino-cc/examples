/* namino rosso blinkLed example

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

#include <Arduino.h>

#include <namino_rosso.h>
#include <RTClib.h>             // RTC      Library

#ifdef NAMINO_ROSSO_BOARD
#undef NAMINO_ROSSO_BOARD
#endif

// Namino Rosso Instance
#define LED_BUILTIN             1       // Out Channel for Built-in Led
#define NAMINO_I2C_SCL          0       // Namino Rosso I2C SCL Pin
#define NAMINO_I2C_SDA          1       // Namino Rosso I2C SDA Pin

namino_rosso nr = namino_rosso();
bool            configANIN = true;
bool            naminoReady = false;
float           externalTemperature = 0.0;
float           internalTemperature = 0.0;

// RTC Object
RTC_DS1307      rtc;
bool            rtcFound = false;


// Time counters (in millis)
#define LOOP_PERIOD           1000            // 1000ms loop period
#define TIME_PERIOD           1000            // Read period of RTC
#define FIELD_READ_PERIOD     4000            // 4s interval of field read

uint32_t      lastLoop = 0;
uint32_t      secsFromBoot = 0;               // Seconds from Namino Rosso configuration
uint32_t      lastRTCReadTime = 0;            // Last RTC Read in millis()
uint32_t      lastFieldRead = 0;              // Last field Read in millis()

void setup() {
  Serial.begin(115200);
  delay(500);  // mandatory delay

  Serial.println();
  Serial.println("============================================");
  Serial.println("## NAMINO ROSSO Blink Onboard Led example ##");
  Serial.println("============================================");
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

  // Init Namino Rosso Board
  Serial.println("Starting Namino industrial Board");
  delay(2000);
  // reset namino microcontroller
  nr.resetSignalMicroprocessor();
  nr.begin(800000U, MISO, MOSI, SCK, SS);
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
  // Read Industrial Registers
  nr.readAllRegister();
  naminoReady = nr.isReady();

  // Analog Config (only once at boot when Namino is Ready)
  if (configANIN && naminoReady) {
    // Configure analog Input (2 x PT1000 on channel 1 and 3, to be read as Probe)  
    nr.writeRegister(WR_ANALOG_IN_CH01_CONF, ANALOG_IN_CH01_CONF_VALUES::CH01_PT1000);
    nr.writeRegister(WR_ANALOG_IN_CH03_CONF, ANALOG_IN_CH03_CONF_VALUES::CH03_PT1000);
    nr.writeRegister(WR_ANALOG_IN_CH05_CONF, ANALOG_IN_CH05_CONF_VALUES::CH05_DISABLED );
    nr.writeRegister(WR_ANALOG_IN_CH06_CONF, ANALOG_IN_CH06_CONF_VALUES::CH06_DISABLED );
    nr.writeRegister(WR_ANALOG_IN_CH07_CONF, ANALOG_IN_CH07_CONF_VALUES::CH07_DISABLED );
    nr.writeRegister(WR_ANALOG_IN_CH08_CONF, ANALOG_IN_CH08_CONF_VALUES::CH08_DISABLED );
    // Configure Analog Output in voltage [V]
    nr.writeRegister(WR_ANALOG_OUT_CH01_CONF, ANALOG_OUT_CH01_CONF_VALUES::OUT_CH01_VOLTAGE);
    nr.writeAnalogOut(0.0); // output voltage
    nr.writeDigOut(LED_BUILTIN, true);
    nr.writeAllRegister();
    delay(200);
    secsFromBoot = 1;
    Serial.println("NR Analog config completed");
    Serial.printf("fwVersion: [0x%04x] boardType: [0x%04x] LifeTime: [%d]\n", nr.fwVersion(), nr.boardType(), secsFromBoot);
    configANIN = false;
    return;
  }

  // Namino Ready ?
  if (not naminoReady)  {
    Serial.println("NR NOT Ready");
    return;
  }
  // Seconds from boot
  secsFromBoot = nr.readLifeTime();

  // Read RTC (every second)
  if (rtcFound && ( (theTime - lastRTCReadTime) >= TIME_PERIOD))  {
    // Reading RTC
    lastRTCReadTime = theTime;
    DateTime now = rtc.now();
    sprintf(buf, "%01d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year() - 2000);
    Serial.printf("Current Date and Time: [%s]\n", buf);
  }

  // Read Fields Values (PT1000 Temperature Probes)
  if (naminoReady && (theTime - lastFieldRead) >= FIELD_READ_PERIOD)   {
    lastFieldRead = theTime;
    externalTemperature = nr.readPt1000(1);
    internalTemperature = nr.readPt1000(2);
    sprintf(buf,"External Temperature: %.1f Internal Temperature: %.1f", externalTemperature, internalTemperature);
    Serial.println(buf);
  }
  // Read Led Status
  ledIsOn = nr.readDigOut(LED_BUILTIN);
  Serial.printf("NR Lifetime: [%d] Led Status:[%s]\n", secsFromBoot, ledIsOn ? "ON" : "OFF");

  // Reverse Led Status
  nr.writeDigOut(LED_BUILTIN, not ledIsOn);

  // Debug infos:
  // nr.showRegister();    

  // Update the industrial registers, mandatory to make the writing of the new registers effective
  if (naminoReady && secsFromBoot)  {
    nr.writeAllRegister();
  }
  delay(50);
}
