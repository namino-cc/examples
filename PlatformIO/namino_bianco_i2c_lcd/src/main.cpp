#include "main.h"

// LCD 20x4
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address, for a 20 chars and 4 line display

uint32_t    lastLoop = millis();
uint16_t    counter = 0;

void i2cScanner() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
         Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      nDevices++;
     }
     else if (error==4)
     {
      Serial.print("Unknown error at address 0x");
      if (address<16)
         Serial.print("0");
      Serial.println(address,HEX);
     }    
    }
    if (nDevices == 0)
       Serial.println("No I2C devices found\n");
    else
       Serial.println("done\n");
}

void setup() {
  // Serial Setup
  Serial.begin(115200);
  delay(2000);  // mandatory delay, recomended delay on bianco board
  
  Serial.println("");
  Serial.println("=====================================");
  Serial.println("##         I2C LCD EXAMPLE        ##");
  Serial.println("=====================================");
  Serial.println();
  Serial.print("SCL: ");
  Serial.println(SCL);
  Serial.print("SDA: ");
  Serial.println(SDA);
  Serial.println("-------------------");
  Serial.flush();
  delay(2000);

  Wire.begin(SDA, SCL);
  // i2c bus scan for find devices
  i2cScanner();

  // LCD on i2c Setup
  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("  I2C  LCD  EXAMPLE ");
  lcd.setCursor(0,1);
  lcd.print("    NAMINO BIANCO   ");

  Serial.println("Display Init Done");
}

void loop() {
  uint32_t theTime = millis();

  // limit loop period
  if ((theTime - lastLoop) < LOOP_PERIOD)  {
    return;
  }
  lastLoop = theTime;

  lcd.setCursor(0,3);
  lcd.printf("   %8d", counter++);
}


