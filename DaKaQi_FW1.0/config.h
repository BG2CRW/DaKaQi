/*
   config.h
*/

#ifndef CONFIG_H_
#define CONFIG_H_

//choose module
#define USING_RFID 0
#define USING_NFC 1
/*
  #define RX 3
  #define TX 8

  #define RFID_SS 10
  #define RST 5
  #define FLASH_SS A0

  #define SCK 13
  #define MOSI 11
  #define MISO 12

  #define IRQ 2

  #define  RECV_PIN 7

  #define LED 9
  #define SPEAKER 6
  #define REED_SWITCH 4
  #define BAT_TEST A6

  #define JUMP0 A1
  #define JUMP1 A2
  #define JUMP2 A3
  #define JUMP3 A7
*/

#define RX 3
#define TX 2

#define RFID_SS 10
#define RST 5
#define FLASH_SS A0

#define SPI_SCK 13
#define SPI_MOSI 11
#define SPI_MISO 12

#define  RECV_PIN 7

#define LED 9
#define SPEAKER 6
#define REED_SWITCH 4
#define BAT_TEST A6

#define JUMP0 A1
#define JUMP1 A2
#define JUMP2 A3
#define JUMP3 A7

#endif /* CONFIG_H_ */
