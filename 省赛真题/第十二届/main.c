#include <STC15F2K60S2.H>
#include <intrins.h>
#include "iic.h"
sbit S7=P3^0;
sbit S6=P3^1;
sbit S5=P3^2;
sbit S4=P3^3;
unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	
//U-0xc1,F-0x8e,n-0xc8
unsigned char SMG_mode=0;//数码管显示模式
unsigned char LED_mode=0;//LED开闭控制，0为开
unsigned char V_mode=1;//电压选取,1表示通道1，3表示通道3
unsigned int v1;//从光敏电阻读取电压
unsigned int v3;//从滑动电阻读取电压
unsigned int v;//显示在数码管上的电压
unsigned int v3_pro=0;//存在AT24C02中的电压
unsigned char count_S7=0;//长按S7计时
unsigned char S7_key=0;
unsigned int dat_f=0;//显示频率

unsigned int dat_f_AT=0;//从eeprom中读取的
unsigned int count_f=0;//频率计数
unsigned long dat_cycle=0;//显示周期
unsigned char count_t=0;//计时器计时

void SelectHC(unsigned char channel);
void InitSys();//初始化，关闭蜂鸣器继电器
void displaySMG(unsigned char pos,unsigned char value);//数码管控制
void showSMG();//数码管显示
void offSMG();//数码管关闭
void showLED();//LED显示
void scanKey();//按键控制
void delaySMG(unsigned int t);

void InitTimer();

void main()
{
	InitTimer();
	InitSys();
	while(1)
	{
		v1=((unsigned int)readPCF(0x01))*100/51;//PCF两次读取间隔
		v3=((unsigned int)readPCF(0x03))*100/51;

		dat_cycle=1000000/dat_f;
		if(V_mode==1)
		{
			v=v1;
		}
		else
		{
			v=v3;
		}
		scanKey();
		showSMG();
		showLED();
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
void InitSys()
{
	SelectHC(4);
	P0=0xff;
	SelectHC(5);
	P0=P0&0xaf;
}
void displaySMG(unsigned char pos,unsigned char value)
{
	SelectHC(7);
	P0=0xff;
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
	if(SMG_mode==0)//显示频率
	{
		displaySMG(0,0x8e);
		delaySMG(100);
		if(dat_f>9999)
		{
			displaySMG(3,SMG_number[dat_f/10000]);
			delaySMG(100);
			
		}
		if(dat_f>999)
		{
			displaySMG(4,SMG_number[(dat_f/1000)%10]);
			delaySMG(100);
		}
		if(dat_f>99)
		{
			displaySMG(5,SMG_number[(dat_f/100)%10]);
			delaySMG(100);
		}
		if(dat_f>9)
		{
			displaySMG(6,SMG_number[(dat_f/10)%10]);
			delaySMG(100);
		}
		displaySMG(7,SMG_number[dat_f%10]);
		delaySMG(100);
		offSMG();
	}
	else if(SMG_mode==1)
	{
		displaySMG(0,0xc8);
		delaySMG(100);
		if(dat_cycle>999999)
		{
			displaySMG(1,SMG_number[dat_cycle/1000000]);
			delaySMG(100);
		}
		if(dat_cycle>99999)
		{
			displaySMG(2,SMG_number[(dat_cycle/100000)%10]);
			delaySMG(100);
		}
		if(dat_cycle>9999)
		{
			displaySMG(3,SMG_number[(dat_cycle/10000)%10]);
			delaySMG(100);
		}
		if(dat_cycle>999)
		{
			displaySMG(4,SMG_number[(dat_cycle/1000)%10]);
			delaySMG(100);
		}
		if(dat_cycle>99)
		{
			displaySMG(5,SMG_number[(dat_cycle/100)%10]);
			delaySMG(100);
		}
		if(dat_cycle>9)
		{
			displaySMG(6,SMG_number[(dat_cycle/10)%10]);
			delaySMG(100);
		}
		displaySMG(7,SMG_number[dat_cycle%10]);
		delaySMG(100);
		offSMG();
	}
	else if(SMG_mode==2)//电压界面
	{
		displaySMG(0,0xc1);
		delaySMG(100);
		displaySMG(1,SMG_number[16]);
		delaySMG(100);
		displaySMG(2,SMG_number[V_mode]);
		delaySMG(100);
		displaySMG(5,SMG_Dotnumber[v/100]);
		delaySMG(100);
		displaySMG(6,SMG_number[(v%100)/10]);
		delaySMG(100);
		displaySMG(7,SMG_number[v%10]);
		delaySMG(100);
		offSMG();	
	}
}
void Delay6ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 65;
	j = 136;
	do
	{
		while (--j);
	} while (--i);
}

void scanKey()
{
	if(S4==0)
	{
		delaySMG(100);
		if(S4==0)
		{
			if(SMG_mode==2)
			{
				SMG_mode=0;
			}
			else
			{
				SMG_mode++;
			}
		}
		while(S4==0)
		{
			showSMG();
			showLED();
		}
	}
	if(S5==0)
	{
		delaySMG(100);
		if(S5==0)
		{
			if(V_mode==1)
			{
				V_mode=3;
				v=v3;
			}
			else
			{
				V_mode=1;
				v=v1;
			}
		}
		while(S5==0)
		{
			showSMG();
			showLED();
		}
	}
	if(S6==0)
	{
		delaySMG(100);
		if(S6==0)
		{
			writeAT24(0x00,v3/10);
			v3_pro=v3;
		}
		while(S6==0)
		{
			showSMG();
			showLED();
		}
	}
	if(S7==0)
	{
		delaySMG(100);
		if(S7==0)
		{
			S7_key=1;
			writeAT24(0x01,dat_f/256);
			Delay6ms();
			writeAT24(0x02,dat_f/256);
			Delay6ms();
			dat_f_AT=dat_f;
		}
		while(S7==0)
		{
			showSMG();
			showLED();
		}
		S7_key=0;
		count_S7=0;
	}
}
void InitTimer()
{
	TMOD = 0x16;                    //设置定时器为模式0(16位自动重装载)
	TL1 = (65535-50000+1)%256;                     //初始化计时值
    TH1 = (65535-50000+1)/256;
	TH0=0xff;
	TL0=0xff;
	TR0=1;
	ET0=1;
    TR1 = 1;                        //定时器1开始计时
    ET1 = 1;                        //使能定时器0中断
    EA = 1;
}
void ServiceTimer0() interrupt 1
{
	count_f++;
}

void ServiceTimer1() interrupt 3
{
	TL1 = (65535-50000+1)%256;                     //初始化计时值
    TH1 = (65535-50000+1)/256;
	count_t++;
	if(count_t==20)
	{
		dat_f=count_f;
		count_f=0;
		count_t=0;
	}
	if(S7_key==1)
	{
		count_S7++;
		if(count_S7==20)
		{
			if(LED_mode==0)
			{
				LED_mode=1;
			}
			else
			{
				LED_mode=0;
			}
		}
	}
	
}
void showLED()
{
	SelectHC(4);
	P0=0xff;
	if(LED_mode==0)
	{
		if(SMG_mode==0)//L3点亮
		{
			P0=P0&0xfb;
		}
		else if(SMG_mode==1)//L4点亮
		{
			P0=P0&0xf7;
		}
		else//L5点亮
		{
			P0=P0&0xef;
		}
		if(v3>v3_pro)//点亮L1
		{
			P0=P0&0xfe;
		}
		if(dat_f>dat_f_AT)//点亮L2
		{
			P0=P0&0xfd;
		}
		P0=0xff;
	}
	else
	{
		P0=0xff;
	}
}
