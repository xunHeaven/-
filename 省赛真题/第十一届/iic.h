#ifndef _IIC_H
#define _IIC_H

#include <intrins.h>
#include <STC15F2K60S2.H>

sbit scl=P2^0;
sbit sda=P2^1;
static void I2C_Delay(unsigned char n);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);

void AT24_Write(unsigned addr,unsigned char dat);
unsigned char AT24_Read(unsigned char addr);

void PCF_Write(unsigned char dat);
unsigned char PCF_Read(unsigned char ctrl_byte);

#endif