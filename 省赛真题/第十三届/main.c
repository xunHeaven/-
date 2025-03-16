#include <intrins.h>
#include <STC15F2K60S2.H>
#include "ds1302.h"
#include "onewire.h"
#include <stdio.h>

sbit relay=P0^4;//继电器
sbit buzzer=P0^6;
sbit R1=P3^3;
sbit R2=P3^2;
sbit C1=P3^5;
sbit C2=P3^4;
sbit L1=P0^0;
sbit L3=P0^2;//3号LED灯
sbit L2=P0^1;


unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	
//U-0xc1
unsigned char SMG_mode=1;//数码管显示模式
unsigned char LED_mode=0;//0为温度控制标记，1为时间控制
unsigned int temp;//存储温度
unsigned int temp1=23;//这是给定的温度，将与读出的进行比较
unsigned char S17key=0;//s17按下标记
unsigned char zhengdian=0;//整点标记，为0时非整点
unsigned char relayKey=0;//继电器控制标记，为0时断开，为1时吸合；
unsigned int count_t=0;//计时器计数；
unsigned char count_5s=0;//5s计时
unsigned char L3key=1;//3号LED灯

void Initsys();//对单片机进行初始化
void SelectHC(unsigned char channel);
void display_SMG(unsigned char pos,unsigned char value);//数码管位选和段选
void offSMG();//关掉所有数码管
void showSMG();//数码管显示控制
void showLED();//LED闪烁控制
void selectLED();
void scanKey();//按键控制函数
void ctrlRelay(unsigned char relayKey);//继电器控制函数
void delaySMG(unsigned char t);//延时函数、
void InitTimer();

void main()
{
	Initsys();
	InitTimer();
	write_date();
	while(1)
	{
		read_date();
		temp=read_Temp();
		if((Timer[0]==0)&&(Timer[1]==0))
		{
			zhengdian=1;//整点标记启动
			count_5s=100;
		}
		scanKey();
		showSMG();
		showLED();
	}
}
void InitTimer()//50ms
{
	TMOD = 0x01;                    //设置定时器为模式0(16位自动重装载)
    TL0 = (65535-50000+1)%256;                     //初始化计时值
    TH0 = (65536-50000+1)/256;
    TR0 = 1;                        //定时器0开始计时
    ET0= 1;                        //使能定时器0中断
    EA = 1;
}


