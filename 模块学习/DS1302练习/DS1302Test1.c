#include <STC15F2K60S2.H>
#include "ds1302.h"

unsigned char Write_ds1302_adr[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
unsigned char Read_ds1302_adr[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
//20年4月18日，周六，23：59：24
unsigned char Timer[7]={0x24,0x59,0x23,0x18,0x04,0x06,0x20};

unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};

void SelectHC573(unsigned char channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)| 0x80;
			break;
		case 5:
			P2=(P2&0x1f)| 0xa0;
			break;
		case 6:
			P2=(P2&0x1f)| 0xc0;
			break;
		case 7:
			P2=(P2&0x1f)| 0xe0;
			break;
	}
}

void display_SMG(unsigned char pos,unsigned char value)
{
	SelectHC573(7);
	P0=0xff;//消影
	
	SelectHC573(6);
	P0=(0x01<<pos);
	
	SelectHC573(7);
	P0=value;
}
void ds1302_Config()
{
	char i;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<7;i++)
	{
		Write_Ds1302_Byte(Write_ds1302_adr[i],Timer[i]);
	}
	Write_Ds1302_Byte(0x8e,0x80);
}
void read_ds1302_time()
{
	char i;
	for(i=0;i<7;i++)
	{
		Timer[i]=Read_Ds1302_Byte(Read_ds1302_adr[i]);
	}
}
void smg_all()
{
	SelectHC573(7);
	P0=0xff;//消影;
	SelectHC573(6);
	P0=0xff;//消影
}
void delay_SMG(unsigned int t)
{
	while(t--);
}
void show_time()
{
	display_SMG(0,SMG_number[Timer[2]/16]);
	delay_SMG(100);
	display_SMG(1,SMG_number[Timer[2]%16]);
	delay_SMG(100);
	
	display_SMG(2,SMG_number[16]);
	delay_SMG(100);
	
	display_SMG(3,SMG_number[Timer[1]/16]);
	delay_SMG(100);
	display_SMG(4,SMG_number[Timer[1]%16]);
	delay_SMG(100);
	
	display_SMG(5,SMG_number[16]);
	delay_SMG(100);
	
	display_SMG(6,SMG_number[Timer[0]/16]);
	delay_SMG(100);
	display_SMG(7,SMG_number[Timer[0]%16]);
	delay_SMG(100);
	
	smg_all();
}




void main()
{
	ds1302_Config();
	while(1)
	{
		read_ds1302_time();
		show_time();
	}
}