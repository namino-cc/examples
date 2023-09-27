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

#ifndef NAMINO_ROSSO_H
#define NAMINO_ROSSO_H

#include <Arduino.h>
#include <SPI.h>

/* INDUSTRIAL SIDE memory map register */
typedef enum {
  // Read only registers
  RO_NAMINO_ID            = 0,
  RO_LIFE_TIME_L          = 1,
  RO_LIFE_TIME_H          = 2,
  RO_RESERVED_3           = 3,
  RO_ANALOG_IN_CH01       = 4,
  RO_ANALOG_IN_CH02       = 5,
  RO_ANALOG_IN_CH03       = 6,
  RO_ANALOG_IN_CH04       = 7,
  RO_ANALOG_IN_CH05       = 8,
  RO_ANALOG_IN_CH06       = 9,
  RO_ANALOG_IN_CH07       = 10,
  RO_ANALOG_IN_CH08       = 11,
  RO_RESERVED_12          = 12,
  RO_TBOARD               = 13,
  RO_DIG_IN_1_16          = 14,
  RO_ENCODER_COUNTER_L    = 15,
  RO_ENCODER_COUNTER_H    = 16,
  RO_ENCODER_SWITCH       = 17,
  RO_ANALOG_FRONT_END_ID  = 20,
  RO_RESERVED_21          = 21,
  RO_RESERVED_22          = 22,
  RO_RESERVED_23          = 23,
  RO_RESERVED_24          = 24,
  RO_RESERVED_25          = 25,
  RO_RESERVED_26          = 26,
  RO_RESERVED_27          = 27,
  RO_RESERVED_28          = 28,
  RO_RESERVED_29          = 29,
  RO_RESERVED_30          = 30,
  RO_LAST_INDEX           = 31,
  // Write registers
  WR_FIRST_INDEX          = 32,
  WR_WRITE_REQUEST        = WR_FIRST_INDEX + 0,   // 32
  WR_ENCODER_SET_L        = WR_FIRST_INDEX + 1,   // 33
  WR_ENCODER_SET_H        = WR_FIRST_INDEX + 2,   // 34
  WR_DIG_OUT_1_16         = WR_FIRST_INDEX + 3,   // 35
  WR_RELE                 = WR_FIRST_INDEX + 4,   // 36
  WR_STEP_NUMBER_L        = WR_FIRST_INDEX + 5,   // 37
  WR_STEP_NUMBER_H        = WR_FIRST_INDEX + 6,   // 38
  WR_STEP_FREQ            = WR_FIRST_INDEX + 7,   // 39
  WR_STEP_RESERVED        = WR_FIRST_INDEX + 8,   // 40
  WR_STEP_COMMAND         = WR_FIRST_INDEX + 9,   // 41
  WR_ANALOG_OUT           = WR_FIRST_INDEX + 10,  // 42
  WR_ANALOG_IN_CH01_CONF  = WR_FIRST_INDEX + 11,  // 43
  WR_ANALOG_IN_CH02_CONF  = WR_FIRST_INDEX + 12,  // 44
  WR_ANALOG_IN_CH03_CONF  = WR_FIRST_INDEX + 13,  // 45
  WR_ANALOG_IN_CH04_CONF  = WR_FIRST_INDEX + 14,  // 46
  WR_ANALOG_IN_CH05_CONF  = WR_FIRST_INDEX + 15,  // 47
  WR_ANALOG_IN_CH06_CONF  = WR_FIRST_INDEX + 16,  // 48
  WR_ANALOG_IN_CH07_CONF  = WR_FIRST_INDEX + 17,  // 49
  WR_ANALOG_IN_CH08_CONF  = WR_FIRST_INDEX + 18,  // 50
  WR_ANALOG_OUT_CH01_CONF = WR_FIRST_INDEX + 19,  // 51
  WR_COUNTER              = WR_FIRST_INDEX + 20,  // 52
  WR_CONFIG_REGISTER      = WR_FIRST_INDEX + 21,  // 53
  WR_RESERVED_54          = 54,
  WR_RESERVED_55          = 55,
  WR_RESERVED_56          = 56,
  WR_RESERVED_57          = 57,
  WR_RESERVED_58          = 58,
  WR_RESERVED_59          = 59,
  WR_RESERVED_60          = 60,
  WR_RESERVED_61          = 61,
  WR_RESERVED_62          = 62,
  WR_LAST_INDEX           = 63,
} NAMINO_ROSSO_REGISTERS;


