/*
   config.h
*/

#ifndef DRIVER_H_
#define DRIVER_H_

void ReadDS3231(unsigned char writeDate[16]);

void serialScript(unsigned char outData[48][16], unsigned char cardNum[7]);

void printerSetup();

void calibrationTime();

void IRSetup();

char getMode();

#endif /* DRIVER_H_ */
