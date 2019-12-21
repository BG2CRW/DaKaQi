//PINOUTS
//Jump:BIT6  BIT5  BIT4  BIT3  BIT2  BIT1  BIT0
//      A7    A3    A2    A1    A0    D8    D4
//Light:D9
//Sound:D6
//Battery_test:A6
//DS3231:A4,A5
//Printer:D2(TX),D3(RX)
//RFID:D5,D10,D11,D12,D13
//Infrared:D7


//mode define:
//0-123:common node:0->start node,125->end node
//127:main station
//126:clear node
#include "config.h"
#include "driver.h"
#include <SPI.h>
#include "main_station.h"
#include "clear_node.h"
#include "common_node.h"

#if USING_RFID
#include <RFID.h>
RFID rfid(RFID_SS, RST);   //D10--读卡器MOSI引脚、D5--读卡器RST引脚
#endif
#if USING_NFC
#include <Adafruit_PN532.h>
#define PN532_SCK  SPI_SCK
#define PN532_MOSI SPI_MOSI
#define PN532_SS   RFID_SS
#define PN532_MISO SPI_MISO
// #define PN532_IRQ   IRQ
#define PN532_RESET RST
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
#endif
int flag = 0;


int k, mode;
//原扇区A密码，16个扇区，每个扇区密码6Byte
const unsigned char sectorKeyA[1][6] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};
//新扇区A密码，16个扇区，每个扇区密码6Byte
const unsigned char sectorNewKeyA[1][16] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff, 0x07, 0x80, 0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};

void setup()
{
  mode = getMode();
  Serial.begin(9600);
  pinMode(9, OUTPUT); //LIGHT
  pinMode(6, OUTPUT); //SOUND
  digitalWrite(6, HIGH);
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(6, LOW);
  digitalWrite(9, LOW);//开机检测
#if USING_RFID
  SPI.begin();
  rfid.init();
#endif
#if USING_NFC
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  //Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  // Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  //Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");
#endif
  if (mode == 127) //main station node
  {
    printerSetup();
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);
  }
  else if (mode == 126) //clear node
  {
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    //only need rfid
    IRSetup();
  }
  else//common node:0->start node,125->end node
  {
    pinMode(2, HIGH);
    digitalWrite(2, HIGH);
    //need infrared,DS3231,RFID
    IRSetup();
    //  Wire.begin();
  }
}



void loop()
{
  int dat = analogRead(BAT_TEST);
  float test = (dat * 3.3 * 2) / 1023;
  Serial.println(test);//battery test
  Serial.println(mode);
  if (mode == 127)//main station
  {
    main_station();
  }
  else if (mode == 126)//clear node
  {
    clear_node();
    calibrationTime();
  }
  else//common node
  {
    common_node();
    calibrationTime();
  }
}



