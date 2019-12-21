#include "main_station.h"
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

void main_station()
{

  unsigned char i, tmp;
  unsigned char status;
  unsigned char state = 1;
  unsigned char card_ok = 0;
#if USING_RFID
  unsigned char str[MAX_LEN];
#endif
  unsigned char RC_size;
  unsigned char blockAddr;        //选择操作的块地址0～63//写卡数据
#if USING_RFID
  unsigned char cardNum[5];

  unsigned char outData[48][16] ;

  //找卡
  card_ok = rfid.isCard();
  //读取卡序列号
  if (rfid.readCardSerial())
  {
    digitalWrite(9, HIGH);
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
    digitalWrite(7, LOW);
    //Serial.println("helloworld");
  }

  //读卡
  blockAddr = 3;
  for (int j = 0; j < 14; j++)
  {
    if (state == 0) break;
    blockAddr = blockAddr + 4;

    if (rfid.auth(PICC_AUTHENT1A, blockAddr, sectorNewKeyA[0], rfid.serNum) == MI_OK)
    {
      for (int i = 3; i > 0; i--)
      {
        status = rfid.auth(PICC_AUTHENT1A, blockAddr, sectorNewKeyA[0], rfid.serNum);
        if (status == MI_OK)  //认证
        {
          //读数据
          if ( rfid.read(blockAddr - i, outData[3 * j + 3 - i]) == MI_OK)
          {

            if ( outData[3 * j + 3 - i][4] == 0 && state)
            {
              state = 0;
            }
          }
        }
      }
    }
  }
  digitalWrite(9, LOW);
  if (card_ok)
  {
    digitalWrite(9, HIGH);
    digitalWrite(6, HIGH);
    delay(300);
    digitalWrite(6, LOW);
    delay(1000);
    digitalWrite(9, LOW);
    //Serial.println("nani");
    //打印机打印出结算后的成绩清单
    serialScript(outData, rfid.serNum);
    status = 1;
    card_ok = 0;
  }
  rfid.halt();
#endif
#if USING_NFC
  unsigned char outData[48][16] = {"/0"};
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);
  if (success) {
    // Display some basic information about the card
    // Serial.println("Found an ISO14443A card");
    //Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
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
      //Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      // Start with block 4 (the first block of sector 1) since sector 0
      // contains the manufacturer data and it's probably better just
      // to leave it alone unless you know what you're doing
      blockAddr = 4;
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockAddr, 0, keya);
      Serial.println(success);
      // Serial.println("ashjkf");
      if (success)
      {

        //Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];

        //              ReadDS3231(writeDate);
        // If you want to write something to block 4 to test with, uncomment
        // the following line and this text should be read back in a minute
        //memcpy(data,writeDate, sizeof data);
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
        blockAddr = 0;
        state = 1;
        char temp = '0';
        for (int j = 0; j < 11; j++)
        {
          if (state == 0) break;
          blockAddr = blockAddr + 4;
          success = 0;
          while (!success)
          {
            success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockAddr, 0, keya);
            success = nfc.mifareclassic_ReadDataBlock(blockAddr, data);
            //nfc.PrintHexChar(data, 16);

            Serial.println("");/*
                  for(int jk=0;jk<16;jk++)
                  {
                    temp = data[jk] - 48 + '0';
                    Serial.println(temp);
                  }*/
            Serial.println(blockAddr);

          }


          for (int ii = 0; ii < 16; ii++)
          {
            temp = data[ii] - 48 + '0';
            outData[blockAddr][ii] = temp;


          }


        }
        //for(int j=0;j<48;j++)
        //for(int i=0;i<16;i++)Serial.print(outData[j][i]);
        Serial.print(outData[3][3]);


        Serial.println(success);

        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          //nfc.PrintHexChar(data, 16);
          Serial.println("");
          // serialScript(&outData[0],uid);
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          //加上下面这句话就死机不明原因，前面那句 Serial.println("Reading Block 4:");打印没执行，表明没进函数就死机，玄学
          serialScript(outData, uid);
          //注释掉以后就不死机，玄学
          //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          delay(100);
          digitalWrite(9, HIGH);
          digitalWrite(6, HIGH);
          delay(300);
          digitalWrite(6, LOW);
          delay(1000);
          digitalWrite(9, LOW);
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
      //Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");

      // Try to read the first general-purpose user page (#4)
      //Serial.println("Reading page 4");
      uint8_t data[32];
      //success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        //nfc.PrintHexChar(data, 4);
        //Serial.println("");

        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        //Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }
  //  for (int k=0;k<7;k++)  Serial.println(uid[k]);//Serial.println(uid[1]);Serial.println(uid[2]);Serial.println(uid[3]);Serial.println(uid[4]);
  /* for (int j=0;j<48;j++)
    for(int i=0;i<16;i++)*/

#endif
}
