#include <STC15F2K60S2.H>
#include <ds1302.h>
#include <iic.h>
#include <onewire.h>
#include <intrins.h>

sbit R1=P3^0;
sbit C2=P4^2;
unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	
unsigned char SMG_mode=1;
unsigned int temp=0;
unsigned char v=0;


void SelectHC(unsigned char channel);
void Initsys();
void displaySMG(unsigned char pos,unsigned char value);
void showSMG();
void offSMG();
void scanKey();
void delaySMG(unsigned int t);
void main()
{
	Initsys();
	temp=read_temp();
	write_date();
	while(1)
	{
		v=read_PCF(0X03);
		temp=read_temp();
		read_date();
		scanKey();
		showSMG();
		
	}
}
void SelectHC(unsigned char channel)
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
void Initsys()
{
	SelectHC(5);
	P0=P0&0xaf;
	SelectHC(4);
	P0=0xff;
}
void displaySMG(unsigned char pos,unsigned char value)
{
	SelectHC(6);
	P0=0x01<<pos;
	SelectHC(7);
	P0=value;
}
void offSMG()
{
	SelectHC(6);
	P0=0x00;
	SelectHC(7);
	P0=0xff;
}
void delaySMG(unsigned int t)
{
	while(t--);
}
void showSMG()
{
	if(SMG_mode==1)//时间
	{
		displaySMG(0,SMG_number[Timer[2]/16]);
		delaySMG(100);
		displaySMG(1,SMG_number[Timer[2]%16]);
		delaySMG(100);
		
		displaySMG(2,SMG_number[16]);
		delaySMG(100);
		
		displaySMG(3,SMG_number[Timer[1]/16]);
		delaySMG(100);
		displaySMG(4,SMG_number[Timer[1]%16]);
		delaySMG(100);
		
		displaySMG(5,SMG_number[16]);
		delaySMG(100);
		
		displaySMG(6,SMG_number[Timer[0]/16]);
		delaySMG(100);
		displaySMG(7,SMG_number[Timer[0]%16]);
		delaySMG(100);
		offSMG();
	}
	else if(SMG_mode==2)//温度
	{
		displaySMG(0,SMG_number[12]);
		delaySMG(100);
		displaySMG(5,SMG_number[temp/100]);
		delaySMG(100);
		displaySMG(6,SMG_number[(temp%100)/10]);
		delaySMG(100);
		displaySMG(7,SMG_number[temp%10]);
		delaySMG(100);
		offSMG();
	}
	else if(SMG_mode==3)
	{
		displaySMG(0,SMG_number[14]);
		delaySMG(100);
		displaySMG(5,SMG_number[v/100]);
		delaySMG(100);
		displaySMG(6,SMG_number[(v%100)/10]);
		delaySMG(100);
		displaySMG(7,SMG_number[v%10]);
		delaySMG(100);
		offSMG();
	}
}

void scanKey()
{
	R1=0;
	C2=1;
	if(C2==0)
	{
		delaySMG(100);
		if(C2==0)
		{
			if(SMG_mode==3)
			{
				SMG_mode=1;
			}
			else
			{
				SMG_mode++;
			}
		}
	}
	while(C2==0)
	{
		showSMG();	
	}
}