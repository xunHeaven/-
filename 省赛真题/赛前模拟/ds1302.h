#ifndef _DS1302_H
#define _DS1302_H
#include <intrins.h>
#include <STC15F2K60S2.H>
extern unsigned char Timer[7];


void Write_Ds1302(unsigned  char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte ( unsigned char address );

void write_date();
void read_date();
#endif