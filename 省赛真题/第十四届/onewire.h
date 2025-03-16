#ifndef _ONEWIRE_H
#define _ONEWIRE_H
#include <intrins.h>
#include <STC15F2K60S2.H>
void Delay_OneWire(unsigned int t);
void Write_DS18B20(unsigned char dat);
unsigned char Read_DS18B20(void);
bit init_ds18b20(void);

unsigned char read_Temp();
#endif