#include <STC15F2K60S2.H>

sbit L1=P0^0;
sbit L8=P0^7;

void HC138_ctrl()
{
	P2=(P2&0x1f)|0x80;
}
//-----------------
void InitTimer0()
{
	TMOD=0x01;
	TH0=(65535-50000)/256;
	TL0=(65535-50000)%256;
	
	ET0=1;
	EA=1;
	TR0=1;
}

unsigned int count=0;
void serviceTimer0()
{
	TH0=(65535-50000)/256;
	TL0=(65535-50000)%256;
	
	count++;
	if(count%10==0)
	{
		L1=~L1;
	}
	if(count==100)
	{
		L8=~L8;
		count=0;
	}
	
}

void main()
{
	HC138_ctrl();
	InitTimer0();
	while(1)
	{
	
	}
}