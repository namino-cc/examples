/*
MIT License

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

#include "./namino_rosso.h"
#include <Arduino.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#ifndef NAMINO_ROSSO_BOARD
#define NAMINO_ROSSO_BOARD
#endif

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif

namino_rosso::namino_rosso() {

}

void namino_rosso::begin(uint32_t clock, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t ss) {
    _miso = miso;
    _mosi = mosi;
    _sck = sck;
    _ss = ss;

    pinMode(_ss, OUTPUT);
    digitalWrite(_ss, HIGH);

    spiSettings = SPISettings(clock, MSBFIRST, SPI_MODE0);
    vspi = new SPIClass(VSPI);
    vspi->begin(_sck, _miso, _mosi, _ss);
    reg[WR_COUNTER] = 0;
}

void namino_rosso::begin(uint32_t clock) {
    _miso = MISO;
    _mosi = MOSI;
    _sck = SCK;
    _ss = SS;

    pinMode(_ss, OUTPUT);
    digitalWrite(_ss, HIGH);

    spiSettings = SPISettings(clock, MSBFIRST, SPI_MODE0);
    vspi = new SPIClass(VSPI);
    vspi->begin(_sck, _miso, _mosi, _ss);
    reg[WR_COUNTER] = 0;
}

void namino_rosso::end() {
    vspi->end();
}

float_t namino_rosso::readAnalogIn(int channel) {
    uint16_t value = 0;
    uint8_t conf = 0;

    switch(channel) {
        case 1:
            value = reg[RO_ANALOG_IN_CH01];
            conf = reg[WR_ANALOG_IN_CH01_CONF];
            switch(conf) {
                case CH01_DISABLED:
                  return 0;
                break;
                case CH01_CURRENT:
                  return value / 1000.0; // place here um conversion
                break;
                case CH01_VOLTAGE:
                  return value / 1000.0; // place here um conversion
                break;
                case CH01_TCJ:
                  return value; // place here um conversion
                break;
                case CH01_TCK:
                  return value; // place here um conversion
                break;
                case CH01_TCT:
                  return value; // place here um conversion
                break;
                case CH01_PT1000:
                  return value / 10.0; // place here um conversion
                break;
                case CH01_NTC:
                  return value / 10.0; // place here um conversion
                break;
                case CH01_PTC:
                  return value / 10.0; // place here um conversion
                break;
                case CH01_LOAD_CELL:
                  return value; // place here um conversion
                break;
                default:
                  return 0;
                break;
            }
        break;
        case 2:
            value = reg[RO_ANALOG_IN_CH02];
            conf = reg[WR_ANALOG_IN_CH02_CONF];
            switch(conf) {
                case CH02_DISABLED:
                  return 0;
                break;
                case CH02_CURRENT:
                  return value / 1000.0; // place here um conversion
                break;
                case CH02_VOLTAGE:
                  return value / 1000.0; // place here um conversion
                break;
                default:
                  return 0;
                break;
            }
        break;
        case 3:
            value = reg[RO_ANALOG_IN_CH03];
            conf = reg[WR_ANALOG_IN_CH03_CONF];
            switch(conf) {
                case CH03_DISABLED:
                  return 0;
                break;
                case CH03_CURRENT:
                  return value / 1000.0; // place here um conversion
                break;
                case CH03_VOLTAGE:
                  return value / 1000.0; // place here um conversion
                break;
                case CH03_TCJ:
                  return value; // place here um conversion
                break;
                case CH03_TCK:
                  return value; // place here um conversion
                break;
                case CH03_TCT:
                  return value; // place here um conversion
                break;
                case CH03_PT1000:
                  return value / 10.0; // place here um conversion
                break;
                case CH03_NTC:
                  return value / 10.0; // place here um conversion
                break;
                case CH03_PTC:
                  return value / 10.0; // place here um conversion
                break;
                case CH03_LOAD_CELL:
                  return value; // place here um conversion
                break;
                default:
                  return 0;
                break;
            }
        break;
        case 4:
            value = reg[RO_ANALOG_IN_CH04];
            conf = reg[WR_ANALOG_IN_CH04_CONF];
            switch(conf) {
                case CH04_DISABLED:
                  return 0;
                break;
                case CH04_CURRENT:
                  return value / 1000.0; // place here um conversion
                break;
                case CH04_VOLTAGE:
                  return value / 1000.0; // place here um conversion
                break;
                default:
                  return 0;
                break;
            }
        break;
        case 5:
            value = reg[RO_ANALOG_IN_CH05];
            conf = reg[WR_ANALOG_IN_CH05_CONF];
            switch(conf) {
                case CH05_DISABLED:
                  return 0;
                break;
                case CH05_CURRENT:
                  return value / 1000.0; // place here um conversion
                break;
                case CH05_VOLTAGE:
                  return value / 1000.0; // place here um conversion
                break;
                case CH05_TCJ:
                  return value; // place here um conversion
                break;
                case CH05_TCK:
                  return value; // place here um conversion
                break;
                case CH05_TCT:
                  return value; // place here um conversion
                break;
                case CH05_PT1000:
                  return value / 10.0; // place here um conversion
                break;
                case CH05_NTC:
                  return value / 10.0; // place here um conversion
                break;
                case CH05_PTC:
                  return value / 10.0; // place here um conversion
                break;
                case CH05_LOAD_CELL:
                  return value; // place here um conversion
                break;
                default:
                  return 0;
                break;
            }
        break;
        case 6:
            value = reg[RO_ANALOG_IN_CH06];
            conf = reg[WR_ANALOG_IN_CH06_CONF];
            switch(conf) {
                case CH06_DISABLED:
                  return 0;
                break;
                case CH06_CURRENT:
                  return value / 1000.0; // place here um conversion
                break;
                case CH06_VOLTAGE:
                  return value / 1000.0; // place here um conversion
                break;
                default:
                  return 0;
                break;
            }
        break;
        case 7:
            value = reg[RO_ANALOG_IN_CH07];
            conf = reg[WR_ANALOG_IN_CH07_CONF];
            switch(conf) {
                case CH07_DISABLED:
                  return 0;
                break;
                case CH07_CURRENT:
                  return value / 1000.0; // place here um conversion
                break;
                case CH07_VOLTAGE:
                  return value / 1000.0; // place here um conversion
                break;
                case CH07_TCJ:
                  return value; // place here um conversion
                break;
                case CH07_TCK:
                  return value; // place here um conversion
                break;
                case CH07_TCT:
                  return value; // place here um conversion
                break;
                case CH07_PT1000:
                  return value / 10.0; // place here um conversion
                break;
                case CH07_NTC:
                  return value / 10.0; // place here um conversion
                break;
                case CH07_PTC:
                  return value / 10.0; // place here um conversion
                break;
                case CH07_LOAD_CELL:
                  return value; // place here um conversion
                break;
                default:
                  return 0;
                break;
            }
        break;
        case 8:
            value = reg[RO_ANALOG_IN_CH08];
            conf = reg[WR_ANALOG_IN_CH08_CONF];
            switch(conf) {
                case CH08_DISABLED:
                  return 0;
                break;
                case CH08_CURRENT:
                  return value / 1000.0; // place here um conversion
                break;
                case CH08_VOLTAGE:
                  return value / 1000.0; // place here um conversion
                break;
                default:
                  return 0;
                break;
            }
        break;
        default:
            return 0;
        break;
    }
    return 0;
}

int32_t namino_rosso::readEncoder() {
    return (reg[RO_ENCODER_COUNTER_H] << 16) | reg[RO_ENCODER_COUNTER_L];
}

uint32_t namino_rosso::readLifeTime() {
    return (reg[RO_LIFE_TIME_H] << 16) | reg[RO_LIFE_TIME_L];
}

uint16_t namino_rosso::fwVersion() {
  return (reg[RO_NAMINO_ID]);
}

uint16_t namino_rosso::boardType() {
  return (reg[RO_ANALOG_FRONT_END_ID]);
}

bool namino_rosso::isReady() {
    return reg[RO_ANALOG_FRONT_END_ID] == 0x1338;
}

void namino_rosso::resetEncoderCounter() {
    reg[WR_ENCODER_SET_L] = 0;
    reg[WR_ENCODER_SET_H] = 0;
    reg[WR_CONFIG_REGISTER] = 1;
}

void namino_rosso::setEncoderCounter(int32_t value) {
    reg[WR_ENCODER_SET_L] = value & 0x0000FFFF;
    reg[WR_ENCODER_SET_H] = (value & 0xFFFF0000) >> 16;
    reg[WR_CONFIG_REGISTER] = 1;
}

bool namino_rosso::readDigIn(int channel) {
    if (channel >= 1 && channel <=8) {
        channel--;
        return (reg[RO_DIG_IN_1_16] & (1 << channel)) >> channel;
    }
    return false;
}

bool namino_rosso::readDigOut(int channel) {
    if (channel >= 1 && channel <=8) {
        channel--;
        return (reg[WR_DIG_OUT_1_16] & (1 << channel)) >> channel;
    }
    return false;
}

bool namino_rosso::readEncoderSwitch() {
    return reg[RO_ENCODER_SWITCH] == 1;
}

float_t namino_rosso::readPt1000(int channel) {
  if (channel == 1) {
        return readAnalogIn(1);
    } else if (channel == 2) {
        return readAnalogIn(3);
    } else if (channel == 3) {
        return readAnalogIn(5);
    } else if (channel == 4) {
        return readAnalogIn(7);
    }
    return 0;
}

float_t namino_rosso::readNTC(int channel) {
  if (channel == 1) {
        return readAnalogIn(1);
    } else if (channel == 2) {
        return readAnalogIn(3);
    } else if (channel == 3) {
        return readAnalogIn(5);
    } else if (channel == 4) {
        return readAnalogIn(7);
    }
    return 0;
}

float_t namino_rosso::readTCJ(int channel) {
  if (channel == 1) {
        return readAnalogIn(1);
    } else if (channel == 2) {
        return readAnalogIn(3);
    } else if (channel == 3) {
        return readAnalogIn(5);
    } else if (channel == 4) {
        return readAnalogIn(7);
    }
    return 0;
}

float_t namino_rosso::readTCK(int channel) {
  if (channel == 1) {
        return readAnalogIn(1);
    } else if (channel == 2) {
        return readAnalogIn(3);
    } else if (channel == 3) {
        return readAnalogIn(5);
    } else if (channel == 4) {
        return readAnalogIn(7);
    }
    return 0;
}

float_t namino_rosso::readTCT(int channel) {
  if (channel == 1) {
        return readAnalogIn(1);
    } else if (channel == 2) {
        return readAnalogIn(3);
    } else if (channel == 3) {
        return readAnalogIn(5);
    } else if (channel == 4) {
        return readAnalogIn(7);
    }
    return 0;
}

float_t namino_rosso::readPTC(int channel) {
  if (channel == 1) {
        return readAnalogIn(1);
    } else if (channel == 2) {
        return readAnalogIn(3);
    } else if (channel == 3) {
        return readAnalogIn(5);
    } else if (channel == 4) {
        return readAnalogIn(7);
    }
    return 0;
}

float_t namino_rosso::readLoadCell(int channel) {
  if (channel == 1) {
        return readAnalogIn(1);
    } else if (channel == 2) {
        return readAnalogIn(5);
    }
    return 0;
}

float_t namino_rosso::tBoard() {
    return reg[RO_TBOARD];
}

bool namino_rosso::writeAnalogInConf(int channel, uint8_t conf) {
    if (channel >= 1 && channel <=8) {
        reg[WR_ANALOG_IN_CH01_CONF + channel - 1] = conf;
        return true;
    }
    return false;
}

void namino_rosso::writeAnalogOut(float_t value) {
    switch(reg[WR_ANALOG_OUT_CH01_CONF]) {
        case OUT_CH01_DISABLED:
            reg[WR_ANALOG_OUT] = 0;
        break;
        case OUT_CH01_CURRENT:
            reg[WR_ANALOG_OUT] = value * 100; // place here um conversion
        break;
        case OUT_CH01_VOLTAGE:
            reg[WR_ANALOG_OUT] = value * 100; // place here um conversion
        break;
        default:
        break;
    }
}

void namino_rosso::writeAnalogOutConf(ANALOG_OUT_CH01_CONF_VALUES conf) {
    reg[WR_ANALOG_OUT_CH01_CONF] = conf;
}

void namino_rosso::writeRegister(uint8_t address, uint16_t value) {
    reg[address] = value;
}

bool namino_rosso::writeDigOut(int channel, bool value) {
    if (channel >= 1 && channel <= 8) {
      channel--;
      if (value) {
        reg[WR_DIG_OUT_1_16] |= 1UL << channel;
      } else {
        reg[WR_DIG_OUT_1_16] &= ~(1UL << channel);
      }
      return true;
    }
    return false;
}

void namino_rosso::writeRele(bool value) {
    reg[WR_RELE] = value;
}

void namino_rosso::setupStep(uint16_t frequency, uint32_t stepsNumber) {
  writeStepFreq(frequency);
  writeStepNumber(stepsNumber);
}

void namino_rosso::writeStepNumber(uint32_t value) {
    reg[WR_STEP_NUMBER_L] = value & 0x0000FFFF;
    reg[WR_STEP_NUMBER_H] = (value & 0xFFFF0000) >> 16;
}

void namino_rosso::writeStepFreq(uint16_t value) {
    reg[WR_STEP_FREQ] = value;
}

void namino_rosso::startStepper() {
  reg[WR_STEP_COMMAND] = 0x10;
}

void namino_rosso::stopStepper() {
  reg[WR_STEP_COMMAND] = 0x20;
}

void namino_rosso::readAllRegister() {
    reg[WR_WRITE_REQUEST] = 0;    // signal board firmware write direction indicator

    vspi->beginTransaction(spiSettings);
    digitalWrite(_ss, LOW);

    uint16_t *rp = reg;
    for (uint16_t i = 0; i <= WR_LAST_INDEX; i++) {
        *rp = vspi->transfer(0);
        *rp = (*rp << 8) | vspi->transfer(0);
        rp++;
    }

    digitalWrite(_ss, HIGH);
    vspi->endTransaction();
}

void namino_rosso::resetSignalMicroprocessor() {
  pinMode(NM_RESET, OUTPUT);
  digitalWrite(NM_RESET, !HIGH);
  delay(200);
  digitalWrite(NM_RESET, !LOW);
  delay(200);
  digitalWrite(NM_RESET, !HIGH);
}

void namino_rosso::writeAllRegister() {
    delay(10);
    reg[WR_WRITE_REQUEST] = 1;    // signal board firmware write direction indicator
    reg[WR_COUNTER]++;

    vspi->beginTransaction(spiSettings);
    digitalWrite(_ss, LOW);

    uint16_t *wp = reg;
    for (uint16_t i = 0; i <= WR_LAST_INDEX; i++) {
        vspi->transfer((*wp & 0xFF00) >> 8);
        vspi->transfer(*wp & 0xFF);
        wp++;
    }

    digitalWrite(_ss, HIGH);
    vspi->endTransaction();
}

void namino_rosso::transferAllRegister() {
    reg[WR_WRITE_REQUEST] = 1;    // signal board firmware write direction indicator
    reg[WR_COUNTER]++;

    vspi->beginTransaction(spiSettings);
    digitalWrite(_ss, LOW);

    uint16_t *wp = reg;
    for (uint16_t i = 0; i <= WR_LAST_INDEX; i++) {
        // DEBUG: Serial.printf("REG W %02d: 0x%04x ", i, *wp);
        uint16_t h = vspi->transfer((*wp & 0xFF00) >> 8);
        uint16_t l = vspi->transfer(*wp & 0xFF);
        // DEBUG: Serial.printf("H 0x%02x L 0x%02x ", h, l);
        *wp = (h << 8) | l;
        // DEBUG: Serial.printf("REG R %02d: 0x%04x ", i, *wp);
        // DEBUG: Serial.println();
        wp++;
    }

    digitalWrite(_ss, HIGH);
    vspi->endTransaction();
}

void namino_rosso::resetWRegister() {
    for (uint8_t i = WR_FIRST_INDEX; i <= WR_LAST_INDEX; i++) {
        reg[i] = 0;
    }
}

void namino_rosso::saveRegister(uint8_t address, uint16_t value) {
    reg[address] = value;
}

uint16_t namino_rosso::loadRegister(uint8_t address) {
    return reg[address];
}

void namino_rosso::showRegister() {
    uint16_t *rp = reg;
    for (uint8_t i = 0; i <= WR_LAST_INDEX; i += 4) {
            Serial.printf("REG %02d: 0x%04x REG %02d: 0x%04x REG %02d: 0x%04x REG %02d: 0x%04x\n",
                i + 0, *(rp + 0), 
                i + 1, *(rp + 1), 
                i + 2, *(rp + 2), 
                i + 3, *(rp + 3));
            rp += 4;
    }
    Serial.println("-----------------------------------------------------------");

    // uint16_t *wp = reg;
    // for (uint8_t i = WR_FIRST_INDEX; i <= WR_LAST_INDEX; i += 4) {
    //         Serial.printf("REG %02d: 0x%04x REG %02d: 0x%04x REG %02d: 0x%04x REG %02d: 0x%04x\n",
    //             i + 0, *(wp + 0), 
    //             i + 1, *(wp + 1), 
    //             i + 2, *(wp + 2), 
    //             i + 3, *(wp + 3));
    //         wp += 4;
    // }
    // Serial.println("W----------------------------------------------------------");
}

void namino_rosso::printValues() {
    static const char *separator = "----------------------------------|-------------------------|------------------------|-----------------------|--------------";

    for (uint8_t i = RO_ANALOG_IN_CH01; i <= RO_ANALOG_IN_CH08; i += 4) {
            Serial.printf("ANALOG IN CH %2d: %f CONF %2d | CH %2d: %f CONF %2d | CH %2d: %f CONF %d | CH %2d: %f CONF %d\n",
                i + 0 - RO_ANALOG_IN_CH01 + 1, readAnalogIn(i + 0), reg[WR_ANALOG_IN_CH01_CONF + i + 0],
                i + 1 - RO_ANALOG_IN_CH01 + 1, readAnalogIn(i + 1), reg[WR_ANALOG_IN_CH01_CONF + i + 1],
                i + 2 - RO_ANALOG_IN_CH01 + 1, readAnalogIn(i + 2), reg[WR_ANALOG_IN_CH01_CONF + i + 2],
                i + 3 - RO_ANALOG_IN_CH01 + 1, readAnalogIn(i + 3), reg[WR_ANALOG_IN_CH01_CONF + i + 3]);
    }
    Serial.println(separator);
    for (uint8_t i = 1; i <= 8; i += 4) {
            Serial.printf("DIGITAL IN CH %2d: %d               | CH %2d: %d                | CH %2d: %d               | CH %2d: %d\n",
                i + 0, readDigIn(i + 0),
                i + 1, readDigIn(i + 1),
                i + 2, readDigIn(i + 2),
                i + 3, readDigIn(i + 3));
    }
    Serial.println(separator);
    for (uint8_t i = 1; i <= 8; i += 4) {
            Serial.printf("DIGITAL OUT CH %2d: %d              | CH %2d: %d                | CH %2d: %d               | CH %2d: %d\n",
                i + 0, readDigOut(i + 0),
                i + 1, readDigOut(i + 1),
                i + 2, readDigOut(i + 2),
                i + 3, readDigOut(i + 3));
    }
    Serial.println(separator);
    Serial.printf("Pt1000 #1 %f                | Pt1000 #2 %f      | PTC #1 %f        | PTC #2 %f       | tBoard %f\n", readPt1000(1),readPt1000(2), readPTC(1), readPTC(2), tBoard());   
    Serial.println(separator);
    Serial.printf("ENCODER %9d                 | ENC.SWITCH %1d \n", readEncoder(), readEncoderSwitch());
    Serial.println(separator);
}
