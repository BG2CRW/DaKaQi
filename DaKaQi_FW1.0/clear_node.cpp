#include "clear_node.h"
#include "config.h"
#include "driver.h"

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
extern Adafruit_PN532 nfc;
#endif

extern unsigned int siteNum;

void clear_node()
{

  unsigned char i, tmp;
  unsigned char status;
  unsigned char state = 1;
#if USING_RFID
  unsigned char str[MAX_LEN];
#endif
  unsigned char RC_size;
  unsigned char blockAddr;        //选择操作的块地址0～63//写卡数据

  unsigned char writeDate[16] = {'<', '0', '0', '0', '>', 0, 0, ':', 0, 0, ':', 0, 0};
  //编号
  writeDate[1] = siteNum / 100 + 48;
  writeDate[2] = siteNum / 10 % 10 + 48;
  writeDate[3] = siteNum % 10 + 48;
#if USING_RFID
  //找卡
  card_ok = rfid.isCard();

  //读取卡序列号
  if (rfid.readCardSerial())
  {
    digitalWrite(9, HIGH);
    //读取序列号显示
    Serial.print("The card's number is  : ");
    Serial.print(rfid.serNum[0], HEX);
    Serial.print(rfid.serNum[1], HEX);
    Serial.print(rfid.serNum[2], HEX);
    Serial.print(rfid.serNum[3], HEX);
    Serial.print(rfid.serNum[4], HEX);
    Serial.println(" ");
  }
  //选卡，返回卡容量（锁定卡片，防止多次读写）
  rfid.selectTag(rfid.serNum);
  if (rfid.serNum[0] == 84 && rfid.serNum[1] == 61 && rfid.serNum[2] == 144 && rfid.serNum[3] == 133 && rfid.serNum[4] == 124)
  {
    digitalWrite(9, HIGH);
    digitalWrite(6, HIGH);
    delay(300);
    digitalWrite(6, LOW);
    delay(1000);
    digitalWrite(9, LOW);
    digitalWrite(2, LOW); //Serial.println("helloworld");
  }
  //写数据卡
  blockAddr = 3;
  for (int j = 0; j < 14; j++)
  {
    blockAddr = blockAddr + 4;
    if (rfid.auth(PICC_AUTHENT1A, blockAddr, sectorKeyA[0], rfid.serNum) == MI_OK)//认证
    {
      status = rfid.write(blockAddr, sectorNewKeyA[0]);
      for (int i = 3; i > 0; i--)
      {
        ReadDS3231(writeDate);
        // writeDate[14] = ((blockAddr - i - 3) - (blockAddr - i - 3) / 4) / 10 + 48;
        //writeDate[15] = ((blockAddr - i - 3) - (blockAddr - i - 3) / 4) % 10 + 48;
        // writeDate[14] = test/10;
        // writeDate[16] = test%10;
        status = rfid.write(blockAddr - i, writeDate);
      }
      flag = 1;
    }

  }
  if (flag)
  {
    //Serial.println("gggg");
    digitalWrite(9, HIGH);
    digitalWrite(6, HIGH);
    delay(300);
    digitalWrite(6, LOW);
    delay(1000);
    digitalWrite(9, LOW);
    flag = 0;
  }
  //读卡
  blockAddr = 3;
  for (int j = 0; j < 14; j++)
  {
    if (state == 0) break;
    blockAddr = blockAddr + 4;

    if (rfid.auth(PICC_AUTHENT1A, blockAddr, sectorKeyA[0], rfid.serNum) == MI_OK)
    {
      status = rfid.write(blockAddr, sectorNewKeyA[0]);

      for (int i = 3; i > 0; i--)
      {

        status = rfid.auth(PICC_AUTHENT1A, blockAddr, sectorNewKeyA[0], rfid.serNum);
        if (status == MI_OK)  //认证
        {
          //读数据
          if ( rfid.read(blockAddr - i, str) == MI_OK)
          {
            Serial.print("Read from the card ,the data is : ");
            Serial.println((char *)str);
          }
        }
      }
    }
  }
  digitalWrite(9, LOW);
  if (card_ok)
  {
    status = 1;
    card_ok = 0;
  }
  rfid.halt();
#endif
#if USING_NFC
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);
  if (success) {
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);

    if (uid[0] == 84 && uid[1] == 61 && uid[2] == 144 && uid[3] == 133 && uid[4] == 124)
    {
      digitalWrite(9, HIGH);
      digitalWrite(6, HIGH);
      delay(300);
      digitalWrite(6, LOW);
      delay(1000);
      digitalWrite(9, LOW);
      digitalWrite(2, LOW); //Serial.println("helloworld");
    }

    if (uidLength == 4)
    {
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      blockAddr = 4;
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockAddr, 0, keya);
      Serial.println(success);
      // Serial.println("ashjkf");
      if (success)
      {

        //Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];

        ReadDS3231(writeDate);
        // If you want to write something to block 4 to test with, uncomment
        // the following line and this text should be read back in a minute
        memcpy(data, writeDate, sizeof data);
        success = 0;
        success = nfc.mifareclassic_WriteDataBlock (blockAddr, data);
        if (success)
        {
          digitalWrite(9, HIGH);
          digitalWrite(6, HIGH);
          delay(300);
          digitalWrite(6, LOW);
          delay(1000);
          digitalWrite(9, LOW);
        }
        // Serial.println(success);
        // Serial.println("hhhhh");
        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(blockAddr, data);
        Serial.println(success);
        // Serial.println("nanni");

        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");

          // Wait a bit before reading the card again
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
          //Serial.println("helloworld");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }

    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");

      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");

        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }
#endif
}
