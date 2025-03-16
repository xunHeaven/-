#ifndef _DS1302_H
#define _DS1302_h
#include <STC15F2K60S2.H>
#include <intrins.h>
sbit SDA=P2^3;
sbit SCK=P1^7;
sbit RST=P1^3;
extern unsigned char TIMER[7];
void Write_Ds1302(unsigned  char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte ( unsigned char address );

void write_time();
void read_time();

#endif