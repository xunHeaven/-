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
unsigned char SMG_mode=0;//数码管显示模式，0为电压显示界面，1为频率显示界面
unsigned char LED_mode=0;//LED显示模式，0为电压模式，1为频率模式
unsigned char S5_key=0;//电压输出模式转换，0时电压值为2，1时电压值根据RB2变化。按下S5实现模式转换
unsigned char S6_key=0;//LED灯启用模式，0时点亮，1时关闭。按下S6实现模式转换
unsigned char S7_key=0;//数码管启用模式，0时点亮，1时关闭。按下S7实现模式转换

//下面三个是为了计算频率
unsigned int dat_f=0;//显示在数码管中的频率值，每秒计算；
unsigned int count_f=0;//在定时器0中计算一秒的频率
unsigned char count_t=0;//记录时间，一秒后更新
//下面两个变量是为了表示电压
unsigned int v1=200;//表示数码管显示电压，初始化为固定电压2
unsigned int v2;//RB2中的电压

//函数声明
void InitSys();//初始化单片机，关掉蜂鸣器继电器等无关设备
void SelectHC(unsigned char channel);//选择HC573锁存器
void display_SMG(unsigned char pos,unsigned char value);//选择点亮的数码管和点亮内容
void off_SMG();//关掉所有数码管
void delaySMG(unsigned int t);//数码管延时函数
void show_SMG();//数码管显示控制函数
void show_LED();//LED显示函数
void scanKey();//按键操作函数
void InitTimer();//定时器函数初始化
//void ServiceTimer0() interrupt 1;//定时器0，用于频率计数
//void ServiceTimer1() interrupt 3;//定时器1，用于计时
 

void main()
{
	InitSys();
	InitTimer();
	while(1)
	{
		v2=((unsigned char)readPCF(0x03))*100/51;
		if(S5_key==0)
		{
			v1=200;
		}
		else
		{
			v1=v2;
		}
		scanKey();
		show_SMG();
		show_LED();
	}
}
void InitSys()
{
	SelectHC(4);
	P0=0xff;
	SelectHC(5);
	P0=P0&0xaf;
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
void display_SMG(unsigned char pos,unsigned char value)
{
	SelectHC(7);
	P0=0xff;
	SelectHC(6);
	P0=0x01<<pos;
	SelectHC(7);
	P0=value;
}
void off_SMG()
{
	SelectHC(6);
	P0=0xff;
	SelectHC(7);
	P0=0xff;
}
void delaySMG(unsigned int t)
{
	while(t--);
}
void show_SMG()
{
	if(S7_key==0)//数码管启用
	{
		if(SMG_mode==0)//数码管显示电压
		{
			display_SMG(0,0xc1);
			delaySMG(100);
			display_SMG(5,SMG_Dotnumber[v1/100]);
			delaySMG(100);
			display_SMG(6,SMG_number[(v1%100)/10]);
			delaySMG(100);
			display_SMG(7,SMG_number[v1%10]);
			delaySMG(100);
			off_SMG();
		}
		else if(SMG_mode==1)//数码管显示频率
		{
			display_SMG(0,0x8e);
			delaySMG(100);
			if(dat_f>9999)
			{
				display_SMG(3,SMG_number[dat_f/10000]);
				delaySMG(100);
			}
			if(dat_f>999)
			{
				display_SMG(4,SMG_number[(dat_f/1000)%10]);
				delaySMG(100);
			}
			if(dat_f>99)
			{
				display_SMG(5,SMG_number[(dat_f/100)%10]);
				delaySMG(100);
			}
			if(dat_f>9)
			{
				display_SMG(6,SMG_number[(dat_f/10)%10]);
				delaySMG(100);
			}
			display_SMG(7,SMG_number[dat_f%10]);
			delaySMG(100);
			off_SMG();
		}
	}
	else//S7_key==1,数码管关闭
	{
		off_SMG();
	}
}

void scanKey()
{
	if(S7==0)
	{
		delaySMG(100);
		if(S7==0)
		{
			if(S7_key==0)
			{
				S7_key=1;
			}
			else
			{
				S7_key=0;
			}
		}
		while(S7==0)
		{
			show_SMG();
			show_LED();
		}
	}
	if(S6==0)
	{
		delaySMG(100);
		if(S6==0)
		{
			if(S6_key==0)
			{
				S6_key=1;
			}
			else
			{
				S6_key=0;
			}
		}
		while(S6==0)
		{
			show_SMG();
			show_LED();
		}
	}
	if(S5==0)
	{
		delaySMG(100);
		if(S5==0)
		{
			if(S5_key==0)
			{
				S5_key=1;
				v1=v2;
			}
			else
			{
				S5_key=0;
				v1=200;
			}
		}
		while(S5==0)
		{
			show_SMG();
			show_LED();
		}
	}
	if(S4==0)
	{
		delaySMG(100);
		if(S4==0)
		{
			if(SMG_mode==0)
			{
				SMG_mode=1;
				LED_mode=1;
			}
			else
			{
				SMG_mode=0;
				LED_mode=0;
			}
		}
		while(S4==0)
		{
			show_SMG();
			show_LED();
		}
	}
}
void show_LED()
{
	if(S6_key==0)
	{
		if(LED_mode==0)//电压显示状态
		{
			SelectHC(4);
			P0=0xfe;//L1点亮，L2熄灭
			if(S5_key==1)//用Rb2测量的电压，则L5点亮；
			{
				P0=P0&0xef;
				if((v1>=150&&v1<250)||(v1>=350))//L3点亮
				{
					P0=P0&0xfb;
				}
				else//L3熄灭
				{
					P0=P0|0x04;
				}
			}
			else//L3和L5熄灭
			{
				P0=P0|0x14;
			}
			
		}
		else//频率显示状态
		{
			SelectHC(4);
			P0=0xfd;//L2点亮，L1熄灭
			if((dat_f>=1000&&dat_f<5000)||(dat_f>=10000))//L4点亮
			{
				P0=P0&0xf7;
			}
			else//熄灭L4，只点亮L2
			{
				P0=0xfd;
			}
		}
	}
	else
	{
		SelectHC(4);
		P0=0xff;
	}
}

void InitTimer()
{
	TMOD = 0x16;                    //高4为控制定时器1，表示16位非自动重装载；低4位控制定时器0，表示8位自动重装载
    TL1 =(65535-50000+1)%256;	//初始化计时值
    TH1 = (65535-50000+1)/256;
	TL0=0xff;
	TH0=0xff;
	
	TR0=1;
	ET0=1;
    TR1 = 1;                        //定时器1开始计时
    ET1 = 1;                        //使能定时器1中断
    EA = 1;
}

void ServiceTimer0() interrupt 1
{
	count_f++;
}

void ServiceTimer1() interrupt 3//50ms
{
	TL1 =(65535-50000+1)%256;	//初始化计时值
	TH1 = (65535-50000+1)/256;
	count_t++;
	if(count_t==20)
	{
		dat_f=count_f;
		count_f=0;
		count_t=0;
	}
}