void ServiceTimer() interrupt 1
{
	TL0 = (65535-50000+1)%256;                     //初始化计时值
	TH0 = (65536-50000+1)/256;
	
	if(zhengdian==1)
	{
		count_5s--;//倒计时5s
		if(count_5s==0)
		{
			zhengdian=0;
		}
	}
	if(relayKey==1)
	{
		count_t++;
		if(count_t==2)
		{
			if(L3key==1)
			{
				L3key=0;
			}
			else
			{
				L3key=1;
			}
			count_t=0;
		}
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
void ctrlRelay(unsigned char relayKey)
{
	SelectHC(5);
	if(relayKey==0)//继电器吸合
	{
		relay=1;
		buzzer=0;

	}
	else if(relayKey==1)//继电器关闭
	{
		relay=0;
		buzzer=0;

	}
}
void Initsys()
{
	SelectHC(5);
	P0=0xaf;
	SelectHC(4);
	P0=0xff;
}
void display_SMG(unsigned char pos, unsigned char value)
{
	
	SelectHC(6);
	P0=(0x01<<pos);
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
	if(SMG_mode==1)//显示温度
	{
		display_SMG(0,0xc1);
		delaySMG(100);
		display_SMG(1,SMG_number[SMG_mode]);
		delaySMG(100);
		display_SMG(5,SMG_number[temp/100]);
		delaySMG(100);
		display_SMG(6,SMG_Dotnumber[(temp%100)/10]);
		delaySMG(100);
		display_SMG(7,SMG_number[temp%10]);
		delaySMG(100);
		offSMG();
	}
	else if(SMG_mode==2)//显示时间
	{
		read_date();
		if(S17key==0)
		{
			display_SMG(0,0xc1);
			delaySMG(100);
			display_SMG(1,SMG_number[SMG_mode]);
			delaySMG(100);
			display_SMG(3,SMG_number[Timer[2]/16]);//显示时
			delaySMG(100);
			display_SMG(4,SMG_number[Timer[2]%16]);
			delaySMG(100);
			
			display_SMG(5,0xbf);
			delaySMG(100);
			
			display_SMG(6,SMG_number[Timer[1]/16]);//显示分
			delaySMG(100);
			display_SMG(7,SMG_number[Timer[1]%16]);
			delaySMG(100);
			offSMG();
			
		}
		else if(S17key==1)
		{
			display_SMG(0,0xc1);
			delaySMG(100);
			display_SMG(1,SMG_number[SMG_mode]);
			delaySMG(100);
			display_SMG(3,SMG_number[Timer[1]/16]);//显示分
			delaySMG(100);
			display_SMG(4,SMG_number[Timer[1]%16]);
			delaySMG(100);
			
			display_SMG(5,0xbf);
			delaySMG(100);
			
			display_SMG(6,SMG_number[Timer[0]/16]);//显示秒
			delaySMG(100);
			display_SMG(7,SMG_number[Timer[0]%16]);
			delaySMG(100);
			offSMG();
		}
	}
	else if(SMG_mode==3)//参数设置界面
	{
		display_SMG(0,0xc1);
		delaySMG(100);
		display_SMG(1,SMG_number[SMG_mode]);
		delaySMG(100);
		
		display_SMG(6,SMG_number[temp1/10]);
		delaySMG(100);
		display_SMG(7,SMG_number[temp1%10]);
		delaySMG(100);
		offSMG();
	}
}

void scanKey()
{
	R1=0;
	R2=1;
	C1=C2=1;
	if(C1==0)//S12
	{
		delaySMG(100);
		if(C1==0)
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
		while(C1==0)
		{
			showSMG();
			showLED();
		}
	}
	else if(C2==0)//S16
	{
		delaySMG(100);
		if(C2==0)
		{
			if(SMG_mode==3)
			{
				if(temp1==99)
				{
					temp1=10;
				}
				else
				{
					temp1++;
				}
			}
		}
		while(C2==0)
		{
			showSMG();
			showLED();
		}
	}
	
	R2=0;
	R1=1;
	C1=C2=1;
	if(C1==0)//S13
	{
		delaySMG(100);
		if(C1==0)
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
		while(C1==0)
		{
			showSMG();
			showLED();
		}
	}
	else if(C2==0)//S17
	{
		delaySMG(100);
		if(C2==0)
		{
			if(SMG_mode==2)
			{
				S17key=1;
			}
			if(SMG_mode==3)
			{
				if(temp1==10)
				{
					temp1=99;
				}
				else
				{
					temp1--;
				}
			}
		}
		while(C2==0)
		{
			showSMG();
			showLED();
		}
		S17key=0;
	}
}
void selectLED()
{
	
	SelectHC(4);
	P0=0xff;
	
}
void showLED()
{
	selectLED();
	if(zhengdian==1)
	{
		SelectHC(4);
		L1=0;
	}
	if(LED_mode==0)//温度控制模式
	{
		SelectHC(4);
		L2=0;
		if(temp>(temp1*10))
		{
			SelectHC(4);
			L3=L3key;
			ctrlRelay(0);
			selectLED();
			relayKey=1;
		}
		else
		{
			SelectHC(4);
			L3=1;
			ctrlRelay(1);
			selectLED();
			relayKey=0;
		}
	}
	else if(LED_mode==1)//时间控制模式
	{
		if(zhengdian==1)
		{
			SelectHC(4);
			L3=L3key;
			ctrlRelay(0);
			selectLED();
			relayKey=1;
		}
		else
		{
			SelectHC(4);
			L3=1;
			ctrlRelay(1);
			selectLED();
			relayKey=0;
		}
	
	}
	SelectHC(4);
	P0=0xff;
	
}