typedef enum {
  CH01_DISABLED    = 0,
  CH01_CURRENT     = 1,
  CH01_VOLTAGE     = 2,
  CH01_TCJ         = 10,
  CH01_TCK         = 11,
  CH01_TCT         = 12,
  CH01_PT1000      = 20,
  CH01_NTC         = 21,
  CH01_PTC         = 22,
  CH01_LOAD_CELL   = 30,
} ANALOG_IN_CH01_CONF_VALUES;

typedef enum {
  CH02_DISABLED    = 0,
  CH02_CURRENT     = 1,
  CH02_VOLTAGE     = 2,
} ANALOG_IN_CH02_CONF_VALUES;

typedef enum {
  CH03_DISABLED    = 0,
  CH03_CURRENT     = 1,
  CH03_VOLTAGE     = 2,
  CH03_TCJ         = 10,
  CH03_TCK         = 11,
  CH03_TCT         = 12,
  CH03_PT1000      = 20,
  CH03_NTC         = 21,
  CH03_PTC         = 22,
  CH03_LOAD_CELL   = 30,
} ANALOG_IN_CH03_CONF_VALUES;

typedef enum {
  CH04_DISABLED    = 0,
  CH04_CURRENT     = 1,
  CH04_VOLTAGE     = 2,
} ANALOG_IN_CH04_CONF_VALUES;

typedef enum {
  CH05_DISABLED    = 0,
  CH05_CURRENT     = 1,
  CH05_VOLTAGE     = 2,
  CH05_TCJ         = 10,
  CH05_TCK         = 11,
  CH05_TCT         = 12,
  CH05_PT1000      = 20,
  CH05_NTC         = 21,
  CH05_PTC         = 22,
  CH05_LOAD_CELL   = 30,
} ANALOG_IN_CH05_CONF_VALUES;

typedef enum {
  CH06_DISABLED    = 0,
  CH06_CURRENT     = 1,
  CH06_VOLTAGE     = 2,
} ANALOG_IN_CH06_CONF_VALUES;

typedef enum {
  CH07_DISABLED    = 0,
  CH07_CURRENT     = 1,
  CH07_VOLTAGE     = 2,
  CH07_TCJ         = 10,
  CH07_TCK         = 11,
  CH07_TCT         = 12,
  CH07_PT1000      = 20,
  CH07_NTC         = 21,
  CH07_PTC         = 22,
  CH07_LOAD_CELL   = 30,
} ANALOG_IN_CH07_CONF_VALUES;

typedef enum {
  CH08_DISABLED    = 0,
  CH08_CURRENT     = 1,
  CH08_VOLTAGE     = 2,
} ANALOG_IN_CH08_CONF_VALUES;

// jumper J6 (side A) configuration request
typedef enum {
  OUT_CH01_DISABLED    = 0,
  OUT_CH01_CURRENT     = 1,
  OUT_CH01_VOLTAGE     = 2,
} ANALOG_OUT_CH01_CONF_VALUES;

/// @brief 
class namino_rosso {
public:
    namino_rosso();

    /**
     * open SPI interface.
     */
    void begin(uint32_t clock=800000U);

    /**
     * open SPI interface.
     */
    void begin(uint32_t clock=800000U, uint8_t miso=MISO, uint8_t mosi=MOSI, uint8_t sck=SCK, uint8_t ss=SS);

    /**
     * close SPI interface.
     */
    void end();

    /**
     * read namino rosso signal board, um converted
     * @param channel channel number, unit of measure as configuration
     */
    float_t readAnalogIn(int channel);

    /**
     * read namino rosso life time
     */
    uint32_t readLifeTime();

    /**
     * firmware version
     * @return signal board firmware version
     */
    uint16_t fwVersion();

    /**
     * board code identifier
     * @return identifier values 0x1338=rosso, 0x2904=arancio, ...
     */
    uint16_t boardType();

    /**
     * analog section is ready for configuration ?
     * @return true = ok, ready
     */
    bool isReady();

    /**
     * read namino rosso encoder
     */
    int32_t readEncoder();

    /**
     * read namino rosso encoder switch
     */
    bool readEncoderSwitch();

    /**
     * reset encoder counter
     */
    void resetEncoderCounter();

    /**
     * set encoder counter
     * @param value encoder counter
     */
    void setEncoderCounter(int32_t value);

    /**
     * read namino rosso digital input
     * @param channel channel number
     */
    bool readDigIn(int channel);

    /**
     * read namino rosso digital out feedback
     * @param channel channel number
     */
    bool readDigOut(int channel);

