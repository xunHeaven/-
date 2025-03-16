#ifndef _IIC_H
#define _IIC_H
#include <intrins.h>
#include <STC15F2K60S2.H>

static void I2C_Delay(unsigned char n);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);
void write_PCF(unsigned char dat);
unsigned char read_PCF(unsigned char ctrl_byte);
#endif