#include <STC15F2K60S2.H>

sbit S4=P3^3;
sbit S5=P3^2;
unsigned int t_min=0;
unsigned int t_sec=0;
unsigned int t_ms=0;
unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};
	void delay(unsigned int t)
{
	while(t--);
}
void SelectHC573(unsigned char channel)
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
void showSMG(unsigned char value,unsigned char pos)
{
	SelectHC573(7);
	P0=0xff;
	SelectHC573(6);
	P0=(0x01<<pos);
	SelectHC573(7);
	P0=value;
}

void displayTimer()
{
	showSMG(SMG_number[t_ms%10],7);
	delay(500);
	showSMG(SMG_number[t_ms/10],6);
	delay(500);
	
	showSMG(SMG_number[16],5);
	delay(500);
	
	showSMG(SMG_number[t_sec%10],4);
	delay(500);
	showSMG(SMG_number[t_sec/10],3);
	delay(500);
	
	showSMG(SMG_number[16],2);
	delay(500);
	
	showSMG(SMG_number[t_min%10],1);
	delay(500);
	showSMG(SMG_number[t_min/10],0);
	delay(500);
}
//------------定时器
void InitTimer0()
{
	TMOD=0x01;
	TH0=(65535-50000)/256;
	TL0=(65535-50000)%256;
	
	ET0=1;
	EA=1;
	TR0=1;
}
void ServiceTimer0() interrupt 1
{
	TH0=(65535-50000)/256;
	TL0=(65535-50000)%256;
	t_ms++;
	if(t_ms==20)
	{
		t_sec++;
		t_ms=0;
		if(t_sec==60)
		{
			t_min++;
			t_sec=0;
		}
		if(t_min==99)
		{
			t_min=0;
		}
	}
	
}
//--------------

void ScanKey()
{
	if(S4==0)//秒表启动与暂停
	{
		delay(100);
		if(S4==0)
		{
			TR0=~TR0;
			while(S4==0)
			{
				displayTimer();
			}
		}
	}
	if(S5==0)//秒表清零
	{
		delay(100);
		if(S5==0)
		{
			t_min=0;
			t_sec=0;
			t_ms=0;
			while(S5==0)
			{
				displayTimer();
			}
		}
	}
}


void main()
{
	InitTimer0();
	while(1)
	{
		displayTimer();
		ScanKey();
	}
}