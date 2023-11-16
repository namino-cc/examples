# Project **namino_arancio_blinkLed**
***
## Purpose of this Project: Turn ON and OFF repeatedly on board TEST LED, connected in parallel to DIG OUT1 (digital output 1) - Side B – J15
***

To use the DIG OUT1 (digital output 1) output channel present on the industrial side of the "namino arancio" board it is necessary to use the "*namino_arancio*" library.

Therefore, an "*na*" object of type "*namino_arancio*" is created and used to configure the industrial side and to control the output channels.

During the loop the state of the LED is read and the state reversed.

The example also shows how to use the RTC Chip to manage the system date and time.

NB: Firmware must be version **1.007** and above (na.fwVersion() must return 0x1007 and above)
