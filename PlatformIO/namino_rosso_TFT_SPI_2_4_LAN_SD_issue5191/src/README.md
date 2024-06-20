refers to a test board that has an SPI bus with multiple devices, ESP32-S3 - TFT - LAN - SD card. 
Attached is the library to consolidate with MELD, modified in the code

file Ethernet2.h:

  EthernetClass() { _dhcp = NULL; w5500_cspin = 9; }
  void init(uint8_t _cspin = 9) { w5500_cspin = _cspin; }

file EthernetServer.h:

class EthernetServer : 
public Print {
private:

