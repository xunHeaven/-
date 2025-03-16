#include <STC15F2K60S2.H>

sbit S7=P3^0;
sbit S6=P3^1;
sbit S5=P3^2;
sbit S4=P3^3;

sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
sbit L5=P0^4;
sbit L6=P0^5;
sbit L7=P0^6;
sbit L8=P0^7;


sbit HC138_A=P2^5;
sbit HC138_B=P2^6;
sbit HC138_C=P2^7;


void HC138_ctrl(unsigned int hc)
{
	switch(hc)
	{
		case 4:
			HC138_C=1;
			HC138_B=0;
			HC138_A=0;
			break;
	}
}
void delay(unsigned int t)
{
	while(t--);
}
void ScanKeys_alone()
{
	if(S7==0)
	{
		delay(100);
		if(S7==0)
		{
			L1=0;
			while(S7==0);
			L1=1;
		}
	}
	if(S6==0)
	{
		delay(100);
		if(S6==0)
		{
			L2=0;
			while(S6==0);
			L2=1;
		}
	}
	if(S5==0)
	{
		delay(100);
		if(S5==0)
		{
			L3=0;
			while(S5==0);
			L3=1;
		}
	}
	if(S4==0)
	{
		delay(100);
		if(S4==0)
		{
			L4=0;
			while(S4==0);
			L4=1;
		}
	}
}
void main(void)
{
	HC138_ctrl(4);
	while(1)
	{
		ScanKeys_alone();
	}
}





