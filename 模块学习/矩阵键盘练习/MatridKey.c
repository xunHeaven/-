#include <STC15F2K60S2.H>

sbit HC138_A=P2^5;
sbit HC138_B=P2^6;
sbit HC138_C=P2^7;

sbit R1=P3^0;
sbit R2=P3^1;
sbit R3=P3^2;
sbit R4=P3^3;

sbit C1=P4^4;
sbit C2=P4^2;
sbit C3=P3^5;
sbit C4=P3^4;

unsigned int key=18;

unsigned int code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,
	0xbf,0x7f};
void delay(unsigned int t)
{
	while(t--);
}
void HC138_ctrl(unsigned int hc)
{
	switch(hc)
	{
		case 6:
			HC138_C=1;
			HC138_B=1;
			HC138_A=0;
			break;
		case 7:
			HC138_C=1;
			HC138_B=1;
			HC138_A=1;
			break;
	}
}
void SMG_show(unsigned int number)
{
	HC138_ctrl(6);
	P0=0x01;
	HC138_ctrl(7);
	if(number!=18)
	{
		P0=SMG_number[number];
	}
	else
	{
		P0=0xff;
	}
}

void Matri_Key()
{
	R1=R2=R3=R4=0;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		delay(100);
		if(C1==0)
		{
			key=0;
		}
	}
	else if(C2==0)
	{
		delay(100);
		if(C2==0)
		{
			key=1;
		}
	}
	else if(C3==0)
	{
		delay(100);
		if(C3==0)
		{
			key=2;
		}
	}
	else if(C4==0)
	{
		delay(100);
		if(C4==0)
		{
			key=3;
		}
	}
	
	C1=C2=C3=C4=0;
	R1=R2=R3=R4=1;
	if(R1==0)
	{
		delay(100);
		if(R1==0)
		{
			key=key;
		}
		while(R1==0)
		{
			SMG_show(key);
		}
		key=18;
		SMG_show(key);

	}
	else if(R2==0)
	{
		delay(100);
		if(R2==0)
		{
			key=key+4;
		}
		while(R2==0)
		{
			SMG_show(key);
		}
		key=18;
		SMG_show(key);

	}
	else if(R3==0)
	{
		delay(100);
		if(R3==0)
		{
			key=key+8;
		}
		while(R3==0)
		{
			SMG_show(key);
		}
		key=18;
		SMG_show(key);

	}
	else if(R4==0)
	{
		delay(100);
		if(R4==0)
		{
			key=key+12;
		}
		while(R4==0)
		{
			SMG_show(key);
		}
		key=18;
		SMG_show(key);

	}

}

void main()
{
	while(1)
	{
		Matri_Key();
	}
}

