# Project **namino_rosso_blinkLed**
***
## Purpose of this Project: Turn ON and OFF repeatedly on board TEST LED, connected in parallel to DIG OUT1 (digital output 1) - Side B – J15
***

To use the DIG OUT1 (digital output 1) output channel present on the industrial side of the "namino rosso" board it is necessary to use the "*namino_rosso*" library.

Therefore, an "*nr*" object of type "*namino_rosso*" is created and used to configure the industrial side and to control the output channels.

During the loop the state of the LED is read and the state reversed.

Two PT1000 probes are also connected to the analog inputs, and the temperature is read by the probes by calling the readPt1000() function every 4 seconds.

The example also shows how to use the RTC Chip to manage the system date and time.

NB: Firmware must be version 1.007 and above (nr.fwVersion() must return 0x1007 and above)
