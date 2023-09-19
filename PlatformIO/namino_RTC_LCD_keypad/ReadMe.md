# Project **namino_RTC_LCD_Keypad_Shield**
***
## Purpose of this Project: Show how to use **Robot D1 LCD Keypad Shield** and internal **DS1307 RTC** via I2C with *Namino Boards*.  
***

The ***LCD Keypad Shield*** board provides a 2 row x 16 column **HD44780** lcd display and 5 navigation keys.  

The display uses **6 digital pins** as output of the shield (from **D4 to D9** on Arduino) and 1 analog pin (A0 on Arduino) as input for reading the buttons.  
On some boards, an additional control pin (**D10** on Arduino) is also defined to turn the display backlight on and off.

The screen is managed with the ***Arduino LiquidCrystal*** library.  

This example sketch **does not use the namino library to work, and can be used as-is on both rosso and arancio naminos**.  

The definition of the Arduino shield pins are remapped in the code to the corresponding esp32 pins on side A of the namino board (in both the rosso and arancio versions) with the declaration of the int constants **RS, Enable, D4, D5, D6, D7,** passed to the constructor of the **lcd** object of type **LiquidCrystal**.  
The **keyPin** constant remaps analog input A0 to esp32 channel 3.  
The **BL** constant remaps digital output D10 to esp32 channel 38.  
The **NAMINO_I2C_SCL** and **NAMINO_I2C_SDA** remap *I2C* SCL and SDA pins.

In the ***setup()*** function you configure the monitor serial interface, the I/O pins, RTC and the lcd display.  

In the ***loop()*** function, it is checked that at least **LOOP_PERIOD** ms have passed since the previous loop, otherwise it is exited.  
If at least *TIME_PERIOD* ms have passed since the last reading of the RTC Clock, a new reading is taken and the information on the display is updated.

The **readKeyboard()** function is called for reading the analog input, which returns the currently pressed key based on the read value, which is written on the display at position 9, 1.

The ***readKeyboard()*** function, in addition to analyzing and printing the analog value read from the pin in position 1.0, takes into account the time elapsed since the previous key was pressed, and keeps a flashing cursor active for BLINK_ON seconds.  

The ***setBackLight()*** function turns On and Off display backlight. 
