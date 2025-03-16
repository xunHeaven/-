#ifndef _IIC_H
#define _IIC_H
#include <STC15F2K60S2.H>
#include <intrins.h>
static void I2C_Delay(unsigned char n);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);

void write_AT24(unsigned char addr,unsigned char dat);//�����д��EEPROM 
unsigned char read_AT24(unsigned char addr);//��eeprom�ж��� 

void write_PCF(unsigned char dat);//���ģ���ź� 0-5v 
unsigned char read_PCF(unsigned char ctrl_byte);//��������������������ĵ�ѹ�� 
#endif
