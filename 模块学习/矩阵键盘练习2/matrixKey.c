#include <STC15F2K60S2.H>
#include <intrins.h>
sbit R1=P3^3;
sbit R2=P3^2;
sbit R3=P3^1;
sbit R4=P3^0;
sbit C1=P4^4;
sbit C2=P4^2;
sbit C3=P3^5;
sbit C4=P3^4;
void HCselect(unsigned char channel);
void scanKey();
void InitSys();
void offSMG();
void displaySMG(unsigned char pos,unsigned char value);
void delay(unsigned int t);

void main()
{
	while(1)
	{
	
	}
}

void HCselect(unsigned char channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)|0x80;
			break;
		case 5:
			P2=(P2&0x1f)|0xa0;
			break;
		case 6:
			P2=(P2&0x1f)|0xc0;
			break;
		case 7:
			P2=(P2&0x1f)|0xe0;
			break;
	}
}
void InitSys()
{
	HCselect(5);
	P0=P0&0xaf;
	HCselect(4);
	P0=0xff;
}

void displaySMG(unsigned char pos,unsigned char value)
{
	HCselect(6);
	P0=(0x01<<pos);
	HCselect(7);
	P0=value;
}
void offSMG()
{
	HCselect(6);
	P0=0x00;
	HCselect(7);
	P0=0xff;
}
void delay(unsigned int t)
{
	while(t--);
}
void showSMG()
{
	displaySMG(5,SMG_number[distance/100]);
	delay(500);
	offSMG();
	displaySMG(6,SMG_number[(distance/10)%10]);
	delay(500);
	offSMG();
	displaySMG(7,SMG_number[distance%10]);
	delay(500);
	offSMG();
}
void scanKey()
{
	R1=R2=0;
	C1=C2=1;
	
}