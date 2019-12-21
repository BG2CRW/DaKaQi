#include "driver.h"
#include <DS3231.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <IRremote.h>
#include "config.h"
SoftwareSerial mySerial(RX, TX); // RX, TX
DS3231 Clock;
IRrecv irrecv(RECV_PIN);
decode_results results;
bool Century = false, h12, PM, ADy, A12h, Apm;
byte ADay, AHour, AMinute, ASecond, ABits, year, month, date, DoW, hour, minute, second;

void ReadDS3231(unsigned char writeDate[16])
{
  writeDate[5] = Clock.getHour(h12, PM) / 10 + 48;
  writeDate[6] = Clock.getHour(h12, PM) % 10 + 48;
  writeDate[8] = Clock.getMinute() / 10 + 48;
  writeDate[9] = Clock.getMinute() % 10 + 48;
  writeDate[11] = Clock.getSecond() / 10 + 48;
  writeDate[12] = Clock.getSecond() % 10 + 48;

}

void printerSetup()
{
  mySerial.begin(9600);
}

void IRSetup()
{
  irrecv.enableIRIn(); //初始化红外遥控
}

void serialScript(unsigned char outData[48][16], unsigned char cardNum[7])
{
  long int t0, t1;
  long int h, m, s;
  // unsigned char starttime[8];
  Serial.println("dsfhiashiufhuioashuf");
  mySerial.println("BY2HIT Report Card:");
  mySerial.print("Card No.:");
  // for (int j=0;j<48;j++)
  // for(int i=0;i<16;i++)
  //  Serial.println(outData[j][i]);

  mySerial.print(cardNum[0], HEX);
  mySerial.print(cardNum[1], HEX);
  mySerial.print(cardNum[2], HEX);
  mySerial.print(cardNum[3], HEX);
  mySerial.print(cardNum[4], HEX);
  mySerial.println(" ");
  mySerial.println("Node number:    Time:");
  /*
    for (int i = 0; i < 42; i=i+4)
    {
      if (outData[i][1] == '0' && outData[i][2] == '0'&& outData[i][3] == '4')
      {
          k=i;
      }
      else if (outData[i][1] == '0' && outData[i][2] == '0'&& outData[i][3] == '5')
      {
        mySerial.println();
        mySerial.print("Start           ");
        for (int j = 5; j < 13; j++)
          mySerial.write(outData[k][j]);
        mySerial.println();
        mySerial.print("Final           ");
       // Serial.println("gdga");
        for (int j = 5; j < 13; j++)
          mySerial.write(outData[i][j]);
        for(int i=0;i<3;i++)
        mySerial.println();
        break;
      }
      else if(outData[i][1] == '1' && outData[i][2] == '2'&& outData[i][3] == '6')
      {;}
      else
      {
        for (int j = 0; j < 13; j++)
        {
          if (j == 5) mySerial.print("           ");
          mySerial.write(outData[i][j]);
        }
        mySerial.println();
      }
    }*/
}

void calibrationTime()
{
  if (irrecv.decode(&results))
  {
    if (results.value == 16753245)   //确认接收到的第一排按键1的编码，此码是预先读出来的按键编码。
    {
      Clock.setSecond(0);//Set the second
      Clock.setMinute(0);//Set the minute
      Clock.setHour(0);  //Set the hour
      Clock.setDoW(1);    //Set the day of the week
      Clock.setDate(1);  //Set the date of the month
      Clock.setMonth(1);  //Set the month of the year
      Clock.setYear(16);  //Set the year (Last two digits of the year)

      digitalWrite(9, HIGH);
      digitalWrite(6, HIGH);
      delay(300);
      digitalWrite(6, LOW);
      delay(9700);
      digitalWrite(9, LOW);
    }
    irrecv.resume(); // 接收下一个值
  }
}

char getMode()
{
  int mode = 0, bit1, bit0, bit2, bit3, bit4, bit5, bit6;
  pinMode(4, INPUT);
  pinMode(8, INPUT);
  bit6 = analogRead(A7);
  bit5 = analogRead(A3);
  bit4 = analogRead(A2);
  bit3 = analogRead(A1);
  bit2 = analogRead(A0);
  bit1 = digitalRead(8);
  bit0 = digitalRead(4);
  bit1 = !bit1;
  bit0 = !bit0;
  if (bit6 < 500)
    bit6 = 1;
  else
    bit6 = 0;

  if (bit5 < 500)
    bit5 = 1;
  else
    bit5 = 0;

  if (bit4 < 500)
    bit4 = 1;
  else
    bit4 = 0;

  if (bit3 < 500)
    bit3 = 1;
  else
    bit3 = 0;

  if (bit2 < 500)
    bit2 = 1;
  else
    bit2 = 0;

  return bit0 + bit1 * 2 + bit2 * 4 + bit3 * 8 + bit4 * 16 + bit5 * 32 + bit6 * 64;
}