    /**
     * read namino rosso Pt1000
     * @param channel channel number
     */
    float_t readPt1000(int channel);

    /**
     * read namino rosso NTC
     * @param channel channel number
     */
    float_t readNTC(int channel);

    /**
     * read namino rosso TC J
     * @param channel channel number
     */
    float_t readTCJ(int channel);

    /**
     * read namino rosso TC K
     * @param channel channel number
     */
    float_t readTCK(int channel);

    /**
     * read namino rosso TC T
     * @param channel channel number
     */
    float_t readTCT(int channel);
    
    /**
     * read namino rosso PTC
     * @param channel channel number
     */
    float_t readPTC(int channel);

    /**
     * read namino rosso load cell
     * @param channel channel number
     */
    float_t readLoadCell(int channel);

    /**
     * read namino rosso board temperature
     * @return celsius temperature
     */
    float_t tBoard();

    /**
     * read all namino rosso signal board registers.
     */
    void readAllRegister();

    /**
     * full duplex read/write all namino rosso signal board registers.
     */
    void transferAllRegister();

    /**
     * write namino rosso analog in board configuration
     * @param channel channel number
     * @param conf conf values
     * @return true = success
     */
    bool writeAnalogInConf(int channel, uint8_t conf);


    /**
     * write namino rosso analog out value, voltage
     * @param value value 0..10 V
     */
    void writeAnalogOut(float_t value);  

    /**
     * write namino rosso analog out board configuration, jumper J6 (side A) configuration request
     * @param conf conf values
     */
    void writeAnalogOutConf(ANALOG_OUT_CH01_CONF_VALUES conf);

    /**
     * write namino rosso signal board registers.
     * @param address register number
     * @param value register value
     */
    void writeRegister(uint8_t address, uint16_t value);

    /**
     * reset write registers aread
     */
    void resetWRegister();

    /**
     * write namino rosso digital out
     * @param channel channel number
     * @param value out value
     * @return true = success
     */
    bool writeDigOut(int channel, bool value);

    /**
     * write namino rosso set rele status
     * @param value out value
     */
    void writeRele(bool value);

    /**
     * write namino rosso set PTO, pulse train output
     * @param frequency frequency of the pulse train, values ​​expressed in Hz admitted from 1 to 65535
     * @param stepsNumber number of steps that the PTO must carry out. If set to 0, the PTO does not finish counting until the stop command is received
     */
    void setupStep(uint16_t frequency, uint32_t stepsNumber);

    /**
     * write namino rosso set step number
     * @param value out value
     */
    void writeStepNumber(uint32_t value);

    /**
     * write namino rosso set step freq
     * @param value out value
     */
    void writeStepFreq(uint16_t value);

    /**
     * namino rosso start PTO
    // da note sul campo, comando:
    //     0x10 start -> esegue il numero di step indicati nel registro 36
    //     0x20 interrompe il treno in esecuzione, riarma il PTO per una nuova partenza
    // se si lancia un treno di impulsi, durante l'esecuzione il bit 8 (run) del registro 3 (naminoStatus) va a 1.
    // quando il treno è terminato, il registro 3 (naminoStatus) si modifica nel seguente modo: bit 8 (run) -> 0; bit 9 (done) -> 1
    // per riarmare il treno è necessario inviare il comando 0x20 (stop) e quindi il comando 0x10 (start)
    // Se frequenza = 0 la linea è utilizzata come uscita digitale out 
     */
    void startStepper();

    /**
     * namino rosso stop PTO
     * interrupts the running train, rearms the PTO for a new departure
     */
    void stopStepper();

    /**
     * write all namino rosso signal board registers.
     */
    void writeAllRegister();

    /**
     * register show, unformatted
     */
    void showRegister();

    /**
     * register show, formatted
     */
    void printValues();

    /*
     * reset namino microprosessor side
     */
    void resetSignalMicroprocessor();

    /**
     * save in register cache.
     * @param address register number
     * @param value register value
     */
    void saveRegister(uint8_t address, uint16_t value);

    /**
     * load from register cache.
     * @param address register number
     * @result cache register value
     */
    uint16_t loadRegister(uint8_t address);

private:
    uint8_t _miso;
    uint8_t _mosi;
    uint8_t _sck;
    uint8_t _ss;
    SPISettings spiSettings;
    SPIClass *vspi = NULL;

    uint16_t reg[WR_LAST_INDEX + 1];    // in / out register array
};

#endif  /* NAMINO_ROSSO_H */
