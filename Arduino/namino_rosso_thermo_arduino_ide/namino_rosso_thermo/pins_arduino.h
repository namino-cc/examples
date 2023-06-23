//
// Copyright (c) 2023 Namino Team, version: 1.0.15 @ 2023-06-16
//
//
#include <stdint.h>

#define NAMINO_ROSSO_BOARD


/* Begin Pins on ESP32-S3-WROOM-1U-N4R8 */
#define GPIO4       4
#define GPIO5       5
#define GPIO6       6
#define GPIO7       7
#define GPIO15      15
#define GPIO16      16
#define GPIO17      17
#define GPIO18      18
#define GPIO8       8
#define GPIO19      19
#define GPIO20      20
#define GPIO3       3
#define GPIO46      46
#define GPIO9       9
#define GPIO10      10
#define GPIO11      11
#define GPIO12      12
#define GPIO13      13
#define GPIO14      14
#define GPIO21      21
#define GPIO47      47
#define GPIO48      48
#define GPIO45      45
#define GPIO0       0
#define GPIO35      35
#define GPIO36      36
#define GPIO37      37
#define GPIO38      38
#define GPIO39      39
#define GPIO40      40
#define GPIO41      41
#define GPIO42      42
#define GPIO44      44
#define GPIO43      43
#define GPIO2       2
#define GPIO1       1

#define RESET_ADD_ON    GPIO46
#define SS              GPIO10
#define MOSI            GPIO11
#define MISO            GPIO13
#define SCK             GPIO12
// SPI SD CARD
#define CS_SDCARD   GPIO2
// prog pins
#define BOOT_MODE   GPIO47
#define ISP_TX      GPIO17
#define ISP_RX      GPIO18
#define NM_RESET    GPIO48
/* End Pins on ESP32-S3-WROOM-1U-N4R8 */

/* Begin Analog Pins on ESP32-S3-WROOM-1U-N4R8 */
#define ADC1_CH3    GPIO4
#define ADC1_CH4    GPIO5
#define ADC1_CH5    GPIO6
#define ADC1_CH6    GPIO7
#define ADC2_CH4    GPIO15
#define ADC2_CH5    GPIO16
#define ADC2_CH6    GPIO17
#define ADC2_CH7    GPIO18
#define ADC1_CH7    GPIO8
#define ADC2_CH8    GPIO19
#define ADC2_CH9    GPIO20
#define ADC1_CH2    GPIO3
#define ADC1_CH8    GPIO9
#define ADC1_CH9    GPIO10
#define ADC2_CH0    GPIO11
#define ADC2_CH1    GPIO12
#define ADC2_CH2    GPIO13
#define ADC2_CH3    GPIO14
#define ADC1_CH1    GPIO2
#define ADC1_CH0    GPIO1
/* End Analog Pins on ESP32-S3-WROOM-1U-N4R8 */

/* Begin Touch Pins on ESP32-S3-WROOM-1U-N4R8 */
#define TOUCH4      GPIO4
#define TOUCH5      GPIO5
#define TOUCH6      GPIO6
#define TOUCH7      GPIO7
#define TOUCH8      GPIO8
#define TOUCH3      GPIO3
#define TOUCH9      GPIO9
#define TOUCH10     GPIO10
#define TOUCH11     GPIO11
#define TOUCH12     GPIO12
#define TOUCH13     GPIO13
#define TOUCH14     GPIO14
#define TOUCH2      GPIO2
#define TOUCH1      GPIO1
/* End Touch Pins on ESP32-S3-WROOM-1U-N4R8 */

#define TX              0
#define RX              0

#define SDA                     GPIO1
#define SCL                     GPIO0
#define NAMINO_ROSSO_I2C_SDA    SDA
#define NAMINO_ROSSO_I2C_SCL    SCL
#define NM_I2C_SDA              SDA
#define NM_I2C_SCL              SCL

#define A0              ADC1_CH0
#define A1              ADC1_CH1
#define A2              ADC1_CH2
#define A3              ADC1_CH3
#define A4              ADC1_CH4
#define A5              ADC1_CH5
#define A6              ADC1_CH6
#define A7              ADC1_CH7
#define A8              ADC2_CH0
#define A9              ADC2_CH1
#define A10             ADC2_CH2
#define A11             ADC2_CH3
#define A12             ADC2_CH4
#define A13             ADC2_CH5
#define A14             ADC2_CH6
#define A15             ADC2_CH7

#define DAC1            0
#define DAC2            0

/* Begin Arduino naming */
#define RESET_ARDUINO   GPIO46
#define PC0             GPIO3
#define PC1             GPIO4
#define PC2             GPIO5
#define PC3             GPIO6
#define PC4             GPIO7
#define PC5             GPIO8
#define PB5             GPIO35
#define PB4             GPIO36
#define PB3             GPIO37
#define PB2             GPIO38
#define PB1             GPIO39
#define PB0             GPIO40
#define PD7             GPIO41
#define PD6             GPIO42
#define PD5             GPIO21
#define PD4             GPIO16
#define PD3             GPIO14
#define PD2             GPIO9
#define PD1             GPIO17
#define PD0             GPIO18
/* End Arduino naming */

/* Begin alternate naming
#define J1_io0      I2C_SCL

#define J2_35       PB5_IO35
#define J2_36       PB4_IO36
#define J2_37       PB3_IO37
#define J2_38       PB2_IO38
#define J2_39       PB1_IO39
#define J2_40       PB0_IO40

#define J3_io8      PD7_IO8
#define J3_7        PD6_IO7
#define J3_21       PD5_IO21
#define J3_16       PD4_IO16
#define J3_14       PD3_IO14
#define J3_9        PD2_IO9    
#define J3_17       TX_IO17
#define J3_18       RX_IO18

#define J4_cs_io2   CS_SDCARD_IO2
#define J4_sclk     uCSPI_CLK
#define J4_mosi     ucSPI_MOSI
#define J4_miso     uCSPI_MISO

#define J9_io3      PC0_IO3
#define J9_4        PC1_IO4
#define J9_5        PC2_IO5
#define J9_6        PC3_IO6
#define J9_7        PC4_IO7
#define J9_8        PC5_IO8

#define J10_enc_A   PLU_INPUT2
#define J10_enc_B   PLU_INPUT3
#define J10_sw      PIO0_28
End alternate naming */
