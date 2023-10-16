/*
 * namino bianco pin io test
 * partition scheme: minimal 1.3MB / 700 kb SPIFF
 * PSRAM: QSPI PSRAM / Quad SPI PSRAM
 * board profile: namino rosso, when available set to: namino bianco board
 */
#include <Arduino.h>
#include "./namino_bianco_pins.h"

#define GPIO_CONF_OUTPUT(x)     { pinMode(x, OUTPUT); digitalWrite(x, LOW);}
#define CHECK_BIT(var,pos)      ((var) & (1<<(pos)))

u_int8_t pins_io[] = {GPIO35, GPIO36, GPIO37, GPIO38, GPIO39, GPIO40, GPIO41, 
                      GPIO42, GPIO21, GPIO16, GPIO14, GPIO47, GPIO17, GPIO18
};

uint32_t count = 0;

void setup(){
    Serial.begin(115200);
    delay(5000);
    Serial.println();
    Serial.println("=====================================");
    Serial.println("## PIN IO D/AN NAMINO BIANCO TEST  ##");
    Serial.println("=====================================");
    Serial.println();
    delay(2000);

    Serial.println("conf i/o out pins: ");
    for (u_int8_t i : pins_io) {
        Serial.print(i);
        Serial.print(", ");
        GPIO_CONF_OUTPUT(i);
    }
    Serial.println();

    Serial.println("test i/o out pins: "); 
    for (u_int8_t i : pins_io) {
        Serial.println(i);
        digitalWrite(i, LOW);
        delay(200);
        digitalWrite(i, HIGH);
        delay(200);
        digitalWrite(i, LOW);
        delay(200);
        digitalWrite(i, HIGH);
        delay(300);
        digitalWrite(i, LOW);
        delay(300);
    }
}

void loop() {
    // digital I/O
    Serial.println(count);
    for (u_int8_t i = 0; i < (sizeof(pins_io)/sizeof(*pins_io)); i++) {
        if (CHECK_BIT(count, i)) {
            digitalWrite(pins_io[i], HIGH);
        } else {
            digitalWrite(pins_io[i], LOW);
        }
    }
    count++;

    // analog Inputs
    Serial.print("A0 ");
    Serial.print(analogRead(A0) * (3.3 / 4096.0));
    Serial.print(" A1 ");
    Serial.print(analogRead(A1) * (3.3 / 4096.0));
    Serial.print(" A2 ");
    Serial.print(analogRead(A2) * (3.3 / 4096.0));
    Serial.print(" A3 ");
    Serial.print(analogRead(A3) * (3.3 / 4096.0));
    Serial.print(" A4 ");
    Serial.print(analogRead(A4) * (3.3 / 4096.0));
    Serial.print(" A5 ");
    Serial.print(analogRead(A5) * (3.3 / 4096.0));
    Serial.println();

    delay(50);
}
